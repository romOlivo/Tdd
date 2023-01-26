#include <map>
#include <omp.h>
#include <cmath>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include <unordered_map>
using namespace std;

#include "TDD/Interfaces.cpp"
#include "TDD/UniqueTable.cpp"
#include "TDD/ComputeTable.cpp"
#include "TDD/ComplexNumber.cpp"
#include "TDD/Node.cpp"
#include "TDD/Edge.cpp"
#include "TDD/DD.cpp"

//  ------------------------- Support functions ------------------------- 

void print(string s) {
    cout << s << "\n";
}

DD* createControlatedDD() {
    Edge*  leftNode1 = new Edge(1, new Node());
    Edge* rightNode1 = new Edge(2, new Node());
    Edge*  leftNode2 = new Edge(1, new Node());
    Edge* rightNode2 = new Edge(3, new Node());
    Edge*  leftNode3 = new Edge(1, new Node());
    Edge* rightNode3 = new Edge(4, new Node());
    Edge*  leftNode4 = new Edge(1, new Node());
    Edge* rightNode4 = new Edge(5, new Node());
    Edge*  leftNode5 = new Edge(1, new Node());
    Edge* rightNode5 = new Edge(6, new Node());
    Edge*  leftNode6 = new Edge(1, new Node());
    Edge* rightNode6 = new Edge(7, new Node());
    Edge*  leftNode7 = new Edge(1, new Node());
    Edge* rightNode7 = new Edge(2, new Node());
    Edge*  leftNode8 = new Edge(1, new Node());
    Edge* rightNode8 = new Edge(3, new Node());

    Edge*  leftNode1_1 = new Edge(1, new Node(leftNode1, rightNode1));
    Edge* rightNode1_1 = new Edge(2, new Node(leftNode2, rightNode2));
    Edge*  leftNode2_1 = new Edge(1, new Node(leftNode3, rightNode3));
    Edge* rightNode2_1 = new Edge(2, new Node(leftNode4, rightNode4));
    Edge*  leftNode3_1 = new Edge(1, new Node(leftNode5, rightNode5));
    Edge* rightNode3_1 = new Edge(2, new Node(leftNode6, rightNode6));
    Edge*  leftNode4_1 = new Edge(1, new Node(leftNode7, rightNode7));
    Edge* rightNode4_1 = new Edge(2, new Node(leftNode8, rightNode8));
    
    Edge*  leftNode1_2 = new Edge(2, new Node(leftNode1_1, rightNode1_1));
    Edge* rightNode1_2 = new Edge(10, new Node(leftNode2_1, rightNode2_1));
    Edge*  leftNode2_2 = new Edge(5, new Node(leftNode3_1, rightNode3_1));
    Edge* rightNode2_2 = new Edge(2, new Node(leftNode4_1, rightNode4_1));

    Edge*  leftNode1_3 = new Edge(2, new Node(leftNode1_2, rightNode1_2));
    Edge* rightNode1_3 = new Edge(3, new Node(leftNode2_2, rightNode2_2));

    Edge* head = new Edge(10, new Node(leftNode1_3, rightNode1_3));

    return new DD(head);  // Expected result: 580 608 000
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
    int maxLevel = 16;
    int N = pow(2, maxLevel);
    Edge *nodeArray[N];

    for(int i = 0; i < N; i++) {
        nodeArray[i] = new Edge(new ComplexNumber(i+1, 1), new Node());
    }

    for (int level = 1; level < maxLevel; level++) {
        N = pow(2, maxLevel - level - 1);
        for(int i = 0; i < N; i++) {
            nodeArray[i] = new Edge(
                new ComplexNumber(level + 1, i), 
                new Node(nodeArray[2*i], nodeArray[2*i+1])
            );
        }
    }

    Edge*  leftNode1 = new Edge(2, new Node(nodeArray[0], nodeArray[0]));
    Edge* rightNode1 = new Edge(3, new Node(nodeArray[0], nodeArray[0]));
    Edge*  leftNode2 = new Edge(2, new Node(nodeArray[0], nodeArray[0]));
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
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res->get_string() << "\n";
    }
}

void printParallelRuns(DD* dd, int numIters, int level) {
    print("  # Parallel run:");
    for(int i = 1; i <= numIters; i++) {
        auto start = chrono::high_resolution_clock::now();
        auto res = dd->getDDProductParallel(level);
        chrono::duration<double, std::milli> duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res->get_string() << "\n";
    }
}

void printParallelCachedRuns(DD* dd, int numIters, int level) {
    print("  # Cached parallel run:");
    for(int i = 1; i <= numIters; i++) {
        auto start = chrono::high_resolution_clock::now();
        auto res = dd->getDDProductParallelCached(level);
        chrono::duration<double, std::milli> duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res->get_string() << "\n";
    }
}

void printParallelPrivateRuns(DD* dd, int numIters, int level) {
    print("  # Private parallel run:");
    for(int i = 1; i <= numIters; i++) {
        auto start = chrono::high_resolution_clock::now();
        auto res = dd->getDDProductParallelPrivate(level);
        chrono::duration<double, std::milli> duration = chrono::high_resolution_clock::now() - start;
        cout << "   --> Iter: " << i << "\t time: " << duration.count() << "\t result: " << res->get_string() << "\n";
    }
}

//  --------------------------- Main program ---------------------------- 

