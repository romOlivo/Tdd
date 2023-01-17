#include <map>
#include <omp.h>
#include <cmath>
#include <chrono>
#include <thread>
#include <iostream>
using namespace std;

//  -------------------- External support functions -------------------- 

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
};

//  ---------------------- Interface declaration ----------------------- 

class IUniqueTable;

class IComputeTable;

class IEdge;

class INode {
    public:
        virtual string getString() = 0;
        virtual IEdge* getLeftEdge() = 0;
        virtual IEdge* getRightEdge() = 0;
        virtual long getProduct(long n) = 0;
        virtual long getProductParallel(long n) = 0;
        virtual IEdge* getDDProduct(long n, IUniqueTable* ut, IComputeTable* ct) = 0;
        virtual IEdge* getDDProductParallel(long n, IUniqueTable* ut, IComputeTable* ct, int level) = 0;
        virtual IEdge* getDDProductParallelCached(long n, IUniqueTable* ut, IComputeTable* ct, int level) = 0;
        virtual IEdge* getDDProductParallelPrivate(long n, IUniqueTable* ut, IComputeTable* ct, int level) = 0;
};

class IEdge {
    public:
        virtual long getValue() = 0;
        virtual INode* getNode() = 0;
        virtual long getProduct() = 0;
        virtual string getString() = 0;
        virtual long getProductParallel() = 0;
        virtual IEdge* getDDProduct(IUniqueTable* ut, IComputeTable* ct) = 0;
        virtual IEdge* getDDProductParallel(IUniqueTable* ut, IComputeTable* ct) = 0;
        virtual IEdge* getDDProductParallelCached(IUniqueTable* ut, IComputeTable* ct) = 0;
        virtual IEdge* getDDProductParallel(IUniqueTable* ut, IComputeTable* ct, int level) = 0;
        virtual IEdge* getDDProductParallelCached(IUniqueTable* ut, IComputeTable* ct, int level) = 0;
        virtual IEdge* getDDProductParallelPrivate(IUniqueTable* ut, IComputeTable* ct, int level) = 0;
};

class IComputeTable {
    public:
        virtual IEdge* lookup(IEdge* edge) = 0;
        virtual void insert(IEdge* inputEdge, IEdge* resultEdge) = 0;
};

class IUniqueTable {
    public:
        virtual INode* lookup(INode* node) = 0;
};

class IDD {
    public:
        virtual IUniqueTable* getUniqueTable() = 0;
};

//  ------------------- Implementation of interfaces ------------------- 

class UniqueTable : public IUniqueTable {
    // Constructors
    public:
        UniqueTable() {
            omp_init_lock(&insertLock);
        }
    // Methods
    public:
        INode* lookup(INode* node) {
            omp_set_lock(&insertLock);
            INode* dev = node;
            if (table.find(node->getString()) == table.end()) {
                insert(node);
            } else {
                dev = table[node->getString()];
            }
            omp_unset_lock(&insertLock);
            return dev;
        }
        void insert(INode* node) {
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
            table[node->getString()] = node;
        }
    private:
        std::map<string, INode*> table;
        omp_lock_t insertLock;
};

class UniqueTablePrivate : public IUniqueTable {
    // Constructors
    public:
        UniqueTablePrivate() {
        }
    // Methods
    public:
        INode* lookup(INode* node) {
            INode* dev = node;
            if (table.find(node->getString()) == table.end()) {
                insert(node);
            } else {
                dev = table[node->getString()];
            }
            return dev;
        }
        void insert(INode* node) {
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
            table[node->getString()] = node;
        }
    private:
        std::map<string, INode*> table;
};

class CachedUniqueTable : public IUniqueTable {
    // Constructors
    public:
        CachedUniqueTable(IUniqueTable* ut) {
            this->ut = ut;
        }
    // Methods
    public:
        INode* lookup(INode* node) {
            INode* dev = node;
            if (table.find(node->getString()) == table.end()) {
                insert(node);
            } else {
                dev = table[node->getString()];
            }
            return dev;
        }
        void insert(INode* node) {
            node = ut->lookup(node);
            table[node->getString()] = node;
        }
    private:
        std::map<string, INode*> table;
        IUniqueTable* ut;
};

