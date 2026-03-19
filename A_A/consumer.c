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
		printf("shared memory failed\n");
		printf("retrying.....\n");
		sleep(3);
		shm_fd = shm_open(name, O_RDONLY, 0666);
	}

	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}
	
	char readbuf[80];
	/* now read from the shared memory region */
	printf("%s\n", (char *)ptr);
		
	// read the offset from the pipe and print it 
	FILE *fp;
	if((fp = fopen("myfifo","r")) == NULL) {
		perror("fopen");
		exit(1);
	}
	printf("About to read from pipe, fp=%p\n", fp);
int c = fgetc(fp);
printf("First char: %d ('%c')\n", c, c);
ungetc(c, fp);  // put it back
	while(fgets(readbuf,8,fp) != NULL){
	        printf("Now reading from the blocking op \n");	
	//	fgets(readbuf,8,fp);
		printf("Received char: %s\n", readbuf);
		char *message = ptr + atoi(readbuf);
		printf("Producer sent :%.7s \n",message);
		printf("ptr = %p\n", ptr);
		memcpy(ptr + atoi(readbuf), "freeeee", 8);
		printf("Replaced the string to now be: %.6s \n",message);
                sleep(3);
	}
        printf("CLosing the pipe nowi\n");	
	fclose(fp);

	/* remove the shared memory segment */
	if (shm_unlink(name) == -1) {
		printf("Error removing %s\n",name);
		exit(-1);
	}
	

	return 0;
}