int main() {
    print("\n------- Start Program -------\n");

    print(" Generating DDs...");
    DD* ddControlatedSequential = createControlatedDD();
    DD* ddControlatedParallel = createControlatedDD();
    DD* ddSmallSequential = createSmallDD();
    DD* ddSmallParallel = createSmallDD();
    DD* ddLargeSequential = createLargeDD();
    DD* ddLargeParallel = createLargeDD();
    DD* ddLargeParallelCached = createLargeDD();
    DD* ddLargeParallelPrivate = createLargeDD();
    INode* node = ddSmallSequential->getHeadEdge()->getNode();
    int level;
    print(" DDs Generated\n");

    print(" Testing basics...");
    cout << "  # Thread id: " << omp_get_thread_num() << "\n";
    cout << "  # Node string: " << node->getString() << "\n";
    auto ut = new UniqueTable();
    ut->insert(node);
    cout << "  # Node lookup table: " << ut->lookup(node)->getString() << "\n";
    IComplexNumber* c = new ComplexNumber(3, -2);
    c = c->product(new ComplexNumber(-4, 1));
    cout << "  # C1 * C2: " << c->get_string() << "\n";                                                     // Expected result: -10 + 11i
    print(" Basics tested.\n");


    print(" Testing controlated product...");
    IComplexNumber* res;
    auto start = chrono::high_resolution_clock::now();
    res = ddControlatedSequential->getProduct();
    chrono::duration<double, std::milli> duration = chrono::high_resolution_clock::now() - start;
    cout << "  # Sequential \t time: " << duration.count() << "\t result: " << res->get_string() << "\n";
    start = chrono::high_resolution_clock::now();
    res = ddControlatedParallel->getProductParallel();
    duration = chrono::high_resolution_clock::now() - start;
    cout << "  # Parallel \t time: " << duration.count() << "\t result: " << res->get_string() << "\n";
    print(" Small controlated tested.\n");

    print(" Testing small product...");
    start = chrono::high_resolution_clock::now();
    res = ddSmallSequential->getProduct();
    duration = chrono::high_resolution_clock::now() - start;
    cout << "  # Sequential \t time: " << duration.count() << "\t result: " << res->get_string() << "\n";
    start = chrono::high_resolution_clock::now();
    res = ddSmallParallel->getProductParallel();
    duration = chrono::high_resolution_clock::now() - start;
    cout << "  # Parallel \t time: " << duration.count() << "\t result: " << res->get_string() << "\n";
    print(" Small product tested.\n");

    /*
    print(" Testing large product...");
    start = chrono::high_resolution_clock::now();
    res = ddLargeSequential->getProduct();
    duration = chrono::high_resolution_clock::now() - start;
    cout << "  # Sequential \t time: " << duration.count() << "\t result: " << res->get_string()  << "\n";
    start = chrono::high_resolution_clock::now();
    res = ddLargeParallel->getProductParallel();
    duration = chrono::high_resolution_clock::now() - start;
    cout << "  # Parallel \t time: " << duration.count() << "\t result: " << res->get_string()  << "\n";
    print(" Large product tested.\n");
    */

    print(" Testing small DD product...");
    int TIMES = 7;
    printSequentialRuns(ddSmallSequential, TIMES); 
    printParallelRuns(ddSmallParallel, TIMES, 1);
    print(" Small DD product tested.\n");

    print(" Testing controlated DD product...");
    printSequentialRuns(ddControlatedSequential, TIMES); 
    printParallelRuns(ddControlatedParallel, TIMES, 1);
    print(" Controlated DD product tested.\n");

    int NLevels = 4;
    print(" Testing large DD product...");
    for(level = 1; level < NLevels; level++) {
        printf(" Testing large DD product with level %i...\n", level);
        printSequentialRuns(ddLargeSequential, TIMES);
        printParallelRuns(ddLargeParallel, TIMES, level);
        printParallelCachedRuns(ddLargeParallelCached, TIMES, level);
        printParallelPrivateRuns(ddLargeParallelPrivate, TIMES, level);
        print(" Large DD product tested.\n");

        ddLargeSequential = createLargeDD();
        ddLargeParallel = createLargeDD();
        ddLargeParallelCached = createLargeDD();
        ddLargeParallelPrivate = createLargeDD();
    }

   /*

    DD* ddEqualSequential = createEqualDD();
    DD* ddEqualParallel = createEqualDD();
    DD* ddEqualParallelCached = createEqualDD();
    DD* ddEqualParallelPrivated = createEqualDD();

    print(" Testing equal DD product...");
    level = 1;
    printSequentialRuns(ddEqualSequential, TIMES);
    printParallelRuns(ddEqualParallel, TIMES, level);
    printParallelCachedRuns(ddEqualParallelCached, TIMES, level);
    printParallelPrivateRuns(ddEqualParallelPrivated, TIMES, level);
    print(" Equal DD product tested.\n");

    ddEqualParallel = createEqualDD();
    ddEqualParallelCached = createEqualDD();
    ddEqualParallelPrivated = createEqualDD();

    print(" Testing equal DD product with level 2...");
    level = 2;
    printParallelRuns(ddEqualParallel, TIMES, level);
    printParallelCachedRuns(ddEqualParallelCached, TIMES, level);
    printParallelPrivateRuns(ddEqualParallelPrivated, TIMES, level);
    print(" Equal DD product tested.\n");

    ddEqualParallel = createEqualDD();
    ddEqualParallelCached = createEqualDD();
    ddEqualParallelPrivated = createEqualDD();

    print(" Testing equal DD product with level 3...");
    level = 3;
    printParallelRuns(ddEqualParallel, TIMES, level);
    printParallelCachedRuns(ddEqualParallelCached, TIMES, level);
    printParallelPrivateRuns(ddEqualParallelPrivated, TIMES, level);
    print(" Equal DD product tested.\n");

    */

    print("\n------- Program Ended -------\n");
}