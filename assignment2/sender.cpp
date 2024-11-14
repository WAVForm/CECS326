#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include "../ynlib.h"

#define MSG_SIZE 100 
struct msg{ //message struct
    long mtype;
    char mtext[MSG_SIZE];
};

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Invalid argument. Usage sender [message queue id] [# of receivers]\n");
        return 1;
    }
    int mqid = ynlib::str_to_int(argv[1]); //message queue id
    int ramt = ynlib::str_to_int(argv[2]); //how many receivers?
    char* sstr = ynlib::concat_strs("Sender, PID ", ynlib::int_to_str(getpid())); //string to make printing easier

    printf("%s, begins execution\n", sstr);
    printf("%s, received message queue id %d through commandline argument\n", sstr, mqid);

    msg message;
    bool wantMessage = true;
    while(wantMessage){
        //create message
        char text[MSG_SIZE];
        int rid;
        printf("%s: Please input your message\n", sstr); //message to send
        scanf("%s", &text);
        printf("%s, which receiver is this message for? (1-%d)\n", sstr, ramt); //reciever to send to
        scanf("%d", &rid);
        while(rid <= 0 || rid > ramt){ //confirm id is in range
            printf("%s, enter a valid receiver ID (1-%d):\n", sstr, ramt);
            scanf("%d", &rid);
        }

        //send message
        message.mtype = rid; //set channel
        strcpy(message.mtext, text); //set text
        printf("%s, sent the following message into message queue for receiver %d:\n%s\n", sstr, rid, message.mtext); //preemptive message
        if(msgsnd(mqid, &message, sizeof(message.mtext), 0) == -1){ //send the message and also check if it didn't send
            printf("%s, message failed to send\n", sstr); //if not sent
            continue;
        }
        printf("Sent, waiting.\n");

        //wait for response
        while(msgrcv(mqid, &message, sizeof(message.mtext), 999, 0) == -1){//wait for the response on ack channel
            continue;
            //printf("%s, failed to receive a response\n", sstr);
        }
        printf("%s, receives a message:\n%s\n", sstr, message.mtext);

        //repeat
        char confirm;
        printf("Send another message? (y/n): ");
        scanf(" %c", &confirm);
        while(confirm != 'y' && confirm != 'n' && confirm != 'Y' && confirm != 'N'){ //check that char is valid
            printf("Please enter either the character y or n: ");
            scanf(" %c", &confirm);
        }
        if(confirm == 'n' || confirm == 'N'){ //wants to exit, keep this silent unless something went wrong
            wantMessage = false;
            for(int i = 1; i <= ramt; i++){ //for each receiver send terminate message
                //send
                message.mtype = i; //set channel 
                strcpy(message.mtext, "\0");//set text
                if(msgsnd(mqid, &message, sizeof(message.mtext), 0) == -1){ //send and check if failed
                    printf("%s, terminate message failed to send\n", sstr);
                }

                //wait for response
                while(msgrcv(mqid, &message, sizeof(message.mtext), 999, 0) == -1){//wait for response
                    continue;
                    //printf("%s, teminate confirmation failed to receive.\n", sstr);
                }
                if(ynlib::str_eq(message.mtext, "\0") == 0){ //if message is weird
                    printf("%s, receiver %d did not respond correctly to terminate, continuing\n", sstr, i+1);
                }
            }
            break;
        }
    }
    printf("%s, terminates\n", sstr);
    return 0;
}