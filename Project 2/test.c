#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include <sys/time.h>
#include "hash.h"

HashTable *ht0;
HashTable *ht1;
HashTable *ht2;
HashTable *ht3;
HashTable *ht4;
HashTable *ht5;
HashTable* hop[6];

void* test (HashTable* hp){
    struct timeval stop1, start1;
    
    gettimeofday(&start1, NULL);
    for (int j = 0; j < 1000; j++) {
        hash_insert (hp, j, (void *) 35);
    }
    gettimeofday(&stop1, NULL);
    printf("MULTITHREAD INSERT 1000: %ld\n", stop1.tv_usec - start1.tv_usec);
    
    
    void* vp = 0;
    gettimeofday(&start1, NULL);
    for (int j = 0; j < 1000; j++) {
        vp = 0;
        hash_get (hp, j, &vp);
    }
    gettimeofday(&stop1, NULL);
    printf("MULTITHREAD GET 1000: %ld\n", stop1.tv_usec - start1.tv_usec);
    
    
    for (int j = 0; j < 1000; j++) {
        hash_delete (hp, j);
    }
    gettimeofday(&stop1, NULL);
    printf("MULTITHREAD DELETE 1000: %ld\n", stop1.tv_usec - start1.tv_usec);
    
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    //Multithread Experiment
    int thread_count = 2;
    pthread_t tid[thread_count];
    struct timeval stop1, start1;    
    ht3 = hash_init (1000, 1);
    
    int error;
    for(int i = 0; i < thread_count; i++){
        error = pthread_create(&tid[i], NULL, &test, ht3);
        if(error != 0){
            printf("Thread cannot be created.");
        }
    }
    
    for(int j = 0; j < thread_count; j++){
        pthread_join(tid[j], NULL);
    }
    
    hash_destroy (ht3);

    //Size Experiment
    ht0 = hash_init (100, 1);
	ht1 = hash_init (250, 1);
    ht2 = hash_init (500, 1);
    ht3 = hash_init (1000, 1);
    ht4 = hash_init (1000, 25);
    ht5 = hash_init (1000, 100);
    
    hop[0] = ht0;
    hop[1] = ht1;
    hop[2] = ht2;
    hop[3] = ht3;
    hop[4] = ht4;
    hop[5] = ht5;
    
    for(int i = 0; i < 6; i++){
        gettimeofday(&start1, NULL);
        for (int j = 0; j < 10000; j++) {
           hash_insert (hop[i], j, (void *) 35);
        }
        gettimeofday(&stop1, NULL);
        printf("INSERT 10000: %d %ld\n",i, stop1.tv_usec - start1.tv_usec);
    }
    
    void* vp = 0;
    for(int i = 0; i < 6; i++){
        gettimeofday(&start1, NULL);
        for (int j = 0; j < 10000; j++) {
            vp = 0;
            hash_get(hop[i], j, &vp);
        }
        gettimeofday(&stop1, NULL);
        printf("GET 10000: %d %ld\n",i, stop1.tv_usec - start1.tv_usec);
    }
    
    for(int i = 0; i < 6; i++){
        gettimeofday(&start1, NULL);
        for (int j = 0; j < 10000; j++) {
           hash_delete (hop[i], j);
        }
        gettimeofday(&stop1, NULL);
        printf("DELETE 10000: %d %ld\n",i, stop1.tv_usec - start1.tv_usec);
    }
    
    hash_destroy (ht0);
    hash_destroy (ht1);
    hash_destroy (ht2);
    hash_destroy (ht3);
    hash_destroy (ht4);
    hash_destroy (ht5);

    return 0;
}
