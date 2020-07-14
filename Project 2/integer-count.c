#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "hash.h"

HashTable *ht1; // space allocated inside library

int comparator (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void* integer_count (char* filename){
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL && fp < 0){
        printf("Could not open file %s",filename);
        exit(EXIT_FAILURE);
    }
    
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int num;
    while ((read = getline(&line, &len, fp)) != -1) {
        num = atoi(line);
        void* count;
        if(hash_get(ht1, num, &count) == 0){
            hash_update(ht1, num, count + 1);
        }
        else{
            hash_insert(ht1, num, 1);
        }
    }
    
    void * test = 0;
    hash_get(ht1, 3, &test);
    fclose(fp);
    return NULL;
}

int main(int argc, char **argv){
    int N = 100;
    int K = 10;
    ht1 = hash_init(N, K);
    int fileCount = atoi(argv[1]);
    pthread_t tid[fileCount];
    char* files[fileCount];
    
    int j = 0;
    for(int i = 2; i < argc; i++){
        char* filename = argv[i];
        files[j] = filename;
        j++;
    }
    
    int error;
    for(int i = 0; i < fileCount; i++){
        error = pthread_create(&tid[i], NULL, &integer_count, files[i]);
        if(error != 0){
            printf("Thread cannot be created.");
        }
    }
    
    for(int j = 0; j < fileCount; j++){
        pthread_join(tid[j], NULL);
    }
    
    int hash_item_count = 0;
    for(int i = 0; i < N; i++){
        Item* cur = ht1->hash_array[i];
        while(cur != NULL){
            hash_item_count++;
            cur = cur -> next;
        }
    }
    
    int keys[hash_item_count];
    int p = 0;
    for(int i = 0; i < N; i++){
        Item* cur = ht1->hash_array[i];
        while(cur != NULL){
            int num = cur->key;
            keys[p] = num;
            p++;
            cur = cur -> next;
        }
    }
    
    qsort(keys, hash_item_count, sizeof(int), comparator);
    FILE *out = fopen(argv[argc - 1], "w");
    if (out == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    void* count;
    for(int i = 0; i < hash_item_count; i++){
        count = 0;
        hash_get(ht1, keys[i], &count);
        fprintf(out, "%d: %d\n", keys[i], count);
    }
    
    hash_destroy(ht1);
    return 0;
}
