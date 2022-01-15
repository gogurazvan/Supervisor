//.....................
// -> FUNCTII ghe
//.....................

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

//#include "library.h"


typedef FILE * service_t;
typedef FILE * supervisor_t;

int cnt_service = 0;

struct service_aux
{
	service_t fd;
    int id;
    char name[MAX_STRING];
    int status;
    
};


typedef struct service_aux service;

service services[MAX_SERVICES];

service_t find_name(const char* servc)
{
	for(int i=0; i<cnt_service; ++i)
	{
		if(strcmp(servc, services[i].name) == 0)
			return services[i].fd;
	}
	return NULL;
}

service_t service_create(const char* servicename){

	if(find_name(servicename) != NULL)
	{
		printf("procesul deja e creat, utilizeaza open\n");
		return NULL;
	}
	if(cnt_service < MAX_SERVICES){
		services[cnt_service].id = cnt_service;
		strcpy(services[cnt_service].name, servicename);
		services[cnt_service].status = SUPERVISOR_STATUS_PENDING;
		cnt_service++;
	}
	else{
		printf("prea multe servicii");
		return NULL;
	}


    char path[MAX_STRING];
	service_t pipe;
	getcwd(path, sizeof(path));
	strcat(path, "/service");
	pipe = popen(path, "w");
	if(pipe == NULL){
		printf("eroare creare serviciu");
		return NULL;
	}
    fprintf(pipe, "%s\n", servicename);
	fflush(pipe);

	
	services[cnt_service - 1].fd = pipe;
	

	return pipe;
}

service_t service_resume(service_t service){
	for(int i = 0; i < cnt_service; i++){

		if(services[i].fd == service){

			if(services[i].status != SUPERVISOR_STATUS_STOPPED){
				printf("Serviciul deja ruleaza.\n");
				return NULL;
			}


			service_t pipe;
			char path[MAX_STRING];
			getcwd(path, sizeof(path));
			strcat(path, "/service");
			pipe = popen(path, "w");

			if(pipe == NULL){
				printf("eroare creare serviciu");
				return NULL;
			}

		    fprintf(pipe, "%s\n", services[i].name);
	    	fflush(pipe);

	    	services[i].status = SUPERVISOR_STATUS_PENDING;

			return pipe;
		}
	}
}

service_t service_open(const char * servicename){
	int service_exists = -1;
	service_t pipe;

	for(int i = 0; i < cnt_service; i++){
		if(strcmp(servicename, services[i].name) == 0){
			service_exists = i;
			pipe = services[i].fd;
			break;
		}
	}

	if(service_exists < 0){
		pipe = service_create(servicename);
		services[cnt_service - 1].status = SUPERVISOR_STATUS_RUNNING;	//romanian fix
	}
	else{

		if(services[service_exists].status == SUPERVISOR_STATUS_STOPPED){
			pipe=service_resume(pipe);
			services[service_exists].fd=pipe;
	    }
	    services[service_exists].status = SUPERVISOR_STATUS_RUNNING;
	    
	}

    //printf("open finnish\n");
	return pipe;

}

 int service_remove(service_t service){
 	int temp_id = -1;
 	for(int i = 0; i < cnt_service; i++)
 		if(services[i].fd == service)
 			temp_id = services[i].id;
 	if(temp_id == -1)
 	{
 		printf("eroare remove\n");
 		return -1;
 	}

 	for(int i = temp_id; i < cnt_service - 1; i++){
 		services[i].fd = services[i + 1].fd;
 		strcpy(services[i].name, services[i + 1].name);
 		services[i].status = services[i + 1].status;
 	}
 	cnt_service--;

 	printf("The service has been removed\n");
 	return 0;

 }

int service_cancel( service_t service ){
	char servicename[MAX_STRING];
	for(int i = 0; i < cnt_service; i++)
 		if(services[i].fd == service){
 			strcpy(servicename, services[i].name);
 			services[i].status = SUPERVISOR_STATUS_STOPPED;
 		}

	char k_signal[]= "kill";
	fprintf(service, "%s\n", k_signal);
	fflush(service);
	if(pclose(service) == -1){
		printf("eroare inchidere serviciu");
		return -1;
	}
	
	return 0;
}

