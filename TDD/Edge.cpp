#include "utils.cpp"
#include "Interfaces.cpp"
#include "ComplexNumber.cpp"

#ifndef EDGE_H // include guard
#define EDGE_H
class Edge: public IEdge {

    // Constructors
    public:
        Edge(long n, INode* node) {
            this->node = node;
            this->n = new ComplexNumber(n);
        }

        Edge(IComplexNumber* n, INode* node) {
            this->node = node;
            this->n = n;
        }

    // Interface methods
    public:
        IComplexNumber* getValue() {
            return this->n;
        }
        
        IComplexNumber* getProduct() {
            return node->getProduct(n);
        }

        INode* getNode() {
            return node;
        }

        string getString() {
            return  string_format("%s%i", n->get_string(), node);
        }

        IComplexNumber* getProductParallel() {
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
        IComplexNumber* n;
};
#endif
