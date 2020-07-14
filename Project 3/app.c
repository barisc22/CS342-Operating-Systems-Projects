#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "ralloc.h"
#include "ralloc.c"
#include <sys/time.h>

#define N 5                   // number of processes - threads
#define M 3                   // number of resource types
#define n 5
int handling_method;          // deadlock handling method
int exist[3] =  {n, n, n};//, n, n, n, n, n, n, n};  // resources existing in the system
pthread_t tids[N];            // ids of created threads

void *aprocess (void *p)
{
    int req[3];
    int k;
    int pid;
    
    pid =  *((int *)p);        
    req[0] = n;
    req[1] = n;
    req[2] = n;
    /*
    req[3] = n;
    req[4] = n;
    req[5] = n;
    req[6] = n;
    req[7] = n;
    req[8] = n;
    req[9] = n;
    */
    ralloc_maxdemand(pid, req); 
    int given = 0;
    
    for (k = 0; k < n; ++k) {
        
        req[0] = 1;
        req[1] = 1;
        req[2] = 1;
        /*
        req[3] = 1;
        req[4] = 1;
        req[5] = 1;
        req[6] = 1;
        req[7] = 1;
        req[8] = 1;
        req[9] = 1;
        */
        if(ralloc_request (pid, req) != -1){
            given++;
            ralloc_release (pid, req);
        }
    }
    /*req[0] = given;
    req[1] = given;
    req[2] = given;
    ralloc_release (pid, req);*/
    printf ("Leaving aprocess: %d\n", pid);
    pthread_exit(NULL); 
}


int main(int argc, char **argv)
{
    struct timeval stop1, start1;
    int dn; // number of deadlocked processes
    int deadlocked[N]; // array indicating deadlocked processes
    int k;
    int i;
    int pids[N];
    
    for (k = 0; k < N; ++k)
        deadlocked[k] = -1; // initialize
    
    int choice = 0;
    printf("CHOOSE HANDLING METHOD:\n");
    printf("DEADLOCK_NOTHING = 1\n");
    printf("DEADLOCK_DETECTION = 2\n");
    printf("DEADLOCK_AVOIDANCE = 3\n");
    scanf("%d", &choice);
    if(choice == 1){
        handling_method = DEADLOCK_NOTHING; 
    }
    else if(choice == 2){
        handling_method = DEADLOCK_DETECTION; 
    }
    else if(choice == 3){
        handling_method = DEADLOCK_AVOIDANCE; 
    }
    else{
        printf("Numbers must be between 1 and 3.");
        return -1;
    }
    
    ralloc_init (N, M, exist, handling_method);    
    for (i = 0; i < N; ++i) {
        pids[i] = i;
        pthread_create (&(tids[i]), NULL, (void *) &aprocess,
                        (void *)&(pids[i])); 
    }
    
    while (1) {
        sleep (15); // detection period
        if (handling_method == DEADLOCK_DETECTION) {
            gettimeofday(&start1, NULL);
            dn = ralloc_detection(deadlocked);
            gettimeofday(&stop1, NULL);
            printf("DEADLOCK TIME: %ld\n", stop1.tv_usec - start1.tv_usec);
            if (dn > 0) {
                printf ("there are %d deadlocked processes\n", dn);
            }
        }
        
        printf("Printing available...\n");
        for(int i = 0; i < p; i++){
            printf("%d ", available[i]);
            printf("\n");
        }
        
        printf("Printing need...\n");
        for(int i = 0; i < p; i++){
            for(int j = 0; j < r; j++){
                printf("%d ", need[i][j]);
            }
            printf("\n");
        }
        
        printf("Printing allocated...\n");
        for(int i = 0; i < p; i++){
            for(int j = 0; j < r; j++){
                printf("%d ", allocated[i][j]);
            }
            printf("\n");
        }
        
        printf("Printing request...\n");
        for(int i = 0; i < p; i++){
            for(int j = 0; j < r; j++){
                printf("%d ", request[i][j]);
            }
            printf("\n");
        }
        
        ralloc_end();
        exit(0);
        return 0;
    }    
}
