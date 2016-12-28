#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include "bank.h"
#define h_addr h_addr_list[0]

pthread_t thread;
pthread_t inputHandler;
int file_descrip;
int exit_out; 


int compareString(char* a, char* b)
{

	return strcmp(a, b) == 0;

}

/*Print output from server*/
void* handler(void* server_name)
{ 

	char* server = (char*) server_name;
	char request[2048];


	printf("Receive Server Output Status: READY\n"); 


	while(read(file_descrip, request, sizeof(request)) > 0)
	{
		printf("%s", request);

	}

	return NULL;

}

void* inputSend(void* ignore)
{

	printf("Status of Client to Take Input: READY\n");

	char input[2048];

	while(1)
	{
		scanf(" %[^\n]", input);
		write(file_descrip, input, strlen(input) + 1);


		if(compareString(input, "exit"))
		{ 

			pthread_kill(thread, SIGINT);
			printf("Disconnected from server.\n");

			exit(0);

		}


		sleep(2);

	}

}


void set_iaddr_str(struct sockaddr_in * sockaddr, char * x, unsigned int portNum )
{ 

	struct hostent * hostptr;
	memset( sockaddr, 0, sizeof(*sockaddr) );

	sockaddr -> sin_family = AF_INET;
	sockaddr -> sin_port = htons(portNum);

	if ((hostptr = gethostbyname(x)) == NULL ) 
	{

		printf("Getting addr information status: ERROR\n" );

	} else {

		bcopy(hostptr->h_addr, (char *)&sockaddr->sin_addr, hostptr->h_length );

	}

}


int main(int argc, char** argv)
{

	if(argc != 2)
	{

		printf("Error: Server host must be specified as a command line argument.\n");

		return 0;

	}

	int x = 0;

	char* server_name = argv[1];

	file_descrip = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddress;

	set_iaddr_str(&serverAddress, server_name, __SERVER_PORT__);


	while( (x = connect(file_descrip, (const struct sockaddr*) &serverAddress, sizeof(serverAddress)))) 
	{

		printf("Attempting to connect to %s...\n", server_name);
		perror("");
		sleep(3);

	}

	printf("Connected to Server.\n");

	pthread_create(&thread, NULL, handler, server_name);
	pthread_detach(thread);
	pthread_create(&inputHandler, NULL, inputSend, NULL);
	pthread_detach(thread);


	while(write(file_descrip, "0", 2) == 2)
	{

		sleep(1);

	}

	/*Cleanup*/
	pthread_kill(thread, SIGINT);
	pthread_kill(inputHandler, SIGINT);
	close(file_descrip);

	printf("Disconnected From Server.\n");

	return 0;

}

