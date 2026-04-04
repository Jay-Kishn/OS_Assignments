#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

#define SOCK_PATH "unix_socket_example"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_un serv_addr;
    char buffer[256];

    /* create socket, get sockfd handle */
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* fill in server address */
    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCK_PATH); 
    
    /* Read from a file */ 
    FILE *in_file  = fopen("sample_file.txt", "r"); // read only
    if(!in_file){
               perror("fopen");
               return EXIT_FAILURE;
    }
    
    
    // get the size of the file
    struct stat st;
    fstat(fileno(in_file), &st);
    int size = (int)st.st_size;
    bool transmitted_size = false;
      
    printf("Size of the file is %d\n",size);
    while(in_file){
      if(!transmitted_size){
        transmitted_size=true;
        /* send user message to server */
        n = sendto(sockfd, &size, sizeof(size), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

      }else{
      	fread(buffer,sizeof(*buffer),sizeof(buffer)/sizeof(buffer[0]),in_file);
        /* send user message to server */
        n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
      }

    if (n < 0) 
         error("ERROR writing to socket");
    }
    close(sockfd);
    return 0;
}
