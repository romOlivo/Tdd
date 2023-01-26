#include <string>

using namespace std;

#include "Edge.cpp"
#include "Interfaces.cpp"
#include "UniqueTable.cpp"
#include "ComplexNumber.cpp"

#ifndef NODE_H // include guard
#define NODE_H
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

        IComplexNumber* getProduct(IComplexNumber* n) {
            IComplexNumber* value = n;
            if (leftEdge != nullptr)
                value = value->product(leftEdge->getProduct());
            if (rightEdge != nullptr)
                value = value->product(rightEdge->getProduct());
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return value;
        }

        IComplexNumber* getProductParallel(IComplexNumber* n) {
            IComplexNumber* leftValue = new ComplexNumber();
            IComplexNumber* rightValue = new ComplexNumber();
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
            return n->product(leftValue)->product(rightValue);
        }

        IEdge* getDDProduct(IComplexNumber* n, IUniqueTable* ut, IComputeTable* ct) {
            IComplexNumber* value = n;
            IEdge* leftEdge = nullptr;
            IEdge* rightEdge = nullptr;
            if (this->leftEdge != nullptr) {
                leftEdge  = this->leftEdge->getDDProduct(ut, ct);
                value = value->product(leftEdge->getValue());
            }
            if (this->rightEdge != nullptr) {
                rightEdge = this->rightEdge->getDDProduct(ut, ct);
                value = value->product(rightEdge->getValue());
            }
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            auto node = ut->lookup(new Node(leftEdge, rightEdge));
            return new Edge(value, node);
        }

        IEdge* getDDProductParallel(IComplexNumber* n, IUniqueTable* ut, IComputeTable* ct, int level) {
            IComplexNumber* value = n;
            IComplexNumber* leftValue = new ComplexNumber();
            IComplexNumber* rightValue = new ComplexNumber();
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
            #pragma omp flush
            value = value->product(leftValue);
            value = value->product(rightValue);
            auto node = ut->lookup(new Node(leftEdge, rightEdge));
            return new Edge(value, node);
        }

        IEdge* getDDProductParallelCached(IComplexNumber* n, IUniqueTable* ut, IComputeTable* ct, int level) {
            IComplexNumber* value = n;
            IComplexNumber* leftValue = new ComplexNumber();
            IComplexNumber* rightValue = new ComplexNumber();
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
            value = n->product(leftValue)->product(rightValue);
            auto node = ut->lookup(new Node(leftEdge, rightEdge));
            return new Edge(value, node);
        }

        IEdge* getDDProductParallelPrivate(IComplexNumber* n, IUniqueTable* ut, IComputeTable* ct, int level) {
            IComplexNumber* value = n;
            IComplexNumber* leftValue = new ComplexNumber();
            IComplexNumber* rightValue = new ComplexNumber();
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
            value = n->product(leftValue)->product(rightValue);
            auto node = ut->lookup(new Node(leftEdge, rightEdge));
            return new Edge(value, node);
        }

        string getString() {
            if (leftEdge == nullptr && rightEdge == nullptr)
                return string_format("%i", nullptr);
            else
                return string_format("%i",  leftEdge->getNode()) + leftEdge->getValue()->get_string()
                     + string_format("%i", rightEdge->getNode()) + rightEdge->getValue()->get_string();
            /*
                return  string_format("%i%s%i%s", 
                                        leftEdge->getNode(), leftEdge->getValue()->get_string(), 
                                        rightEdge->getNode(), rightEdge->getValue()->get_string());
            */
        }

    private:
        IEdge *leftEdge;
        IEdge *rightEdge;
        // static const int MOD_NUMBER = pow(2, 30) - 1;

};
#endif
