
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "symbolTable.h"
#include "syntaxAnalyzer.h"



HashTable* create_table(HashTable* father) {
    int i = 0;
    HashTable* new_table = (HashTable*)malloc(sizeof(HashTable));

   // if (father == NULL) {
   //     new_table->father_table= NULL;
   // }
   // else {
       new_table->father_table = father;
      
   
    for (i = 0; i < SIZE; i++) {
        new_table->hashArray[i] = 0;
    }
    return new_table;

}


/* Hash a string for a particular hash table. */
int ht_hash(char* key) {

    unsigned long int hashval = 0;
    int i = 0;

    /* Convert our string to an integer */
    while (hashval < ULONG_MAX && i < strlen(key)) {
        hashval = hashval << 8;
        hashval += key[i];
        i++;
    }

    return hashval % SIZE;
}


struct DataItem* search(HashTable* curr_table, char* key, char* role) {
    //get the hash 
    int hashIndex = ht_hash(key);
    if (role != NULL)
    {
        while (curr_table->hashArray[hashIndex] != NULL || curr_table->hashArray[hashIndex + 1] != NULL) {
            if (curr_table->hashArray[hashIndex] != NULL) {
                if (strcmp(curr_table->hashArray[hashIndex]->key, key) == 0)
                    return curr_table->hashArray[hashIndex];
            }

            //go to next cell
            ++hashIndex;

            //wrap around the table
            hashIndex %= SIZE;
        }
    }

    //move in array until an empty 
    else {
        while (curr_table->hashArray[hashIndex] != NULL) {

            if (strcmp(curr_table->hashArray[hashIndex]->key, key) == 0) {
                curr_table->hashArray[hashIndex]->flagUsed = 1;
                return curr_table->hashArray[hashIndex];
            }
            //go to next cell
            ++hashIndex;

            //wrap around the table
            hashIndex %= SIZE;
        }
    }

    if (curr_table->father_table == NULL)
    {
        return NULL;
    }
    return search(curr_table->father_table, key, role);
}

struct DataItem* searchInScope(HashTable* curr_table, char* key) {
    //get the hash 
    int hashIndex = ht_hash(key);
 
    //move in array until an empty 

       while (curr_table->hashArray[hashIndex] != NULL) {

           if (strcmp(curr_table->hashArray[hashIndex]->key, key) == 0){
               curr_table->hashArray[hashIndex]->flagUsed = 1;
               return curr_table->hashArray[hashIndex];
           }
            //go to next cell
            ++hashIndex;

            //wrap around the table
            hashIndex %= SIZE;
        }

        return NULL;
    
}

void insert(HashTable* curr_table, DataItem* dt) {


    struct DataItem* item = (struct DataItem*) malloc(sizeof(struct DataItem));
    item->type = dt->type;
    item->key = dt->key;
    item->role = dt->role;
    item->flagUsed = 0;
    if (strcmp(item->role, "var") != 0) {
        item->flagUsed = 1;
    }
    if (dt->lp != NULL)
        item->lp= dt->lp;
    if (dt->list != NULL) {
        item->list = dt->list;
       
    }
    else {
        item->list = NULL;
    }
    item->sizeOfDims = dt->sizeOfDims;
    item->sizeOfLp = dt->sizeOfLp;

    //get the hash 
    int hashIndex = ht_hash(dt->key);


    //move in array until an empty or deleted cell
    while (curr_table->hashArray[hashIndex] != NULL && curr_table->hashArray[hashIndex]->key != -1) {
        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= SIZE;
    }

    curr_table->hashArray[hashIndex] = item;
}

HashTable* freeHash(HashTable* curr_table) {
    
    HashTable* father = curr_table->father_table;
    for (int i = 0; i < SIZE;i++) {
        if (curr_table->hashArray[i] != NULL) {
            delete(curr_table, curr_table->hashArray[i]);
        }
    }
    free(curr_table);
    return father;
   
}

DataItem* delete(HashTable* curr_table, DataItem* item) {
    char* key = item->key;

    //get the hash 
    int hashIndex = ht_hash(key);
    //move in array until an empty
    if (curr_table->hashArray[hashIndex + 1] != NULL) {
        if (strcmp(curr_table->hashArray[hashIndex + 1]->key, item->key) == 0) {
            hashIndex++;
        }
    }
        while (curr_table->hashArray[hashIndex] != NULL) {


            if (curr_table->hashArray[hashIndex]->key == key) {
                if (curr_table->hashArray[hashIndex]->flagUsed != 1) {
                    fprintf(yyout_parser_semantic,"error, lack of usage of declared variable: %s \n", curr_table->hashArray[hashIndex]->key);
                }
                struct DataItem* temp = curr_table->hashArray[hashIndex];

                //assign a dummy item at deleted position
                curr_table->hashArray[hashIndex] = dummyItem;
                return temp;
            }

            //go to next cell
            hashIndex++;

            //wrap around the table
            hashIndex %= SIZE;
        }
    return NULL;
}

/*void display(HashTable* curr_table) {
    int i = 0;

    for (i = 0; i < SIZE; i++) {

        if (curr_table->hashArray[i] != NULL)

            printf(" (%d,%d)", curr_table->hashArray[i]->key, curr_table->hashArray[i]->data);
        else
            printf(" ~~ ");
    }

    printf("\n");
}*/

void addDims(int num, DataItem* di) {
    //if first dim was inserted
    if (di->list->dim != NULL)
    {
        ListDims* second = (ListDims*)malloc(sizeof(ListDims));
        di->list->next = second;
        second->dim = num;
        second->head = di->list->head;
        di->list = second;
        di->sizeOfDims++;
    }
    //if first dim wasnt inserted
    else {
        di->sizeOfDims = 1;
        di->list->dim = num;
        di->list->head = di->list;
    }
}

void addParams(char* type, DataItem* di) {
    if (di->lp->param != NULL)
    {
        ListParams* second = (ListParams*)malloc(sizeof(ListParams));
        di->lp->next = second;
        second->param = type;
        second->head = di->lp->head;
        di->lp = second;
        di->sizeOfLp++;
    }
    //if first dim wasnt inserted
    else {
        di->sizeOfLp = 1;
        di->lp->param = type;
        di->lp->head = di->lp;
    }

   

}