int service_status(service_t service){		
	for(int i = 0; i < cnt_service; i++)
 		if(services[i].fd == service)
 			return services[i].status;
 	return -1;
}


int service_close(service_t service){
	int stat = service_status(service);
	if(stat != 4)
	{
		if(service_cancel(service) == -1)
		{	
			printf("Eroare la cancel\n");
			return -1;
		}
	}
	
	if(service_remove(service) == -1)
	{
		printf("Eroare la remove\n");
		return -1;
	}
	return 0;
}



int supervisor_list(){
	if(cnt_service == 0){
		printf("Nu exista niciun serviciu.\n");
		return -1;
	}
	for(int i = 0; i < cnt_service; i++){
		printf("Serviciul cu id %d are:\n  FD: %p\n  Nume: %s  Status: %d\n\n", services[i].id, services[i].fd, services[i].name, services[i].status);
	}
	return 0;

}

int supervisor_freelist(){
	while(cnt_service)
		if(service_close(services[0].fd) == -1){
			printf("Eroare la stergerea serviciului %s\n", services[0].name);
			return -1;
		}
	return 0;
}

void citire()
{
	//char server_message[256] = "SERVER";
    
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    socklen_t size = sizeof(server_address);
    
    bind(server_socket, (struct sockaddr*) &server_address, size);

    listen(server_socket, 5);
	int client_socket;
    client_socket = accept(server_socket, (struct sockaddr*) &server_address, &size);
    //printf("accept: %d\n", client_socket);

    while(1)
	{
    	//printf("Client: %d\n", client_socket);

    	char client_response[256];

   		if(recv(client_socket, client_response, 256, 0) != 0)
   		{
    		//printf("The client sent: %s\n", client_response);

    		char *comanda;
    		comanda = strtok(client_response, "\n ");
    		
    		if(strcmp(comanda, "create") == 0)
    		{
    			//printf("CREATE\n");
    			comanda = strtok(NULL, " ");
    			if(comanda == NULL)
    			{
    				printf("Comanda invalida\n");
    			}
    			else
    			{
    				//printf("%s\n", comanda);
    				service_create(comanda);
    			}
    		}
    		else if(strcmp(comanda, "exit") == 0)
    		{
    			//printf("EXIT\n");
    			
			    client_socket = accept(server_socket, (struct sockaddr*) &server_address, &size);
			    //printf("accept: %d\n", client_socket); 		
    		}
    		else if(strcmp(comanda, "cancel") == 0)
    		{
    			//printf("CANCEL\n");
    			comanda = strtok(NULL, " ");
    			if(comanda == NULL)
    			{
    				printf("Comanda invalida\n");
    			}
    			else
    			{
    				//printf("%s\n", comanda);
    				service_t srv = find_name(comanda);
    				service_cancel(srv);
    			}
    		}
    		else if(strcmp(comanda, "list") == 0)
    		{
    			//printf("LIST\n");
    			
    			supervisor_list();
    		}
    		else if(strcmp(comanda, "free") == 0)
    		{
    			//printf("FREE_LIST\n");
    			
    			supervisor_freelist();
    		}
    		else if(strcmp(comanda, "remove") == 0)
    		{
    			//printf("REMOVE\n");
    			comanda = strtok(NULL, " ");
    			if(comanda == NULL)
    			{
    				printf("Comanda invalida\n");
    			}
    			else
    			{
    				//printf("%s\n", comanda);
    				service_t srv = find_name(comanda);
    				service_remove(srv);
    			}
    			
    		}
    		else if(strcmp(comanda, "close") == 0)
    		{
    			//printf("CLOSE\n");
    			comanda = strtok(NULL, " ");
    			if(comanda == NULL)
    			{
    				printf("Comanda invalida\n");
    			}
    			else
    			{
    				//printf("%s\n", comanda);
    				service_t srv = find_name(comanda);
    				service_close(srv);
    			}
    			
    		}
    		else if(strcmp(comanda, "status") == 0)
    		{
    			//printf("STATUS\n");
    			comanda = strtok(NULL, " ");
    			if(comanda == NULL)
    			{
    				printf("Comanda invalida\n");
    			}
    			else
    			{
    				//printf("%s\n", comanda);
	    			service_t srv = find_name(comanda);
	    			int stat = service_status(srv);
	    			if(stat == SUPERVISOR_STATUS_RUNNING)
	    			{
	    				printf("Status: running\n");
	    			}
	    			else if(stat == SUPERVISOR_STATUS_PENDING)
	    			{
	    				printf("Status: pending\n");
	    			}
	    			else if(stat == SUPERVISOR_STATUS_STOPPED)
	    			{
	    				printf("Status: Stopped\n");
	    			}
    			}

    		}
    		else if(strcmp(comanda, "resume") == 0)
    		{
    			//printf("RESUME\n");
    			comanda = strtok(NULL, " ");
    			if(comanda == NULL)
    			{
    				printf("Comanda invalida\n");
    			}
    			else
    			{
    				//printf("%s\n", comanda);
    				service_t srv = find_name(comanda);
    				if(srv != NULL){
	    				int val=0;
	    				while(val<cnt_service && services[val].fd!=srv) val+=1;
	    				services[val].fd = service_resume(srv);
	    				printf("succesfully resumed %s\n", comanda);
	    			}else{
	    				printf("Nu s-a gasit serviciul\n");
	    			}
    			}	
    		}
    		else if(strcmp(comanda, "open") == 0)
    		{
    			//printf("RESUME\n");
    			comanda = strtok(NULL, " ");
    			if(comanda == NULL)
    			{
    				printf("Comanda invalida\n");
    			}
    			else
    			{
    				//printf("%s\n", comanda);
    				service_open(comanda);
    				printf("succesfully opened %s\n", comanda);
    			}
    			
    		}
    		
    		memset(client_response, 0, sizeof(client_response));
   		}
   		
	}


    //send(client_socket, server_message, sizeof(server_message), 0);

    close(server_socket);
}

