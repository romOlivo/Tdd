#include <omp.h>

#include "Interfaces.cpp"

#ifndef UNIQUE_TABLE_H // include guard
#define UNIQUE_TABLE_H

class UniqueTable : public IUniqueTable {
    // Constructors
    public:
        UniqueTable() {
            omp_init_lock(&insertLock);
        }
    // Methods
    public:
        INode* lookup(INode* node) {
            //return node;                                          // -------------------------------------------------- Deactivate
            omp_set_lock(&insertLock);
            INode* dev = node;
            std::size_t hashResult = std::hash<std::string>{}(node->getString());
            if (table.find(hashResult) == table.end()) {
                insert(node);
            } else {
                dev = table[hashResult];
            }
            omp_unset_lock(&insertLock);
            return dev;
        }

        void insert(INode* node) {
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::size_t hashResult = std::hash<std::string>{}(node->getString());
            table[hashResult] = node;
        }
        
    private:
        std::map<std::size_t, INode*> table;
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
            std::size_t hashResult = std::hash<std::string>{}(node->getString());
            if (table.find(hashResult) == table.end()) {
                #pragma omp task
                insert(node);
            } else {
                dev = table[hashResult];
            }
            return dev;
        }
        
        void insert(INode* node) {
            std::size_t hashResult = std::hash<std::string>{}(node->getString());
            #pragma omp critical
            {
            node = ut->lookup(node);
            table[hashResult] = node;
            }
        }
    
    private:
        std::map<std::size_t, INode*> table;
        IUniqueTable* ut;
};
#endif
