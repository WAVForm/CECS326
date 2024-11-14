#include <stdio.h> //for printf
#include <unistd.h> //for fork, exec, and wait
#include <sys/mman.h> //for shm family of functions
#include <fcntl.h> // for O_ constants
#include <string.h> //for memcpy
#include "ynlib.h" //for converting strings to int
#include "myShm.h"

int main(int argc, char * argv[]) {
    if (argc != 3) { //are there enough arguements?
        printf("Invalid arguments. Usage: master [# of slaves] [shared memory name]\n");
        return 1;
    }
    int child_amt = str_to_int(argv[1]); //convert arguement 1 to an int
    if (child_amt > 10) { //in myShm.h, CLASS's report member variable has a fixed size of 10
        printf("More than 10 children!\n");
        return 0;
    }
    printf("Master begins execution.\n");

    int fd = shm_open(argv[2], O_RDWR | O_CREAT, 0666); //create the shared memory object (/dev/shm) return file descriptor (fd)
    if (fd == -1) { //check if it failed
        printf("Failed to open shared memory.\n");
        return 1;
    }

    int r = ftruncate(fd, sizeof(struct CLASS)); //fd has size 0 by default, make it's size the same as the size of CLASS
    if (r == -1) { //check if enlarging failed
        printf("ftruncate failed.\n");
        return 1;
    }

    void * addr = mmap(NULL, sizeof(struct CLASS), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //assign shared memory to program's memory space. Make sure is has the same permissions as fd
    if (addr == (void * ) - 1) { //check if map failed
        printf("Failed to map shared memory.\n");
        return 1;
    }

    struct CLASS shm; //create the shared memory data structure
    memcpy(addr, & shm, sizeof(struct CLASS)); //copy the structure into the shared memory    
    printf("Master created a shared memory segment named %s.\n", argv[2]);

    for (int i = 0; i < child_amt; i++) {
        //create slave
        char * rargs[4] = {
            "./slave",
            int_to_str(i + 1),
            argv[2],
            NULL
        }; //define arguements of slave call
        pid_t rpid = fork(); //create new process
        if (rpid == -1) {
            //child failed to be made
            printf("Slave %d failed to create, exiting to safety\n", i + 1);
            return 1;
        } else if (rpid == 0) {
            //you are the child, made successfully
            execvp(rargs[0], rargs); //switch to slave program and run with args
        }
    }
    printf("Master created %d child processes to terminate.\n", child_amt);

    printf("Master waits for all child processes to terminate.\n");

    while (wait(NULL) > 0); //wait for all children to terminate
    printf("Master received termination signals from all %d child processes.\n", child_amt);

    memcpy( & shm, addr, sizeof(struct CLASS)); //copy modified shared memory into structure
    printf("Updated content of shared memory segment after access by child processes:\n");

    printf("\n");
    for (int i = 0; i < 10; i++) {
        printf("Index %d: %d\n", i, shm.report[i]); //print all the indices of report
    }
    printf("\n");

    r = munmap(addr, sizeof(struct CLASS)); //unmap shared memory from program's memory space
    if (r == -1) { //check if unmapping failed
        printf("Couldn't unmap shared memory data.\n");
    }

    fd = shm_unlink(argv[2]); //remove shared memory object
    if (fd == -1) { //check if removal failed
        printf("Couldn't unlink shared memory.\n");
    }

    printf("Master removed shared memory segment, and is exiting.\n");

    return 0;
}