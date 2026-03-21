#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main()
{
	const char *name = "OS";
	const int SIZE = 4096;

	int shm_fd;
	void *ptr;
	int i;

	/* open the shared memory segment */
	
	shm_fd = shm_open(name, O_RDWR, 0666);
	while (shm_fd == -1) {
		printf("Opening of shared memory failed\n");
		printf("Retrying.....\n");
		sleep(3);
		// Open shared memory segment in read only mode
		shm_fd = shm_open(name, O_RDONLY, 0666);
	}

	// now map the shared memory segment in the address space of the process 
	ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}
	
	// create a buffer to read the offset from the pipe
	char readbuf[80];
	/* now read from the shared memory region */
	printf("%s\n", (char *)ptr);
		
	// read the offset from the pipe and print it 
	FILE *fp;
	if((fp = fopen("myfifo","r")) == NULL) {
		perror("fopen");
		exit(1);
	}
    
	//while producer is still sending data, read the offset and print the message at the offset
	while(fgets(readbuf,8,fp) != NULL){
		printf("Received char: %s\n", readbuf);
		// convert the offset to an integer and print the message at that offset
		char *message = ptr + atoi(readbuf);
		printf("Producer sent :%.7s \n",message);

		// rewrite the message at that offset to be "freeeee" 
		memcpy(ptr + atoi(readbuf), "freeeee", 8);
		printf("Replaced the string to now be: %.6s \n",message);
        sleep(3);
	}
    
	printf("Closing the pipe now\n");	
	fclose(fp);
	
	/* remove the shared memory segment */
	if (shm_unlink(name) == -1) {
		printf("Error removing %s\n",name);
		exit(-1);
	}
	

	return 0;
}