class ComputeTable : public IComputeTable {
    // Constructors
    public:
        ComputeTable() {
            omp_init_lock(&insertLock);
        }
    // Methods
    public: 
        IEdge* lookup(IEdge* edge) {
            omp_set_lock(&insertLock);
            IEdge* dev = nullptr;
            if (table.find(edge->getString()) != table.end()) {
                dev = table[edge->getString()];
            }
            omp_unset_lock(&insertLock);
            return dev;
        }
        void insert(IEdge* inputEdge, IEdge* resultEdge) {
            omp_set_lock(&insertLock);
            table[inputEdge->getString()] = resultEdge;
            #pragma omp flush
            omp_unset_lock(&insertLock);
        }

    private:
        std::map<string, IEdge*> table;
        omp_lock_t insertLock;
};

class CachedComputeTable : public IComputeTable {
    // Constructors
    public:
        CachedComputeTable(IComputeTable* ct) {
            this->ct = ct;
        }
    // Methods
    public: 
        IEdge* lookup(IEdge* edge) {
            IEdge* dev = nullptr;
            auto s = edge->getString();
            if (table.find(s) != table.end()) {
                dev = table[s];
            } else {
                dev = ct->lookup(edge);
                table[s] = dev;
            }
            return dev;
        }
        void insert(IEdge* inputEdge, IEdge* resultEdge) {
            table[inputEdge->getString()] = resultEdge;
            ct->insert(inputEdge, resultEdge);
        }

    private:
        std::map<string, IEdge*> table;
        IComputeTable* ct;
};

class Edge: public IEdge {

    // Constructors
    public:
        Edge(long n, INode* node) {
            this->node = node;
            this->n = n;
        }

    // Interface methods
    public:
        long getValue() {
            return n;
        }
        
        long getProduct() {
            return node->getProduct(n);
        }

        INode* getNode() {
            return node;
        }

        string getString() {
            return  string_format("%i%i", n, node);
        }

        long getProductParallel() {
            return node->getProductParallel(n);
        }

        IEdge* getDDProduct(IUniqueTable* ut, IComputeTable* ct) {
            IEdge* edge = ct->lookup(this);
            if (edge == nullptr) {
                edge = node->getDDProduct(n, ut, ct);
                ct->insert(this, edge);
            }
            return edge;
        }

        IEdge* getDDProductParallel(IUniqueTable* ut, IComputeTable* ct) {
            return getDDProductParallel(ut, ct, 1);
        }

        IEdge* getDDProductParallel(IUniqueTable* ut, IComputeTable* ct, int level) {
            IEdge* edge = ct->lookup(this);
            if (edge == nullptr) {
                edge = node->getDDProductParallel(n, ut, ct, level);
                ct->insert(this, edge);
            }
            return edge;
        }

        IEdge* getDDProductParallelCached(IUniqueTable* ut, IComputeTable* ct) {
            return getDDProductParallelCached(ut, ct, 1);
        }

        IEdge* getDDProductParallelCached(IUniqueTable* ut, IComputeTable* ct, int level) {
            IEdge* edge = ct->lookup(this);
            if (edge == nullptr) {
                edge = node->getDDProductParallelCached(n, ut, ct, level);
                ct->insert(this, edge);
            }
            return edge;
        }

        IEdge* getDDProductParallelPrivate(IUniqueTable* ut, IComputeTable* ct, int level) {
            IEdge* edge = ct->lookup(this);
            if (edge == nullptr) {
                edge = node->getDDProductParallelPrivate(n, ut, ct, level);
                ct->insert(this, edge);
            }
            return edge;
        }

    private:
        INode* node;
        long n;
};