supervisor_t supervisor_init(){

    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (connection_status == -1)
    {
        printf("Error making connection\n");
    }

    char server_response[256];

    system("clear");
    
    
    while(1)
    {
		char comanda[256];
		char auxcomanda[256];
    	fgets(comanda, 256, stdin);
    	//printf("%s\n", comanda);

    	strcpy(auxcomanda, comanda);

    	//printf("%s\n", auxcomanda);

    	char *com1 = strtok(auxcomanda, "\n ");
    	//printf("%s\n", com1);
    	//printf("%s\n", comanda);
    	
    	int len = strlen(com1);

    	for(int i=0; com1[i]; ++i)
    	{
    		com1[i] = tolower(com1[i]);
    	}


    	if(strcmp(com1, "help") == 0)
    	{
    		printf("\n- Create < program_name > - creates the service that acceses the program\n");
    		printf("- Open < program_name > - runs the program\n");
    		printf("- Status < program_name > - displays the status of the service\n");
    		printf("- Resume < program_name > - resumes the service \n");
    		printf("- Cancel < program_name > - stops the service \n");
    		printf("- Remove < program_name > - removes the service from the service list but does not stop it \n");
    		printf("- Close < program_name > - removes the service from the service list and stops it \n");
    		printf("- List - lists all services managed by the supervisor\n");
    		printf("- Free - closes all services managed by the supervisor\n");
    		printf("- Exit - leave the program\n");
    	}
    	else if(strcmp(com1, "create") == 0 || strcmp(com1, "open") == 0 || strcmp(com1, "status") == 0 || strcmp(com1, "cancel") == 0 || strcmp(com1, "remove") == 0 || strcmp(com1, "close") == 0 || strcmp(com1, "list") == 0 || strcmp(com1, "free") == 0 || strcmp(com1, "resume") == 0)
    	{
    		int s = send(network_socket, comanda, strlen(comanda), 0);
    	}
    	else if(strcmp(com1, "exit") == 0)
    	{
    		int s = send(network_socket, comanda, strlen(comanda), 0);
    		return NULL;
    	}
    	else
    	{
    		printf("\nComanda gresita\n");
    	}


    	memset(comanda, 0, sizeof(comanda));
    	memset(com1, 0, sizeof(com1));
    	memset(auxcomanda, 0, sizeof(auxcomanda));

    	//printf("%d\n", s);

    	//int valread = read(network_socket, server_response, 256);

    	//printf("The server sent: %s\n", server_response);
	}

    close(network_socket);
}








