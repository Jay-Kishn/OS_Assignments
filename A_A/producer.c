#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main()
{
	const int SIZE = 4096;
	const char *name = "OS";

	int shm_fd;
	void *ptr;

	/* create the shared memory segment */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	/* configure the size of the shared memory segment */
	ftruncate(shm_fd,SIZE);

	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}
	char *base = ptr;
	/*
	Now write to the shared memory region.
	We must increment the value of ptr after each write.
    */
	const char *free_message = "freeeee";
	printf("Writing free space");
        for(int i = 0;i<512;i+=8){
		sprintf(ptr + i,"%s",free_message);
    }

	const char *os_message = "OSisFun";
	FILE *fp;
	umask(0);
	mknod("myfifo", S_IFIFO|0666, 0);
	printf("Named pipe created\n");
	
	if ((fp = fopen("myfifo", "w")) == NULL){
		perror("failed to open");
		exit(1);
	}
	printf("Named pipe opened\n");

	int offset = 0;
	int cnt  = 0;
	int cnt_limit = 12; //iterations after which producer will stop producing

	while(cnt < cnt_limit){
		offset %= cnt_limit;
		const char *ptr = strstr((base+offset), free_message);

		while(ptr == NULL){
			offset += 8;
			offset %= cnt_limit;
			ptr = strstr((base+offset),free_message);
		}
		int pos = ptr - base;
		printf("Ptr after comparing is %d \n",pos);
		printf("Found a free space\n");
		printf("Writing to the shared memory at i=%d\n",cnt);
		sprintf(base + pos,"%s",os_message);

		printf("Now writing the offset as %d\n",pos);
		// We try to write to the pipe the offset of the written material by producer
		fprintf(fp,"%d\n", pos);
		fflush(fp);

		printf("Written the offset: %d\n",pos);
		cnt++;
	}
	
	printf("Closing the pipe now\n");
	fclose(fp);

	return 0;
}
