#include <omp.h>
#include <cmath>
#include <chrono>
#include <thread>
#include <iostream>
using namespace std;

class Node {
    // Constructors
    public:
        Node(long n, Node* leftNode, Node* rightNode) {
            this->n = n;
            this->leftNode = leftNode;
            this->rightNode = rightNode;
        }
        Node(long n) {
            this->n = n;
            this->leftNode = nullptr;
            this->rightNode = nullptr;
        }
    // Public methods
    public:
        long getValue() {
            return n;
        }

        void printValue() {
            cout << "  # Node with value " << getValue() << "\n";
        }

        Node* getLeftNode() {
            return leftNode;
        }

        Node* getRightNode() {
            return rightNode;
        }

        long getProduct() {
            long value = n;
            if (leftNode != nullptr)
                value = (value * leftNode->getProduct()) % MOD_NUMBER;
            if (rightNode != nullptr)
                value = (value * rightNode->getProduct()) % MOD_NUMBER;
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return value;
        }

        long getProductParallel() {
            long leftValue = 1;
            long rightValue = 1;
            #pragma omp parallel num_threads(2) shared(leftValue, rightValue)
            {
                #pragma omp single 
                {
                    if (leftNode != nullptr)
                        #pragma omp task
                        leftValue = leftNode->getProduct();
                    if (rightNode != nullptr)
                        #pragma omp task
                        rightValue = rightNode->getProduct();
                }
            }
            return ((n * leftValue) % MOD_NUMBER * rightValue) % MOD_NUMBER;
        }

        Node* getDDProduct() {
            long value = n;
            Node* leftNode = nullptr;
            Node* rightNode = nullptr;
            if (this->leftNode != nullptr) {
                leftNode  = this->leftNode->getDDProduct();
                value = (value *  leftNode->getValue()) % MOD_NUMBER;
            }
            if (this->rightNode != nullptr) {
                rightNode = this->rightNode->getDDProduct();
                value = (value * rightNode->getValue()) % MOD_NUMBER;
            }
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return new Node(value, leftNode, rightNode);
        }

        Node* getDDProductParallel() {
            long value = n;
            long leftValue = 1;
            long rightValue = 1;
            Node* leftNode = nullptr;
            Node* rightNode = nullptr;
            #pragma omp parallel num_threads(2) shared(leftValue, rightValue, leftNode, rightNode)
            {
                #pragma omp single 
                {
                    #pragma omp task
                    {
                        if (this->leftNode != nullptr) {
                            leftNode  = this->leftNode->getDDProduct();
                            leftValue = leftNode->getValue();
                        }
                    }
                    #pragma omp task
                    {
                        if (this->rightNode != nullptr) {
                            rightNode = this->rightNode->getDDProduct();
                            rightValue = rightNode->getValue();
                        }
                    }
                }
            }
            
            value = ((n * leftValue) % MOD_NUMBER * rightValue) % MOD_NUMBER;
            return new Node(value, leftNode, rightNode);
        }

    private:
        long n;
        Node *leftNode;
        Node *rightNode;
        static const int MOD_NUMBER = 2147483648 - 1;

};

class DD {
    // Constructors
    public:
        DD(Node* node) {
            headNode = node;
        }
    // Public Methods
    public:
        Node* getHeadNode() {
            return headNode;
        }
        long getProduct() {
            return headNode->getProduct();
        }
        long getProductParallel() {
            return headNode->getProductParallel();
        }
        long getDDProduct() {
            headNode = headNode->getDDProduct();
            return headNode->getValue();
        }
        long getDDProductParallel() {
            headNode = headNode->getDDProductParallel();
            return headNode->getValue();
        }
    private:
        Node* headNode;

};

void print(string s) {
    cout << s << "\n";
}

