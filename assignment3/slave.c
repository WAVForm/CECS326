#include <stdio.h> //for printf
#include <sys/mman.h> //for shm family of functions
#include <fcntl.h> //for O_ constants
#include <string.h> //for memcpy
#include "ynlib.h" //for converting strings to int
#include "myShm.h"

int main(int argc, char * argv[]) {
    if (argc != 3) { //are there enough arguements?
        printf("Invalid arguments. Usage: slave [# of child] [shared memory name]\n");
        return 1;
    }
    int num = str_to_int(argv[1]); //convert child id to int
    printf("Slave begins execution.\n");

    printf("I am child %d, received shared memory name %s.\n", num, argv[2]);

    int fd = shm_open(argv[2], O_RDWR, 0); //open the shared memory object, return fd as in master
    if (fd == -1) { //check if open failed
        printf("Failed to open shared memory.\n");
        return 1;
    }

    int r = ftruncate(fd, sizeof(struct CLASS)); //as in master, expand fd to size of CLASS
    if (r == -1) { //check if failed
        printf("Failed ftruncate.\n");
        return 1;
    }

    void * addr = mmap(NULL, sizeof(struct CLASS), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //as in master, map shared memory object to program memory space
    if (addr == (void * ) - 1) { //check map failed
        printf("Failed to map shared memory.\n");
        return 1;
    }

    struct CLASS shm; //create CLASS object
    memcpy( & shm, addr, sizeof(struct CLASS)); //copy shared memory into CLASS object
    int slot = shm.index; //store temporarily
    shm.report[shm.index] = num; //update current index with number of child process
    shm.index += 1; //update index
    memcpy(addr, & shm, sizeof(struct CLASS)); //copy CLASS object into shared memory
    printf("I have written my child number to slot %d and updated index to %d.\n", slot, shm.index); //reference old and new index

    r = munmap(addr, sizeof(struct CLASS)); //as in master, unmap shared memory object from program memory space
    if (r == -1) { //check if unmap failed
        printf("Couldn't unmap shared memory data.\n");
    }

    close(fd); //close access to shared memory object
    printf("Child %d closed access to shared memory and terminates.\n", num);

    return 0;
}