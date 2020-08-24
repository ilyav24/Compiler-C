#pragma once

#define SIZE 100

typedef struct HashTable {
    struct HashTable* father_table;
    struct DataItem* hashArray[SIZE];
}HashTable;

typedef struct DataItem {
    char* type;
    char* key;
    char* role;
    int sizeOfDims;
    int sizeOfLp;
    struct ListDims* list;
    struct ListParams* lp;
    int flagUsed;
}DataItem;

typedef struct ListDims {
    struct ListDims* head;
    int dim;
    struct ListDims* next;
}ListDims;

typedef struct ListParams
{
    struct ListParams* head;
    char* param;
    struct ListParams* next;
}ListParams;

//struct DataItem* hashArray[SIZE];
struct DataItem* dummyItem;
struct DataItem* item;
//struct HashTable* father_table;
struct HashTable* curr_table;

int ht_hash(char* key);
struct DataItem* search(HashTable* curr_table, char* key, char* role);
void insert(HashTable* curr_table, DataItem* dt);
HashTable* freeHash(HashTable* curr_table);
DataItem* delete(HashTable* curr_table, DataItem* item);
//void display();
void addDims(int num, DataItem* di);
void addParams(char* type, DataItem* di);
HashTable* create_table(father);
struct DataItem* searchInScope(HashTable* curr_table, char* key);