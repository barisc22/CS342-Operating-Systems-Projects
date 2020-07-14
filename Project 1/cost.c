#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>

int main() {
    struct timeval stop1, start1, stop2, start2, stop3, start3, stop4, start4, stop5, start5;
    int fd, sz;
    char *c = (char *) calloc(100, sizeof(char)); 
    char *j = (char *) calloc(1000, sizeof(char)); 

    //First Trial
    printf("-------------------------First Trial-------------------------\n"); 
    gettimeofday(&start1, NULL);
    creat("hop.txt", S_IRUSR | S_IRGRP | S_IROTH); //This will create a file.
    gettimeofday(&stop1, NULL);
    printf("Time ellapsed for creat : %ld\n", stop1.tv_usec - start1.tv_usec); 

    //Second Trial
    printf("-------------------------Second Trial-------------------------\n");
    creat("hop1.txt", S_IRUSR | S_IRGRP | S_IROTH); //This will create a file.
    fd = open("hop1.txt", O_RDWR | O_EXCL); //This will open the created file.
    gettimeofday(&start2, NULL);
    printf("opened the fd = % d\n", fd); 
    write(fd, "hello baris\n", 11); //Write hello baris
    close(fd);
    gettimeofday(&stop2, NULL);
    printf("Time ellapsed for write : %ld\n", stop2.tv_usec - start2.tv_usec);
    
    fd = open("hop1.txt",O_RDWR | O_EXCL); //This will open the created file.
    gettimeofday(&start3, NULL);
    sz = read(fd, c, 11); //Read hello baris.
    c[sz] = '\0'; 
    printf("Those bytes are as follows: %s\n", c); //Readed chars.
    close(fd);
    gettimeofday(&stop3, NULL);
    printf("Time ellapsed for read : %ld\n", stop3.tv_usec - start3.tv_usec);
    
    //Third Trial
    printf("-------------------------Third Trial-------------------------\n");   
    creat("hop2.txt", S_IRUSR | S_IRGRP | S_IROTH); //This will create a new file.
    fd = open("hop2.txt",O_RDWR | O_EXCL | O_APPEND | O_TRUNC); //This will open the created file.
    gettimeofday(&start4, NULL);
    for (int i = 0; i < 1000; i++){ //Write 1000 a.
        write(fd, "a", 1);
        if ( i % 100 == 0){ //Calculate write in every 100 iteration.
            gettimeofday(&stop4, NULL);
            printf("Time ellapsed for write for %i number: %ld\n", i, stop4.tv_usec - start4.tv_usec);
            
            gettimeofday(&start5, NULL);
            sz = read(fd, j, i);  //Read 100i chars.
            gettimeofday(&stop5, NULL);
            printf("Time ellapsed for read for %i number: %ld\n", i, stop5.tv_usec - start5.tv_usec);
        }
    }
    close(fd);
}