DD* createSmallDD() {
    Node*  leftNode1 = new Node(1);
    Node* rightNode1 = new Node(2);
    Node*  leftNode2 = new Node(3);
    Node* rightNode2 = new Node(4);
    Node*  leftNode3 = new Node(5);
    Node* rightNode3 = new Node(6);
    Node*  leftNode4 = new Node(7);
    Node* rightNode4 = new Node(8);
    Node*  leftNode5 = new Node(9);
    Node* rightNode5 = new Node(10);
    Node*  leftNode6 = new Node(11);
    Node* rightNode6 = new Node(12);
    Node*  leftNode7 = new Node(13);
    Node* rightNode7 = new Node(14);
    Node*  leftNode8 = new Node(15);
    Node* rightNode8 = new Node(16);

    Node*  leftNode1_1 = new Node(17, leftNode1, rightNode1);
    Node* rightNode1_1 = new Node(18, leftNode2, rightNode2);
    Node*  leftNode2_1 = new Node(19, leftNode3, rightNode3);
    Node* rightNode2_1 = new Node(20, leftNode4, rightNode4);
    Node*  leftNode3_1 = new Node(21, leftNode5, rightNode5);
    Node* rightNode3_1 = new Node(22, leftNode6, rightNode6);
    Node*  leftNode4_1 = new Node(23, leftNode7, rightNode7);
    Node* rightNode4_1 = new Node(24, leftNode8, rightNode8);
    
    Node*  leftNode1_2 = new Node(25, leftNode1_1, rightNode1_1);
    Node* rightNode1_2 = new Node(26, leftNode2_1, rightNode2_1);
    Node*  leftNode2_2 = new Node(27, leftNode3_1, rightNode3_1);
    Node* rightNode2_2 = new Node(28, leftNode4_1, rightNode4_1);

    Node*  leftNode1_3 = new Node(29, leftNode1_2, rightNode1_2);
    Node* rightNode1_3 = new Node(30, leftNode2_2, rightNode2_2);

    Node* head = new Node(31, leftNode1_3, rightNode1_3);

    return new DD(new Node(32, head, head));
}

DD* createLargeDD() {
    int maxLevel = 17;
    int N = pow(2, maxLevel);
    Node *nodeArray[N];

    for(int i = 0; i < N; i++) {
        nodeArray[i] = new Node(i+1);
    }

    for (int level = 1; level < maxLevel; level++) {
        N = pow(2, maxLevel - level - 1);
        for(int i = 0; i < N; i++) {
            nodeArray[i] = new Node(pow(2, level) + i + 1, nodeArray[2*i], nodeArray[2*i+1]);
        }
    }

    Node*  leftNode1 = new Node(2, nodeArray[0], nodeArray[0]);
    Node* rightNode1 = new Node(3, nodeArray[0], nodeArray[0]);
    Node*  leftNode2 = new Node(4, nodeArray[0], nodeArray[0]);
    Node* rightNode2 = new Node(5, nodeArray[1], nodeArray[1]);
    Node*  leftNode3 = new Node(2, nodeArray[1], nodeArray[0]);
    Node* rightNode3 = new Node(3, nodeArray[0], nodeArray[1]);
    Node*  leftNode4 = new Node(4, nodeArray[0], nodeArray[1]);
    Node* rightNode4 = new Node(5, nodeArray[1], nodeArray[0]);

    leftNode1  = new Node(2, leftNode1, rightNode1);
    rightNode1 = new Node(3, leftNode2, rightNode2);
    leftNode2  = new Node(4, leftNode3, rightNode3);
    rightNode2 = new Node(5, leftNode4, rightNode4);

    Node* leftNode = new Node(10, leftNode1, rightNode1);
    Node* rightNode = new Node(10, leftNode2, rightNode2);

    return new DD(new Node(7, leftNode, rightNode));
}

int main() {
    print("\n------- Start Program -------\n");

    print(" Generating DDs...");
    DD* ddSmallSequential = createSmallDD();
    DD* ddSmallParallel = createSmallDD();
    DD* ddLargeSequential = createLargeDD();
    DD* ddLargeParallel = createLargeDD();
    Node* node = ddSmallSequential->getHeadNode();
    print(" DDs Generated\n");

    print(" Testing basics...");
    node->printValue();
    node->getLeftNode()->printValue();
    node->getRightNode()->printValue();
    cout << "  # Thread id: " << omp_get_thread_num() << "\n";
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
    int TIMES = 5;
    print("  # Sequential run:");
    for(int i = 1; i <= TIMES; i++) {
        start = chrono::high_resolution_clock::now();
        res = ddSmallSequential->getDDProduct();
        duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res << "\n";
    }
    print("  # Parallel run:");
    for(int i = 1; i <= TIMES; i++) {
        start = chrono::high_resolution_clock::now();
        res = ddSmallParallel->getDDProductParallel();
        duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res << "\n";
    }
    print(" Small DD product tested.\n");

    print(" Testing large DD product...");
    print("  # Sequential run:");
    for(int i = 1; i <= TIMES; i++) {
        start = chrono::high_resolution_clock::now();
        res = ddLargeSequential->getDDProduct();
        duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res << "\n";
    }
    print("  # Parallel run:");
    for(int i = 1; i <= TIMES; i++) {
        start = chrono::high_resolution_clock::now();
        res = ddLargeParallel->getDDProductParallel();
        duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res << "\n";
    }
    print(" Large DD product tested.\n");

    print("\n------- Program Ended -------\n");
}