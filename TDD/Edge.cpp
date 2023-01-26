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
            IEdge* edge = ct->lookup(node);
            if (edge == nullptr) {
                edge = node->getDDProduct(new ComplexNumber(), ut, ct);
                ct->insert(node, edge);
            }
            return new Edge(edge->getValue()->product(n), edge->getNode());
        }

        IEdge* getDDProductParallel(IUniqueTable* ut, IComputeTable* ct) {
            return getDDProductParallel(ut, ct, 1);
        }

        IEdge* getDDProductParallel(IUniqueTable* ut, IComputeTable* ct, int level) {
            IEdge* edge = ct->lookup(node);
            if (edge == nullptr) {
                edge = node->getDDProductParallel(new ComplexNumber(), ut, ct, level);
                ct->insert(node, edge);
            }
            return new Edge(edge->getValue()->product(n), edge->getNode());
        }

        IEdge* getDDProductParallelCached(IUniqueTable* ut, IComputeTable* ct) {
            return getDDProductParallelCached(ut, ct, 1);
        }

        IEdge* getDDProductParallelCached(IUniqueTable* ut, IComputeTable* ct, int level) {
            IEdge* edge = ct->lookup(node);
            if (edge == nullptr) {
                edge = node->getDDProductParallelCached(new ComplexNumber(), ut, ct, level);
                ct->insert(node, edge);
            }
            return new Edge(edge->getValue()->product(n), edge->getNode());
        }

        IEdge* getDDProductParallelPrivate(IUniqueTable* ut, IComputeTable* ct, int level) {
            IEdge* edge = ct->lookup(node);
            if (edge == nullptr) {
                edge = node->getDDProductParallelPrivate(new ComplexNumber(), ut, ct, level);
                ct->insert(node, edge);
            }
            return new Edge(edge->getValue()->product(n), edge->getNode());
        }

    private:
        INode* node;
        IComplexNumber* n;
};
#endif
