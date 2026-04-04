#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>

#define SOCK_PATH "unix_socket_example"
#define BUFFER_SIZE 256

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd;
     char buffer[BUFFER_SIZE];
     struct sockaddr_un serv_addr, cli_addr;
     int n;
     int size;
     /* create socket */

     sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

     /* fill in socket addres */
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sun_family = AF_UNIX;
     strcpy(serv_addr.sun_path, SOCK_PATH);

     /* bind socket to this address */
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     int iteration = 0;
     int limit = 0;
     bool size_rcvd = false;
     bzero(buffer,BUFFER_SIZE);
     /* read message from client */
     while( !size_rcvd || iteration <= limit){
       int len = sizeof(cli_addr);

       if(size_rcvd){
         n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&cli_addr, &len);
       }else{
         n = recvfrom(sockfd, &size, sizeof(size), 0, (struct sockaddr *)&cli_addr, &len);
       }
       if (n < 0) error("ERROR reading from socket");

       if(!size_rcvd){
         limit = (size/BUFFER_SIZE) ; 
         printf("Size of file is %d\n",size);
         printf("No of iterations is %d\n",limit);
         size_rcvd = true;
         continue;
       }
       
       iteration++;
       printf("%s",buffer);
     }
     unlink(SOCK_PATH);
     return 0; 
}
