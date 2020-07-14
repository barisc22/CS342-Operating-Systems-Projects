#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "ralloc.h"
#include <sys/time.h>

int *available;
int **need;
int **allocated;
int **max_demand;
int **request;
int handling;
int p, r;

pthread_mutex_t lock;
pthread_cond_t cond;
struct timeval stop1, start1;

int ralloc_init(int p_count, int r_count, int r_exist[], int d_handling)
{
    p = p_count;
    r = r_count;
    pthread_mutex_init ( &lock, NULL);
    pthread_cond_init(&cond, NULL);
    handling = d_handling;
    
    if(p_count < 0 || r_count < 0 || r_exist == NULL){
        return -1;
    }
    
    
    available = malloc (sizeof(int) * r_count);
    for(int i = 0; i < r_count; i++){
        available[i] = r_exist[i];
    }
    
    
    request =(int**) malloc (sizeof(int*) * p_count);
    for (int i = 0; i < p_count; i++){
        request[i] = (int*) malloc(sizeof(int) * r_count);
    }

    for (int i = 0; i < p_count; i++){
        for(int j = 0; j < r_count; j++){
            request[i][j] = 0;
        }
    }
    
    
    need =(int**) malloc (sizeof(int*) * p_count);
    for (int i = 0; i < p_count; i++){
        need[i] = (int*) malloc(sizeof(int) * r_count);
    }

    for (int i = 0; i < p_count; i++){
        for(int j = 0; j < r_count; j++){
            need[i][j] = 0;
        }
    }
    
    
    allocated =(int**) malloc (sizeof(int*) * p_count);
    for (int i = 0; i < p_count; i++){
        allocated[i] = (int*) malloc(sizeof(int) * r_count);
    }

    for (int i = 0; i < p_count; i++){
        for(int j = 0; j < r_count; j++){
            allocated[i][j] = 0;
        }
    }
    
    
    max_demand =(int**) malloc (sizeof(int*) * p_count);
    for (int i = 0; i < p_count; i++){
        max_demand[i] = (int*) malloc(sizeof(int) * r_count);
    }

    for (int i = 0; i < p_count; i++){
        for(int j = 0; j < r_count; j++){
            max_demand[i][j] = 0;
        }
    }
    
    return (0); 
}