class Node : public INode {
    // Constructors
    public:
        Node(IEdge* leftEdge, IEdge* rightEdge) {
            this->leftEdge = leftEdge;
            this->rightEdge = rightEdge;
        }
        Node() {
            this->leftEdge = nullptr;
            this->rightEdge = nullptr;
        }
    // Public methods
    public:

        IEdge* getLeftEdge() {
            return leftEdge;
        }

        IEdge* getRightEdge() {
            return rightEdge;
        }

        long getProduct(long n) {
            long value = n;
            if (leftEdge != nullptr)
                value = (value * leftEdge->getProduct()) % MOD_NUMBER;
            if (rightEdge != nullptr)
                value = (value * rightEdge->getProduct()) % MOD_NUMBER;
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return value;
        }

        long getProductParallel(long n) {
            long leftValue = 1;
            long rightValue = 1;
            #pragma omp parallel num_threads(2) shared(leftValue, rightValue)
            {
                #pragma omp single 
                {
                    if (leftEdge != nullptr)
                        #pragma omp task
                        leftValue = leftEdge->getProduct();
                    if (rightEdge != nullptr)
                        #pragma omp task
                        rightValue = rightEdge->getProduct();
                }
            }
            return ((n * leftValue) % MOD_NUMBER * rightValue) % MOD_NUMBER;
        }

        IEdge* getDDProduct(long n, IUniqueTable* ut, IComputeTable* ct) {
            long value = n;
            IEdge* leftEdge = nullptr;
            IEdge* rightEdge = nullptr;
            if (this->leftEdge != nullptr) {
                leftEdge  = this->leftEdge->getDDProduct(ut, ct);
                value = (value *  leftEdge->getValue()) % MOD_NUMBER;
            }
            if (this->rightEdge != nullptr) {
                rightEdge = this->rightEdge->getDDProduct(ut, ct);
                value = (value * rightEdge->getValue()) % MOD_NUMBER;
            }
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            auto node = ut->lookup(new Node(leftEdge, rightEdge));
            return new Edge(value, node);
        }

        IEdge* getDDProductParallel(long n, IUniqueTable* ut, IComputeTable* ct, int level) {
            long value = n;
            long leftValue = 1;
            long rightValue = 1;
            IEdge* leftEdge = nullptr;
            IEdge* rightEdge = nullptr;
            if (level == 0)
                return this->getDDProduct(n, ut, ct);
            #pragma omp task shared(leftValue, rightValue, leftEdge, rightEdge, level)
            {
                if (this->leftEdge != nullptr) {
                    leftEdge = this->leftEdge->getDDProductParallel(ut, ct, level-1);
                    leftValue = leftEdge->getValue();
                }
            }
            #pragma omp task shared(leftValue, rightValue, leftEdge, rightEdge, level)
            {
                if (this->rightEdge != nullptr) {
                    rightEdge = this->rightEdge->getDDProductParallel(ut, ct, level-1);
                    rightValue = rightEdge->getValue();
                }
            }
            #pragma omp taskwait
            value = ((n * leftValue) % MOD_NUMBER * rightValue) % MOD_NUMBER;
            auto node = ut->lookup(new Node(leftEdge, rightEdge));
            return new Edge(value, node);
        }

        IEdge* getDDProductParallelCached(long n, IUniqueTable* ut, IComputeTable* ct, int level) {
            long value = n;
            long leftValue = 1;
            long rightValue = 1;
            IEdge* leftEdge = nullptr;
            IEdge* rightEdge = nullptr;
            if (level == 0)
                return this->getDDProduct(n, new CachedUniqueTable(ut), ct);
                // return this->getDDProduct(n, new CachedUniqueTable(ut), new CachedComputeTable(ct));
            #pragma omp task shared(leftValue, rightValue, leftEdge, rightEdge, level)
            {
                if (this->leftEdge != nullptr) {
                    leftEdge = this->leftEdge->getDDProductParallelCached(ut, ct, level-1);
                    leftValue = leftEdge->getValue();
                }
            }
            #pragma omp task shared(leftValue, rightValue, leftEdge, rightEdge, level)
            {
                if (this->rightEdge != nullptr) {
                    rightEdge = this->rightEdge->getDDProductParallelCached(ut, ct, level-1);
                    rightValue = rightEdge->getValue();
                }
            }
            #pragma omp taskwait
            value = ((n * leftValue) % MOD_NUMBER * rightValue) % MOD_NUMBER;
            auto node = ut->lookup(new Node(leftEdge, rightEdge));
            return new Edge(value, node);
        }

