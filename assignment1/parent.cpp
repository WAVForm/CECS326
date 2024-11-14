#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    if((argc-1) % 2 != 0){ //checks name-age pairs
        std::cout << "Not enough pairs. Usage: parent [name1] [age1] ... [namek] [agek]" << std::endl;
        return 1;
    }
    
    //ideally check if ages are valid here, but we'll let child handle it

    //print number of children
    int children = (argc-1)/2;
    std::cout << "I have " << children << " children." << std::endl;

    //loop create child processes
    for(int i = 0; i < children; i++){ //for each child
        char* args[4] = {"./child", argv[(i*2)+1], argv[(i*2)+2], nullptr}; //set up args: command, name, age, null
        pid_t pid = fork(); //create child process
        
        if(pid == -1){
            //Child not created successfully
            std::cout << "Child fail" << std::endl;
        }
        else if(pid == 0){
            //Child successfully created
            execvp(args[0], args); //switch child process to run command with arguements
        }
        else{
            //You are the parent
            wait(0); //wait for child to return 0
        }
    }
    std::cout << "All child processes terminated. Parent exits." << std::endl;
    return 0;
}