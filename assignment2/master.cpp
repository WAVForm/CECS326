#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include "../ynlib.h"

#define MSG_KEY 2513479 //random unique integer key

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Invalid arguments. Usage master [# of receivers up to 999]\n");
        return 1;
    }

    printf("Master, PID %d, begins execution\n", getpid());
    int ramt = ynlib::str_to_int(argv[1]);
    if(ramt <= 0){
        printf("No receivers wanted, exiting\n");
        return 0;
    }
    else if(ramt >= 999){
        printf("Too many receivers (999 used for ack id)\n");
        return 1;
    }

    /*How permission flag works:
    2 = write permission, 4 = read permission, 6 = read+write permission
        00000
          ^^^
      user|||
      group||
       owner|
    -------------
        00666 = read+write permission for user+group+owner
    */

    //create message queue

    int mqid = msgget(MSG_KEY, IPC_CREAT | 00666); //get a queue. IPC_CREAT tells the command to make one. 0666 sets the permissions
    if(mqid == -1){ //if failure
        printf("Message queue creation failed\n");
        return -1;
    }
    printf("Master acquired a message queue, id %d\n", mqid);
    
    //create sender
    char* sargs[4] = {"./sender", ynlib::int_to_str(mqid), ynlib::int_to_str(ramt), nullptr};
    pid_t spid = fork(); //create new process
    if (spid == -1){
        //child failed to be made
        printf("Sender failed to create, exiting to safety\n");
        return 1;
    }
    else if(spid == 0){
        //you are the child, made successfully
        execvp(sargs[0], sargs); //switch to sender program and run with args
    }
    else{
        //you are parent
        printf("Master created a child process to serve as sender, sender's PID %d\n", spid);
    }

    for(int i = 0; i < ramt; i++){
        //create receivers
        char* rargs[4] = {"./receiver", ynlib::int_to_str(mqid), ynlib::int_to_str(i+1), nullptr};
        pid_t rpid = fork(); //create new process
        if(rpid == -1){
            //child failed to be made
            printf("Receiver %d failed to create, exiting to safety\n", i+1);
            return 1;
        }
        else if(rpid == 0){
            //you are the child, made successfully
            execvp(rargs[0], rargs); //switch to receiver program and run with args
        }
        else{
            //you are the parent
            printf("Master created a child process to serve as receiver %d\n", i+1);
        }
    }

    printf("Master waits for all child processes to terminate\n");
    waitpid(spid, 0, 0); //wait for sender to exit
    msgctl(mqid, IPC_RMID, nullptr); //remove message queue
    printf("Master received termination signals from all child processes, removed message queue, and terminates\n");
    return 0;
}   