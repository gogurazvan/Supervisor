#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef FILE * supervisor_t;


supervisor_t supervisor_init(){
	char pr[100];
	FILE * pipe;
	pipe = popen("/home/goguletele/Desktop/SuperIdol/supervisor", "r");
	if(pipe == NULL){
		perror("eroare initializare");
		return NULL;
	}

	return pipe;
}

int supervisor_close( supervisor_t sup ){
	if(pclose(sup) == -1){
		perror("eroare initializare");
		return -1
	}
	return 1;
}

int main()
{

	supervisor_t x;
	x=supervisor_init();
	return 0;
}