        IEdge* getDDProductParallelPrivate(long n, IUniqueTable* ut, IComputeTable* ct, int level) {
            long value = n;
            long leftValue = 1;
            long rightValue = 1;
            IEdge* leftEdge = nullptr;
            IEdge* rightEdge = nullptr;
            if (level == 0)
                return this->getDDProduct(n, new UniqueTablePrivate(), ct);
            #pragma omp task shared(leftValue, rightValue, leftEdge, rightEdge, level)
            {
                if (this->leftEdge != nullptr) {
                    leftEdge = this->leftEdge->getDDProductParallelPrivate(ut, ct, level-1);
                    leftValue = leftEdge->getValue();
                }
            }
            #pragma omp task shared(leftValue, rightValue, leftEdge, rightEdge, level)
            {
                if (this->rightEdge != nullptr) {
                    rightEdge = this->rightEdge->getDDProductParallelPrivate(ut, ct, level-1);
                    rightValue = rightEdge->getValue();
                }
            }
            #pragma omp taskwait
            value = ((n * leftValue) % MOD_NUMBER * rightValue) % MOD_NUMBER;
            auto node = ut->lookup(new Node(leftEdge, rightEdge));
            return new Edge(value, node);
        }

        string getString() {
            if (leftEdge == nullptr && rightEdge == nullptr)
                return string_format("%i", nullptr);
            else
                return  string_format("%i%i%i%i", 
                                        leftEdge->getNode(), leftEdge->getValue(), 
                                        rightEdge->getNode(), rightEdge->getValue());
        }

    private:
        IEdge *leftEdge;
        IEdge *rightEdge;
        static const int MOD_NUMBER = 2147483648 - 1;
        // static const int MOD_NUMBER = pow(2, 30) - 1;

};

class DD : public IDD {
    // Constructors
    public:
        DD(IEdge* edge) {
            headEdge = edge;
            ut = new UniqueTable();
            ct = new ComputeTable();
        }
    // Interface methods
    public:
        IUniqueTable* getUniqueTable() {
            return ut;
        }
    // Public Methods
    public:
        IEdge* getHeadEdge() {
            return headEdge;
        }
        long getProduct() {
            return headEdge->getProduct();
        }
        long getProductParallel() {
            return headEdge->getProductParallel();
        }
        long getDDProduct() {
            headEdge = headEdge->getDDProduct(ut, ct);
            return headEdge->getValue();
        }
        long getDDProductParallel(int level) {
            #pragma omp parallel num_threads(level*2+1) shared(headEdge)
            {
                #pragma omp single
                {
                    #pragma omp task
                    headEdge = headEdge->getDDProductParallel(ut, ct, level);
                    #pragma omp taskwait
                }
            }
            return headEdge->getValue();
        }
        long getDDProductParallelCached(int level) {
            #pragma omp parallel num_threads(level*2+1) shared(headEdge)
            {
                #pragma omp single
                {
                    #pragma omp task
                    headEdge = headEdge->getDDProductParallelCached(ut, ct, level);
                    #pragma omp taskwait
                }
            }
            return headEdge->getValue();
        }
        long getDDProductParallelPrivate(int level) {
            #pragma omp parallel num_threads(level*2+1) shared(headEdge)
            {
                #pragma omp single
                {
                    #pragma omp task
                    headEdge = headEdge->getDDProductParallelPrivate(ut, ct, level);
                    #pragma omp taskwait
                }
            }
            return headEdge->getValue();
        }
    private:
        IEdge* headEdge;
        IUniqueTable* ut;
        IComputeTable* ct;

};

