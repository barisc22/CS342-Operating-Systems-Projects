#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include  <sys/types.h>
#include <pthread.h>
#include "hash.h"

HashTable *hash_init (int N, int K){
    HashTable* h = malloc(sizeof(HashTable));
    h -> hash_array = malloc (sizeof(Item*) * N);
    h -> N = N;
    h -> K = K;
    int M = N / K;
    h -> M = M;
    
    h -> locks = malloc(sizeof(pthread_mutex_t) * M);
    for(int j = 0; j < M; j++){
        if (pthread_mutex_init(&(h ->locks[j]), NULL) != 0) { 
            printf("Thread error.\n"); 
            return NULL; 
        }   
    }
    
    int i = 0;
    while(i < N){
        h -> hash_array[i] = NULL;
        i++;
    }
    
    if (h -> hash_array){
        return h; 
    }
    else{
        return NULL;
    }
}

int hash_insert (HashTable *hp, int k, void *v){
    int index = k % hp -> N;
    int region = index / hp -> K;
    pthread_mutex_lock(&hp->locks[region]);
    
    if(!hp){
        pthread_mutex_unlock(&hp->locks[region]);
        return -1;
    }
    
    Item* cur = hp -> hash_array[index];
    if(cur == NULL) {
        Item* newItem = malloc (sizeof(Item*));
        newItem -> data = v;
        newItem -> next = NULL;
        newItem -> key = k;
        hp -> hash_array[index] = newItem;
        pthread_mutex_unlock(&hp->locks[region]);
        return 0;
    }
    
    while(cur != NULL){
        if(cur->next != NULL){
            if(cur->next->key == k){
                return -1;
            }
        }
            
        else{
            Item* newItem = malloc (sizeof(Item*));
            newItem -> data = v;
            newItem -> next = NULL;
            newItem -> key = k;
            cur->next = newItem;
            pthread_mutex_unlock(&hp->locks[region]);
            return 0;
        }
        cur = cur->next;
    }
    pthread_mutex_unlock(&hp->locks[region]);
	return -1; 
}

int hash_get (HashTable *hp, int k, void **vp){
    int index = k % (hp -> N);
    int region = index / hp -> K;
    pthread_mutex_lock(&hp->locks[region]);
    
    if(hp == NULL){
        pthread_mutex_unlock(&hp->locks[region]);
        return -1;
    }
    
    Item* cur = hp -> hash_array[index];
    
    while(cur != NULL){
        if(cur -> key == k){
            *vp = cur -> data;
            pthread_mutex_unlock(&hp->locks[region]);
            return 0;
        }
        cur = cur -> next;
    }
	pthread_mutex_unlock(&hp->locks[region]);
	return -1;
}

int hash_delete (HashTable *hp, int k) {
    int index = k % hp -> N;
    int region = index / hp -> K;
    pthread_mutex_lock(&hp->locks[region]);
    
    if(!hp){
        pthread_mutex_unlock(&hp->locks[region]);
        return -1;
    }
    
    Item* cur = hp -> hash_array[index];
    
    if(cur != NULL && cur->key == k) {
        Item* temp = cur;
        hp -> hash_array[index] = cur->next;
        free(temp);
        pthread_mutex_unlock(&hp->locks[region]);
        return 0;
    }
    
    while(cur -> next){
        if(cur -> next -> key == k){
            Item* temp = cur -> next;
            cur -> next = cur -> next -> next;
            free(temp);
            pthread_mutex_unlock(&hp->locks[region]);
            return 0;
        }
        cur = cur -> next;
    }
    pthread_mutex_unlock(&hp->locks[region]);
	return -1; 
}

int hash_update (HashTable *hp, int k, void *v){
    int index = k % hp -> N;
    int region = index / hp -> K;
    pthread_mutex_lock(&hp->locks[region]);
    
    if(!hp){
        pthread_mutex_unlock(&hp->locks[region]);
        return -1;
    }
    
    Item* cur = hp -> hash_array[index];
    
    while(cur){
        if(cur -> key == k){
            cur -> data = v;
            pthread_mutex_unlock(&hp->locks[region]);
            return 0;
        }
    cur = cur -> next;
    }
    pthread_mutex_unlock(&hp->locks[region]);
	return -1;
}

int hash_destroy (HashTable *hp){
    if(!hp){
        return -1;
    }
    
    for(int j = 0; j < hp -> M; j++){
        pthread_mutex_destroy(&hp ->locks[j]);
    }
    
    int i;
    for(i = 0; i < hp -> N; i++){
        Item* cur = hp -> hash_array[i];
        while(cur){
            Item* temp = cur;
            cur = cur -> next;
            free(temp);
        }
    }
    free(hp -> hash_array);
    free(hp -> locks);
    free(hp);
    hp = NULL;
    return 0; 
}
