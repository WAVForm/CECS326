#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include "../ynlib.h"

struct msg{ //message struct
    long mtype;
    char mtext[100];
};

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Invalid argument. Usage: receiver [message queue id] [receiver id]\n");
        return 1;
    }
    int mqid = ynlib::str_to_int(argv[1]);
    int rid = ynlib::str_to_int(argv[2]);
    char* rstr = ynlib::concat_strs(ynlib::concat_strs(ynlib::concat_strs("Receiver ", argv[2]),", PID "), ynlib::int_to_str(getpid())); //string to make printing easier

    printf("%s, begin execution\n", rstr);
    printf("%s, received message queue id %d through command line argument\n", rstr, mqid);

    msg message;
    bool wantMessage = true;
    while(wantMessage){
        //wait for messages
        while(msgrcv(mqid, &message, sizeof(message.mtext), rid, 0) == -1){
            //printf("%s, message failed to receive\n", rstr);
            continue;
        }
        if(ynlib::str_eq(message.mtext, "\0") == 1){ //if it's a terminate message
            wantMessage = false;
            message.mtype = 999; //set channel to ack
            strcpy(message.mtext, ""); //set text
            if(msgsnd(mqid, &message, sizeof(message.mtext), 0) == -1){ //send and check for fail
                printf("%s, terminate response failed to send\n", rstr);
            }
            break;
        }
        else{ //normal message
            printf("%s, retrieved the message from message queue:\n%s\n", rstr, message.mtext); //print message
            
            //respond
            message.mtype = 999; //set channel
            strcpy(message.mtext, ynlib::concat_strs(rstr, " acknowledges receipt of message")); //set text
            printf("%s, sent acknowledgement message into queue\n", rstr); //preemptive message
            while(msgsnd(mqid, &message, sizeof(message.mtext), 0) == -1){ //send and check for fail
                printf("%s, message failed to send\n", rstr);
            }
        }
    }
    printf("%s, terminates\n", rstr);
    return 0;
}