//  ------------------------- Support functions ------------------------- 

void print(string s) {
    cout << s << "\n";
}

DD* createSmallDD() {
    Edge*  leftNode1 = new Edge(1, new Node());
    Edge* rightNode1 = new Edge(2, new Node());
    Edge*  leftNode2 = new Edge(3, new Node());
    Edge* rightNode2 = new Edge(4, new Node());
    Edge*  leftNode3 = new Edge(5, new Node());
    Edge* rightNode3 = new Edge(6, new Node());
    Edge*  leftNode4 = new Edge(7, new Node());
    Edge* rightNode4 = new Edge(8, new Node());
    Edge*  leftNode5 = new Edge(9, new Node());
    Edge* rightNode5 = new Edge(10, new Node());
    Edge*  leftNode6 = new Edge(11, new Node());
    Edge* rightNode6 = new Edge(12, new Node());
    Edge*  leftNode7 = new Edge(13, new Node());
    Edge* rightNode7 = new Edge(14, new Node());
    Edge*  leftNode8 = new Edge(15, new Node());
    Edge* rightNode8 = new Edge(16, new Node());

    Edge*  leftNode1_1 = new Edge(17, new Node(leftNode1, rightNode1));
    Edge* rightNode1_1 = new Edge(18, new Node(leftNode2, rightNode2));
    Edge*  leftNode2_1 = new Edge(19, new Node(leftNode3, rightNode3));
    Edge* rightNode2_1 = new Edge(20, new Node(leftNode4, rightNode4));
    Edge*  leftNode3_1 = new Edge(21, new Node(leftNode5, rightNode5));
    Edge* rightNode3_1 = new Edge(22, new Node(leftNode6, rightNode6));
    Edge*  leftNode4_1 = new Edge(23, new Node(leftNode7, rightNode7));
    Edge* rightNode4_1 = new Edge(24, new Node(leftNode8, rightNode8));
    
    Edge*  leftNode1_2 = new Edge(25, new Node(leftNode1_1, rightNode1_1));
    Edge* rightNode1_2 = new Edge(26, new Node(leftNode2_1, rightNode2_1));
    Edge*  leftNode2_2 = new Edge(27, new Node(leftNode3_1, rightNode3_1));
    Edge* rightNode2_2 = new Edge(28, new Node(leftNode4_1, rightNode4_1));

    Edge*  leftNode1_3 = new Edge(29, new Node(leftNode1_2, rightNode1_2));
    Edge* rightNode1_3 = new Edge(30, new Node(leftNode2_2, rightNode2_2));

    Edge* head = new Edge(31, new Node(leftNode1_3, rightNode1_3));

    return new DD(new Edge(32, new Node(head, head)));
}

