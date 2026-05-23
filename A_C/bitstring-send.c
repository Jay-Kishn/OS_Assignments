#include <stdio.h> // for printf
#include <unistd.h> // for any linux utilities
#include <signal.h> // for signal
#include <sys/wait.h> // for wait() and fork
#include <string.h> // for memset
#include <stdlib.h> // for exit
#include <sys/types.h> // for kill
#include <stdbool.h> // for bool
#define LENGTH 8 // The fixed length bitstring

char recvdString[9]; // the buffer for the child to store the received string into
bool canSend = false; // synchronization mechanism for notifying the parent that the child is ready to receive
int called_times = 0;

void sigHandle(int sig) { 
    // TODO : Implement this function for when child receives a 0 or 1
    if(sig == SIGUSR1){
      recvdString[called_times] = '1';
      printf("1\n");
    }else if(sig == SIGUSR2){
      recvdString[called_times] = '0';
      printf("0\n");
    }
    called_times++;
}


void synchronizeParent (int sig) { // helper function to ensure the parent only sends once the child is ready to receive
    canSend = true;
}


int main () {

    signal(SIGUSR1,synchronizeParent); // to ensure parent is able to understand when child is telling it that its ready to receive
    int cpid = fork(); // fork
    if (cpid == 0) {
        /*
        TODO: Implement signal handling mechanisms to catch 1 or 0 from parent
        */ 
        /*
        TODO: Add some sort of logic for what you want to do after signal is received
        */
        int last_called_times = called_times;
        signal(SIGUSR1, sigHandle);
        signal(SIGUSR2, sigHandle);
        for(int i=0;i<8;i++){
           kill(getppid(),SIGUSR1); // Sends signal to parent that child is ready to receive
           while(last_called_times == called_times){;}
           last_called_times = called_times;
        }
        recvdString[8] = '\0'; // Null terminates the string so that it prints in an expected manner
        printf("[Child] Received bitstring is\t%s\n",recvdString); // Do not edit, prints the received bitstring

        exit(0);
    }
    else {

        printf("Please input a %d-bit bitstring:\t",LENGTH);

        char tmp[256]; // buffer to store bitstring
        fgets(tmp,LENGTH + 1,stdin); // Take input from user

        if (strlen(tmp) != 8) {
            printf("Error : Input string not of length %d\n",LENGTH);
            kill(cpid,9);
            wait(NULL);
            exit(1);
        }

        for (int i=0; i < LENGTH; i++) {
            if (!(tmp[i] == '1' || tmp[i] == '0')) {
                printf("Error : Input string not a bitstring at index %d char %c\n",i,tmp[i]);
                kill(cpid,9);
                wait(NULL);
                exit(1);
            }
        }

        printf("[Parent] Input bitstring is \t%s\n",tmp);

        while (!canSend) {;} // Wait until the child is ready to receive
        for (int i=0; i < LENGTH; i++) {
            while (!canSend) {;} // Wait until the child is ready to receive
            if (tmp[i] == '1') {
                // TODO : Add mechanism to send 1 to child
                kill(cpid,SIGUSR1);
            }
            else {
                // TODO : Add mechanism to send 0 to child
                kill(cpid,SIGUSR2);
            }
            canSend = false;
            
            // This is necessary to ensure the signals 1]all get sent to child and 2] are sent in correct ordering, increase time to increase reliability
        }
        wait(NULL); // reap the child
    }

}