int ralloc_maxdemand(int pid, int r_max[]){
    
    if(r_max == NULL || max_demand == NULL){
        return -1;
    }
    
    for (int i = 0; i < r; i++){ 
        max_demand[pid][i] = r_max[i];
        need[pid][i] = r_max[i];
    }

    return (0); 
}
int ralloc_request (int pid, int demand[]) {
    gettimeofday(&start1, NULL);
    pthread_mutex_lock (&lock); 
    
    for(int i = 0; i < r; i++){
        request[pid][i] += demand[i];
    }
    
    if(handling == DEADLOCK_NOTHING || handling == DEADLOCK_DETECTION){
        //Check if request exceeds max demand
        for(int i = 0; i < r; i++){
            if(max_demand[pid][i] < (allocated[pid][i] + demand[i])){
                printf("Error: Request exceeds max declaration.\n");
                for(int j = 0; j < r; j++){
                    request[pid][j] -= demand[j];
                }
                pthread_mutex_unlock (&lock); 
                gettimeofday(&stop1, NULL);
                printf("TIME: %ld\n", stop1.tv_usec - start1.tv_usec);
                return -1;
            }
        }
        
        //Check if request exceeds available
        int condition = 1;
        do{
            condition = 1;
            for(int i = 0; i < r; i++){
                if(demand[i] > available[i]){
                    condition = 0;
                    break;
                }
            }
            if(condition == 0){
                pthread_cond_wait(&cond, &lock);
            }
            
        }while(condition == 0);
                
        //Grant resources
        for(int i = 0; i < r; i++){
            allocated[pid][i] += demand[i];
            available[i] -= demand[i];
            need[pid][i] -= demand[i];
            request[pid][i] -= demand[i];
        }
        
        //Successful exit
        pthread_mutex_unlock (&lock); 
        gettimeofday(&stop1, NULL);
        printf("TIME: %ld\n", stop1.tv_usec - start1.tv_usec);
        return(0); 
    }
    else if(handling == DEADLOCK_AVOIDANCE){
        //Check if request exceeds max demand
        for(int i = 0; i < r; i++){
            if(max_demand[pid][i] < (allocated[pid][i] + demand[i])){
                printf("Error: Request exceeds max declaration.\n");
                for(int j = 0; j < r; j++){
                    request[pid][j] -= demand[j];
                }
                pthread_mutex_unlock (&lock); 
                gettimeofday(&stop1, NULL);
                printf("TIME: %ld\n", stop1.tv_usec - start1.tv_usec);
                return -1;
            }
        }
        
        //Check if request exceeds available
        int condition = 1;
        do{
            condition = 1;
            for(int i = 0; i < r; i++){
                if(demand[i] > available[i]){
                    condition = 0;
                    break;
                }
            }
            if(condition == 0){
                pthread_cond_wait(&cond, &lock);
            }
            
        }while(condition == 0);
           
        //Grant resources
        for(int i = 0; i < r; i++){
            allocated[pid][i] += demand[i];
            available[i] -= demand[i];
            need[pid][i] -= demand[i];
            request[pid][i] -= demand[i];
        }
        
        //Check if state is safe
        int* work = malloc (sizeof(int) * r);
        int* finish = malloc (sizeof(int) * p);
        int ded = 0;
        for(int i = 0; i < r; i++){
            work[i] = available[i];
        }
        
        for (int i = 0; i < p; i++){
            finish[i] = 0;
            int pure = 1;
            for(int j = 0; j < r; j++){
                if(!(allocated[i][j] == 0 && request[i][j] == 0)){ //Bu şekilde olur mu?
                    pure = 0;
                    break;
                }
            }
            if(pure){
                finish[i] = 1;
            }
        }
        
        int modified = 0;
        do{
            modified = 0;
            for(int i = 0; i < p; i++){
                int go3 = 1;
                for(int j = 0; j < r; j++){
                    if(!(finish[i] == 0 && need[i][j] <= work[j])){
                        go3 = 0;
                    }
                }
                if(go3 == 1){
                    modified++;
                    for(int k = 0; k < r; k++){
                        work[k] += allocated[i][k];
                    }
                    finish[i] = 1;
                }
            }
            
        }while(modified != 0);
        
        for(int i = 0; i < p; i++){
            if(finish[i] == 0){
                ded++;
            }
        }
        
        //Take resources back if state is unsafe
        if(ded != 0){
            printf("No of processes that cannot finish: %d\n", ded);
            for(int i = 0; i < r; i++){
                allocated[pid][i] -= demand[i];
                available[i] += demand[i];
                need[pid][i] += demand[i];
            }
            free(work);
            free(finish);
            
            //Failed exit
            pthread_mutex_unlock (&lock); 
            gettimeofday(&stop1, NULL);
            printf("TIME: %ld\n", stop1.tv_usec - start1.tv_usec);
            return(-1); 
        }
        
        free(work);
        free(finish);
        //Successful exit
        pthread_mutex_unlock (&lock); 
        gettimeofday(&stop1, NULL);
        printf("TIME: %ld\n", stop1.tv_usec - start1.tv_usec);
        
        return(0); 
    }
    else{
        printf("Error: Invalid handling method given.\n");
        pthread_mutex_unlock (&lock);
        
        return -1;
    }
}
int ralloc_release (int pid, int demand[]) {
    pthread_mutex_lock (&lock); 
    
    for(int i = 0; i < r; i++){
        allocated[pid][i] -= demand[i];
        available[i] += demand[i];
        need[pid][i] += demand[i];
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock (&lock); 

    return (0); 
}

int ralloc_detection(int procarray[]) {

    if(handling != DEADLOCK_DETECTION){
        printf("Error: Invalid handling method given.\n");
        return -1;
    }
    int* work = malloc (sizeof(int) * r);
    int* finish = malloc (sizeof(int) * p);
    int ded = 0;
    for(int i = 0; i < r; i++){
        work[i] = available[i];
    }
    
    for (int i = 0; i < p; i++){
        for(int j = 0; j < r; j++){
            finish[i] = 1;
            if(allocated[i][j] != 0 || request[i][j] != 0){
                finish[i]  = 0;
                break;
            }
        }
    }
    
    int modified = 0;
    do{
        modified = 0;
        for(int i = 0; i < p; i++){
            int go3 = 1;
            for(int j = 0; j < r; j++){
                if(!(finish[i] == 0 && request[i][j] <= work[j])){
                    go3 = 0;
                }
            }
            if(go3 == 1){
                modified++;
                for(int k = 0; k < r; k++){
                    work[k] += allocated[i][k];
                }
                finish[i] = 1;
            }
        }
        
    }while(modified != 0);
    
    for(int i = 0; i < p; i++){
        if(finish[i] == 0){
            ded++;
            procarray[i] = 1;
        }
    }
    printf("Deadlocked: %d\n", ded);
    
    free(work);
    free(finish);
    
    return (ded); 
}

int ralloc_end() {
    printf ("ralloc_end called\n");
    
    for (int i = 0; i < p; i++){
        free(need[i]);
    }
    free(need);
    
    for (int i = 0; i < p; i++){
        free(allocated[i]);
    }
    free(allocated);
    
    for (int i = 0; i < p; i++){
        free(max_demand[i]);
    }
    free(max_demand);
    
    for (int i = 0; i < p; i++){
        free(request[i]);
    }
    free(request);
    free(available);
    pthread_mutex_destroy(&lock);
    return (0); 
}
