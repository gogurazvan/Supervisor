#ifndef LIBRARYHEADERFILE_INCLUDED
#define LIBRARYHEADERFILE_INCLUDED


#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>
#include <ctype.h>

#define SUPERVISOR_STATUS_RUNNING 0x1
#define SUPERVISOR_STATUS_PENDING 0x2
#define SUPERVISOR_STATUS_STOPPED 0x4
#define MAX_STRING 255
#define MAX_SERVICES 20
#define PORT 9002


//gcc -c library.c -o library.o
//gcc supervisor.c library.o -o supervisor
//./supervisor

typedef FILE * service_t;
typedef FILE * supervisor_t;

struct service_aux
{
    service_t fd;
    int id;
    char name[MAX_STRING];
    int status;
    
};

typedef struct service_aux service;

supervisor_t supervisor_init();

service_t find_name(const char* servc);

service_t service_create(const char* servicename);


service_t service_open(const char * servicename);

int service_remove(service_t service);


int service_cancel( service_t service );

int service_close(service_t service);

int service_resume(service_t service);


int service_status(service_t service);

int supervisor_list();

int supervisor_freelist();

void citire();


#endif