DD* createLargeDD() {
    int maxLevel = 17;
    int N = pow(2, maxLevel);
    Edge *nodeArray[N];

    for(int i = 0; i < N; i++) {
        nodeArray[i] = new Edge(i+1, new Node());
    }

    for (int level = 1; level < maxLevel; level++) {
        N = pow(2, maxLevel - level - 1);
        for(int i = 0; i < N; i++) {
            nodeArray[i] = new Edge(
                pow(2, level) + i + 1, 
                new Node(nodeArray[2*i], nodeArray[2*i+1])
            );
        }
    }

    Edge*  leftNode1 = new Edge(2, new Node(nodeArray[0], nodeArray[0]));
    Edge* rightNode1 = new Edge(3, new Node(nodeArray[0], nodeArray[0]));
    Edge*  leftNode2 = new Edge(4, new Node(nodeArray[0], nodeArray[0]));
    Edge* rightNode2 = new Edge(5, new Node(nodeArray[1], nodeArray[1]));
    Edge*  leftNode3 = new Edge(6, new Node(nodeArray[1], nodeArray[0]));
    Edge* rightNode3 = new Edge(7, new Node(nodeArray[0], nodeArray[1]));
    Edge*  leftNode4 = new Edge(8, new Node(nodeArray[0], nodeArray[1]));
    Edge* rightNode4 = new Edge(9, new Node(nodeArray[1], nodeArray[0]));

    leftNode1  = new Edge(3, new Node(leftNode1, rightNode1));
    rightNode1 = new Edge(4, new Node(leftNode2, rightNode2));
    leftNode2  = new Edge(5, new Node(leftNode3, rightNode3));
    rightNode2 = new Edge(6, new Node(leftNode4, rightNode4));

    Edge* leftNode = new Edge(10, new Node(leftNode1, rightNode1));
    Edge* rightNode = new Edge(10, new Node(leftNode2, rightNode2));

    return new DD(new Edge(7, new Node(leftNode, rightNode)));
}

DD* createEqualDD() {
    int maxLevel = 19;
    int N = pow(2, maxLevel);
    Edge *nodeArray[N];

    for(int i = 0; i < N; i++) {
        nodeArray[i] = new Edge(100, new Node());
    }

    for (int level = 1; level < maxLevel; level++) {
        N = pow(2, maxLevel - level - 1);
        for(int i = 0; i < N; i++) {
            nodeArray[i] = new Edge(
                (level + 1) * 100, 
                new Node(nodeArray[2*i], nodeArray[2*i+1])
            );
        }
    }
    Edge* leftNode = new Edge(10, new Node(nodeArray[0], nodeArray[0]));
    Edge* rightNode = new Edge(5, new Node(nodeArray[0], nodeArray[0]));

    return new DD(new Edge(7, new Node(leftNode, rightNode)));
}

void printSequentialRuns(DD* dd, int numIters) {
    print("  # Sequential run:");
    for(int i = 1; i <= numIters; i++) {
        auto start = chrono::high_resolution_clock::now();
        auto res = dd->getDDProduct();
        chrono::duration<double, std::milli> duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res << "\n";
    }
}

void printParallelRuns(DD* dd, int numIters, int level) {
    print("  # Parallel run:");
    for(int i = 1; i <= numIters; i++) {
        auto start = chrono::high_resolution_clock::now();
        auto res = dd->getDDProductParallel(level);
        chrono::duration<double, std::milli> duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res << "\n";
    }
}

void printParallelCachedRuns(DD* dd, int numIters, int level) {
    print("  # Cached parallel run:");
    for(int i = 1; i <= numIters; i++) {
        auto start = chrono::high_resolution_clock::now();
        auto res = dd->getDDProductParallelCached(level);
        chrono::duration<double, std::milli> duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res << "\n";
    }
}

void printParallelPrivateRuns(DD* dd, int numIters, int level) {
    print("  # Private parallel run:");
    for(int i = 1; i <= numIters; i++) {
        auto start = chrono::high_resolution_clock::now();
        auto res = dd->getDDProductParallelPrivate(level);
        chrono::duration<double, std::milli> duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res << "\n";
    }
}

//  --------------------------- Main program ---------------------------- 

