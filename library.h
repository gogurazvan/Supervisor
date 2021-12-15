supervisor_t supervisor_init();

int supervisor_close(supervisor_t);


service_t service_create(
	supervisor_t supervisor,
	const char * servicename,
	const char * program_path,
	const char ** argv,
	int argc,
	int flags
);

int service_close(service_t service);


service_t service_open(
	supervisor_t supervisor,
	const char * servicename
);

#define SUPERVISOR_STATUS_RUNNING 0x1
#define SUPERVISOR_STATUS_PENDING 0x2
#define SUPERVISOR_STATUS_STOPPED 0x4

int service_status(service_t service);



int service_suspend(service_t service);

int service_resume(service_t service);



int service_cancel(service_t service);

int service_remove(service_t service);


int supervisor_list(
	supervisor_t supervisor,
	char *** service_names,
	unsigned int * count
);

int supervisor_freelist(
	char ** service_names,
	int count
);