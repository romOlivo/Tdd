#ifndef INTERFACES_H // include guard
#define INTERFACES_H

class IComplexNumber {
    public:
        virtual void normalize() = 0;
        virtual long getRealPart() = 0;
        virtual string get_string() = 0;
        virtual long getImaginaryPart() = 0;
        virtual IComplexNumber* product(IComplexNumber* cx) = 0;
};

class IUniqueTable;

class IComputeTable;

class IEdge;

class INode {
    public:
        virtual string getString() = 0;
        virtual IEdge* getLeftEdge() = 0;
        virtual IEdge* getRightEdge() = 0;
        virtual IComplexNumber* getProduct(IComplexNumber* n) = 0;
        virtual IComplexNumber* getProductParallel(IComplexNumber* n) = 0;
        virtual IEdge* getDDProduct(IComplexNumber* n, IUniqueTable* ut, IComputeTable* ct) = 0;
        virtual IEdge* getDDProductParallel(IComplexNumber* n, IUniqueTable* ut, IComputeTable* ct, int level) = 0;
        virtual IEdge* getDDProductParallelCached(IComplexNumber* n, IUniqueTable* ut, IComputeTable* ct, int level) = 0;
        virtual IEdge* getDDProductParallelPrivate(IComplexNumber* n, IUniqueTable* ut, IComputeTable* ct, int level) = 0;
};

class IEdge {
    public:
        virtual IComplexNumber* getValue() = 0;
        virtual INode* getNode() = 0;
        virtual IComplexNumber* getProduct() = 0;
        virtual string getString() = 0;
        virtual IComplexNumber* getProductParallel() = 0;
        virtual IEdge* getDDProduct(IUniqueTable* ut, IComputeTable* ct) = 0;
        virtual IEdge* getDDProductParallel(IUniqueTable* ut, IComputeTable* ct) = 0;
        virtual IEdge* getDDProductParallelCached(IUniqueTable* ut, IComputeTable* ct) = 0;
        virtual IEdge* getDDProductParallel(IUniqueTable* ut, IComputeTable* ct, int level) = 0;
        virtual IEdge* getDDProductParallelCached(IUniqueTable* ut, IComputeTable* ct, int level) = 0;
        virtual IEdge* getDDProductParallelPrivate(IUniqueTable* ut, IComputeTable* ct, int level) = 0;
};

class IComputeTable {
    public:
        virtual IEdge* lookup(INode* node) = 0;
        virtual void insert(INode* inputNode, IEdge* resultEdge) = 0;
};

class IUniqueTable {
    public:
        virtual INode* lookup(INode* node) = 0;
};

class IDD {
    public:
        virtual IUniqueTable* getUniqueTable() = 0;
};
#endif
