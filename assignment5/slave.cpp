#include <stdio.h> //for printf
#include <unistd.h> //for ftruncate, close
#include <sys/mman.h> //for shm family of functions
#include <fcntl.h> //for O_ constants
#include <semaphore.h> //for sem_ family of functions
#include "ynlib.h" //for converting strings to int
#include <iostream>
#include <fstream>
#include "myShm.h"

int main(int argc, char * argv[]) {
    if (argc != 4) { //are there enough arguements?
        printf("Invalid arguments. Usage: slave [# of child] [shared memory name]\n");
        return 1;
    }
    int num = str_to_int(argv[1]); //convert child id to int
    char* fileName = concat_strs(concat_strs("out-",argv[1]),".tmp");
    std::ofstream outputFile(fileName);
    if(!outputFile.is_open()){
        perror("Failed to open output file\n");
        exit(1);
    }
    outputFile << "Slave begins execution.\n";

    sem_t *output_sem = sem_open(argv[3], O_CREAT, 0666, 1);
    if(output_sem == SEM_FAILED){
        perror("Named semaphore failed to open\n");
        exit(1);
    }
    
    outputFile <<"I am child " << num << ", received shared memory name " << argv[2] << ".\n";

    int fd = shm_open(argv[2], O_RDWR, 0); //open the shared memory object, return fd as in master
    if (fd == -1) { //check if open failed
        perror("Failed to open shared memory.\n");
        exit(1);
    }

    int r = ftruncate(fd, sizeof(struct CLASS)); //as in master, expand fd to size of CLASS
    if (r == -1) { //check if failed
        perror("Failed ftruncate.\n");
        exit(1);
    }

    void * addr = mmap(NULL, sizeof(struct CLASS), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //as in master, map shared memory object to program memory space
    if (addr == (void * ) - 1) { //check map failed
        perror("Failed to map shared memory.\n");
        exit(1);
    }

    struct CLASS* shm = (struct CLASS*) addr; //create CLASS object pointer that points to mapped address
    if(sem_wait(&shm->mutex_sem) == -1){ //wait for unnamed semaphore to unlock, then lock it
        perror("sem_wait failed.\n"); //if failed
        exit(1);
    }
    int slot = shm->index; //store temporarily
    shm->report[shm->index] = num; //update current index with number of child process
    shm->index += 1; //update index
    if(sem_post(&shm->mutex_sem) == -1){ //unlock the unnamed semaphore for next program
        perror("sem_post failed.\n"); //if failed
        exit(1);
    }
    outputFile <<"I have written my child number to slot "<< slot << " and updated index to "<< shm->index << ".\n"; //reference old and new index

    r = munmap(addr, sizeof(struct CLASS)); //as in master, unmap shared memory object from program memory space
    if (r == -1) { //check if unmap failed
        perror("Couldn't unmap shared memory data.\n");
        exit(1);
    }

    close(fd); //close access to shared memory object
    outputFile <<"Child "<< num << " closed access to shared memory and terminates.\n";
    outputFile.close();
    std::ifstream readOutput(fileName);
    if(!readOutput.is_open()){
        perror("Could not open output file\n");
        exit(1);
    }
    if(sem_wait(output_sem) == -1){
        perror("Failed to wait output semaphore\n");
        exit(1);
    }
    std::cout << readOutput.rdbuf();
    readOutput.close();
    remove(fileName);
    if(sem_post(output_sem) == -1){
        perror("Failed to post output semaphore\n");
        exit(1);
    }
    if(sem_close(output_sem) == -1){
        perror("Failed to close output semaphore\n");
        exit(1);
    }
    
    return 0;
}