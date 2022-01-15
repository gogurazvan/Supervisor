#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING 255

int main()
{
    char name[MAX_STRING];
	char b[MAX_STRING];
	int a;

	fgets(name,255,stdin);
    a=strlen(name);
    strcpy(name+a-1,name+a);
    printf("%s has risen\n", name);

	while(1){
		fgets(b,255,stdin);
		a=strlen(b);
		strcpy(b+a-1,b+a);
		
		if(strcmp(b,"kill")==0) break;
	}
	
	printf("%s was killed by an impostor\n", name);

	return 0;
}
