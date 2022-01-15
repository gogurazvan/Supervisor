

#include "library.h"

static void skeleton_daemon()
{
	pid_t pid = 0;
	pid_t sid = 0;

	pid = fork();

	if(pid < 0)
	{
		printf("Error on fork\n");
		exit(1);
	}


	if(pid > 0)
	{
		printf("Getting the process id: %d\n", pid);
		exit(0);
	}

	umask(0);

	sid = setsid();

	if(sid < 0)
	{
		exit(1);
	}

}





int main(){

	skeleton_daemon();

	citire();
	
    return 0;
}