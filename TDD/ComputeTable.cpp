#include <string>
#include <omp.h>

using namespace std;

#include "Edge.cpp"
#include "Interfaces.cpp"
#include "ComplexNumber.cpp"

#ifndef COMPUTE_TABLE_H // include guard
#define COMPUTE_TABLE_H
class ComputeTable : public IComputeTable {
    // Constructors
    public:
        ComputeTable() {
            omp_init_lock(&insertLock);
        }
    // Methods
    public: 
        IEdge* lookup(IEdge* edge) {
            //return nullptr;                                             // -------------------------------------------------- Deactivate
            //omp_set_lock(&insertLock);
            IEdge* dev = nullptr;
            std::size_t hashResult = std::hash<std::string>{}(edge->getString());
            if (table.find(hashResult) != table.end()) {
                dev = table[hashResult];
            }
            //omp_unset_lock(&insertLock);
            return dev;
        }

        void insert(IEdge* inputEdge, IEdge* resultEdge) {
            std::size_t hashResult = std::hash<std::string>{}(inputEdge->getString());
            #pragma omp task
            {
            omp_set_lock(&insertLock);
            table[hashResult] = resultEdge;
            #pragma omp flush
            omp_unset_lock(&insertLock);
            }
        }

    private:
        std::map<std::size_t, IEdge*> table;
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
#endif
