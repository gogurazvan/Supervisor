#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

void Cola(int x,int *v,int k){
	*(v+k)=x;
	if(x==1) return;
	if(x%2==0) Cola(x/2,v,k+1);
		else Cola(3*x+1,v,k+1);
}

int main(int argc, char *argv[]){
	int sz=getpagesize();

	int shm_fd;
	char shm_name[]="myshm";
	int shm_size = (argc+1)*sz*sizeof(int);

	shm_fd=shm_open(shm_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

	if(shm_fd < 0){
		perror(NULL);
		return errno;
	}

	if( ftruncate(shm_fd, shm_size) == -1 ){
		perror(NULL);
		shm_unlink(shm_name);
		return errno;
	}

	char*p =NULL;
	pid_t pid;
	int x,poz;
	printf("Starting parent %d\n",getpid());
	for(int i=1; i<argc; ++i){
		x=strtol(argv[i],&p,10);
		poz=i-1;
		pid=fork();
		if(pid <= 0) break;
	}

	if(pid < 0){

		perror(NULL);
		shm_unlink(shm_name);
		return errno;

	} else if(pid==0){
		int *scrie= (int*)mmap(0,sz*sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, poz*sizeof(int)*sz);
		Cola(x,scrie,0);
		munmap((void*)scrie, sz*sizeof(int));
	}else{

		for(int i=1; i<argc; ++i) wait(NULL);
		int *afis= (int*)mmap(0,shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);

		for(int i=1;i<argc;++i){
			int it=(i-1)*sz;
			printf("%d: ",*(afis+it));
			while( *(afis+it)>1  && it < i*sz){
				printf("%d ",*(afis+it));
				it++;
			}
			if(*(afis+it)==1) printf("%d\n",*(afis+it));
			else printf("colatz limit exceded\n");
		}

		munmap((void*)afis, shm_size);
		shm_unlink(shm_name);

	}
	printf("Done parent %d me %d\n",getppid(),getpid());

	return 0;

}