int main() {
    print("\n------- Start Program -------\n");

    print(" Generating DDs...");
    DD* ddSmallSequential = createSmallDD();
    DD* ddSmallParallel = createSmallDD();
    DD* ddLargeSequential = createLargeDD();
    DD* ddLargeParallel = createLargeDD();
    DD* ddLargeParallelCached = createLargeDD();
    DD* ddLargeParallelPrivate = createLargeDD();
    //DD* ddEqualSequential = createEqualDD();
    //DD* ddEqualParallel = createEqualDD();
    //DD* ddEqualParallelCached = createEqualDD();
    INode* node = ddSmallSequential->getHeadEdge()->getNode();
    int level;
    print(" DDs Generated\n");

    print(" Testing basics...");
    cout << "  # Thread id: " << omp_get_thread_num() << "\n";
    cout << "  # Node string: " << node->getString() << "\n";
    auto ut = new UniqueTable();
    ut->insert(node);
    cout << "  # Node lookup table: " << ut->lookup(node)->getString() << "\n";
    print(" Basics tested.\n");

    print(" Testing small product...");
    long res;
    auto start = chrono::high_resolution_clock::now();
    res = ddSmallSequential->getProduct();
    chrono::duration<double, std::milli> duration = chrono::high_resolution_clock::now() - start;
    cout << "  # Sequential \t time: " << duration.count() << "\t result: " << res << "\n";
    start = chrono::high_resolution_clock::now();
    res = ddSmallParallel->getProductParallel();
    duration = chrono::high_resolution_clock::now() - start;
    cout << "  # Parallel \t time: " << duration.count() << "\t result: " << res << "\n";
    print(" Small product tested.\n");

    print(" Testing large product...");
    start = chrono::high_resolution_clock::now();
    res = ddLargeSequential->getProduct();
    duration = chrono::high_resolution_clock::now() - start;
    cout << "  # Sequential \t time: " << duration.count() << "\t result: " << res << "\n";
    start = chrono::high_resolution_clock::now();
    res = ddLargeParallel->getProductParallel();
    duration = chrono::high_resolution_clock::now() - start;
    cout << "  # Parallel \t time: " << duration.count() << "\t result: " << res << "\n";
    print(" Large product tested.\n");

    print(" Testing small DD product...");
    int TIMES = 7;
    printSequentialRuns(ddSmallSequential, TIMES); 
    printParallelRuns(ddSmallParallel, TIMES, 1);
    print(" Small DD product tested.\n");

    print(" Testing large DD product...");
    level = 1;
    printSequentialRuns(ddLargeSequential, TIMES);
    printParallelRuns(ddLargeParallel, TIMES, level);
    printParallelCachedRuns(ddLargeParallelCached, TIMES, level);
    printParallelPrivateRuns(ddLargeParallelPrivate, TIMES, level);
    print(" Large DD product tested.\n");

    ddLargeParallel = createLargeDD();
    ddLargeParallelCached = createLargeDD();
    ddLargeParallelPrivate = createLargeDD();

    print(" Testing large DD product with level 2...");
    level = 2;
    printParallelRuns(ddLargeParallel, TIMES, level);
    printParallelCachedRuns(ddLargeParallelCached, TIMES, level);
    printParallelPrivateRuns(ddLargeParallelPrivate, TIMES, level);
    print(" Large DD product tested.\n");

    ddLargeParallel = createLargeDD();
    ddLargeParallelCached = createLargeDD();
    ddLargeParallelPrivate = createLargeDD();

    print(" Testing large DD product with level 3...");
    level = 3;
    printParallelRuns(ddLargeParallel, TIMES, level);
    printParallelCachedRuns(ddLargeParallelCached, TIMES, level);
    printParallelPrivateRuns(ddLargeParallelPrivate, TIMES, level);
    print(" Large DD product tested.\n");

    /*


    print(" Testing equal DD product...");
    level = 1;
    printSequentialRuns(ddEqualSequential, TIMES);
    printParallelRuns(ddEqualParallel, TIMES, level);
    printParallelCachedRuns(ddEqualParallelCached, TIMES, level);
    print(" Equal DD product tested.\n");


    ddEqualParallel = createEqualDD();
    ddEqualParallelCached = createEqualDD();

    print(" Testing equal DD product with level 2...");
    level = 2;
    printParallelRuns(ddEqualParallel, TIMES, level);
    printParallelCachedRuns(ddEqualParallelCached, TIMES, level);
    print(" Equal DD product tested.\n");

    */

    print("\n------- Program Ended -------\n");
}