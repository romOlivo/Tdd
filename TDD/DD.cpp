#include "Edge.cpp"
#include "Node.cpp"
#include "Interfaces.cpp"
#include "UniqueTable.cpp"
#include "ComputeTable.cpp"

#ifndef DD_H // include guard
#define DD_H
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
        
        IComplexNumber* getProduct() {
            return headEdge->getProduct();
        }

        IComplexNumber* getProductParallel() {
            return headEdge->getProductParallel();
        }

        IComplexNumber* getDDProduct() {
            headEdge = headEdge->getDDProduct(ut, ct);
            return headEdge->getValue();
        }

        IComplexNumber* getDDProductParallel(int level) {
            #pragma omp parallel num_threads(12) shared(headEdge)
            {
                #pragma omp single
                {
                    headEdge = headEdge->getDDProductParallel(ut, ct, level);
                }
            }
            return headEdge->getValue();
        }

        IComplexNumber* getDDProductParallelCached(int level) {
            #pragma omp parallel num_threads(12) shared(headEdge)
            {
                #pragma omp single
                {
                    headEdge = headEdge->getDDProductParallelCached(ut, ct, level);
                }
            }
            return headEdge->getValue();
        }

        IComplexNumber* getDDProductParallelPrivate(int level) {
            #pragma omp parallel num_threads(12) shared(headEdge)
            {
                #pragma omp single
                {
                    headEdge = headEdge->getDDProductParallelPrivate(ut, ct, level);
                }
            }
            return headEdge->getValue();
        }
    
    private:
        IEdge* headEdge;
        IUniqueTable* ut;
        IComputeTable* ct;

};
#endif
