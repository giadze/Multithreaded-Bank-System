#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "types.h"
#include "var.h"
#include "linkedList.h"
#include "hashmap.h"
#include "tokenizer.h"
#include "bank.h"


typedef struct clientConnection* clientConnection;
typedef struct account* account;


struct clientConnection 
{

	int connectionFileDescrip;
	pthread_t thread;
	account acc;

};



struct account 
{

	pthread_mutex_t mutex;
	string name;
	double balance;
	bool active;

};



pthread_mutex_t accounts_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
map accounts;
linkedList clients;


account openAccount(string token) 
{

	account acc = (account) malloc (sizeof(struct account));
	acc -> name = token;
	acc -> balance = 0;
	acc -> active = false;

	pthread_mutexattr_t attr;


	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);


	if(pthread_mutex_init(&acc -> mutex, &attr))
	{

		perror("Couldn't initialize mutex for account.");
		exit(0);

	}

	return acc;

}


void closeClient(void* clientConn)
{

	if(clientConn)
	{

		clientConnection c = (clientConnection) clientConn;
		free(c);

	}

}


void printStatus(int signum)
{

	int flag = 0;

	if(pthread_mutex_trylock(&accounts_mutex) == 0)
	{

		printf("Bank Information:\n\n");

		for(int n = 0; n < 20; n ++)
		{

			if(accounts -> values[n])
			{

				flag = 1;
				linkedList l = accounts -> values[n];
				linkedNode n = l -> head;


				while(n){

					account acc = (account) ((var) n -> content) -> content;
					string name = acc -> name;
					double balance = acc -> balance;
					int active = acc -> active;

					printf("Name: %s\n\tBalance: $%.2f\n\t%s\n\n", name, balance, active ? "IN SERVICE" : "");


					n = n -> next;

				}

			}

		}



		if (!flag) printf("Empty bank.\n");

		printf("-----------\n");


		pthread_mutex_unlock(&accounts_mutex);

	} else {

		printf("Unable to acquire mutex lock when printing stats\n");	

	}

}


int streq(char* a, char* b)
{

	return strcmp(a, b) == 0;

}


/*Bunch of if statements*/
void* clientHandler(void* c)
{	

	clientConnection client = (clientConnection) c;
	int socketDescriptor = client->connectionFileDescrip;


	pthread_detach(pthread_self());


	char request[2048];
	Tokenizer tokenizer = 0;
	double change = 0;


	write(socketDescriptor, request, sprintf(request, "Server: Ready to receive input from client.\n") + 1);


	while(read(socketDescriptor, request, sizeof(request)) > 0)
	{

		if(streq(request, "0") || streq(request, ""))

			continue;


		printf("Server received input:	<%s>\n", request);
		tokenizer = createTokenizer(request);
		string token = 0;
		token = getNextToken(tokenizer);


		if (streq(token, "open")) 
		{ 

		if (!(token = getNextToken(tokenizer)) || getNextToken(tokenizer)) 
		{

				write(socketDescriptor, request, sprintf(request, "Bank: Invalid Syntax.\n") + 1);

			} else if (getKeyValue(token, accounts)) {

				write(socketDescriptor, request, sprintf(request, "Bank: Cannot open account %s - account name already exists.\n", token) + 1); 

			} else if (accounts -> count == 20) {

				write(socketDescriptor, request, sprintf(request, "Bank: Twenty accounts exist already; cannot open another.\n") + 1);

			} else { 

				account acc = openAccount(token); 
				pthread_mutex_lock(&accounts_mutex);
				insertMapValue(acc -> name, acc, accounts);
				pthread_mutex_unlock(&accounts_mutex);
				write(socketDescriptor, request, sprintf(request, "Bank: Successfully added account %s.\n", acc->name) + 1);

			}

		} else if (streq(token, "start")) { 


			if (!(token = getNextToken(tokenizer)) || getNextToken(tokenizer)) 
			{

				write(socketDescriptor, request, sprintf(request, "Bank: Invalid syntax.\n") + 1);
				continue;

			}


			if (client -> acc) 
			{

				write(socketDescriptor, request, sprintf(request, "Bank: Already serving account %s.\n", client->acc->name) + 1);

				continue;

			}


			if ((client -> acc = (account) getKeyValue(token, accounts)) == NULL) 
			{

				write(socketDescriptor, request, sprintf(request, "Bank: Account %s does not exist.\n", token) + 1);
				continue;

			} 

			account acc = client -> acc;


			if (client -> acc -> active) 
			{

				write(socketDescriptor, request, sprintf(request, "Bank: Account %s is already in use.\n", token) + 1);

				client -> acc = NULL;

			} 


			while(pthread_mutex_trylock(&acc -> mutex) != 0)
			{

				printf("Error attempting to lock already locked mutex: %d\n", pthread_mutex_trylock(&acc->mutex));

				write(socketDescriptor, request, sprintf(request, "Bank: Waiting to start customer session for account %s.\n", token) + 1);

				sleep(2);

			}


			client -> acc = acc;
			client -> acc -> active = true;

			write(socketDescriptor, request, sprintf(request, "Bank: Successfully connected to account %s.\n", token) + 1);

		} else if (streq(token, "credit")) {

			if (!client -> acc || !client -> acc -> active) 
			{

				write(socketDescriptor, request, sprintf(request, "Bank: Not currently in a customer session.\n") + 1);


			} else if (!(token = getNextToken(tokenizer)) || getNextToken(tokenizer)){

				write(socketDescriptor, request, sprintf(request, "Bank: Invalid syntax.\n") + 1);


			} else if (!(change = atof(token)) || change <= 0) {

				write(socketDescriptor, request, sprintf(request, "Bank: Not a valid credit.\n") + 1); 

			} else {

				write(socketDescriptor, request, sprintf(request, "Old Balance:\t$%.2lf\nDepositing:\t$%.2lf\nNew Balance:\t$%.2lf\n", client -> acc -> balance, change, (client -> acc -> balance + change)) + 1);

				client -> acc ->balance += change;

			}

		} else if (streq(token, "debit")) { 

			if (!client -> acc || !client -> acc -> active) 
			{

				write(socketDescriptor, request, sprintf(request, "Bank: Not currently in a customer session.\n") + 1);


			} else if (!(token = getNextToken(tokenizer)) || getNextToken(tokenizer)) {

				write(socketDescriptor, request, sprintf(request, "Bank: Invalid syntax.\n") + 1);


			} else if (!(change = atof(token)) || change <= 0) {

				write(socketDescriptor, request, sprintf(request, "Bank: Not a valid debit.\n") + 1); 


			} else if (change > client -> acc -> balance) {

				write(socketDescriptor, request, sprintf(request, "Bank: Not enough funds to debit.\n") + 1);

			} else {

				write(socketDescriptor, request, sprintf(request, "Old Balance:\t$%.2lf\nDepositing:\t$%.2lf\nNew Balance:\t$%.2lf\n", client -> acc -> balance, change, (client -> acc -> balance - change)) + 1);

				client -> acc -> balance -= change;

			}

		} else if (streq(token, "balance")) { 


			if (!client -> acc || !client -> acc -> active) 
			{

				write(socketDescriptor, request, sprintf(request, "Bank: Not currently in a customer session.\n") + 1);

			} else if (getNextToken(tokenizer)) {

				write(socketDescriptor, request, sprintf(request, "Bank: Invalid syntax.\n") + 1);

			} else {

				write(socketDescriptor, request, sprintf(request, "Balance: $%.2lf\n", client -> acc -> balance) + 1);

			}

		} else if (streq(token, "finish")) { 


			if (!client -> acc || !client -> acc -> active) 
			{

				write(socketDescriptor, request, sprintf(request, "Bank: Not currently in a customer session.\n") + 1);

			} else {

				write(socketDescriptor, request, sprintf(request, "Bank: Ending customer session to account %s.\n", client->acc->name) + 1);

				client -> acc -> active = false;
				pthread_mutex_unlock(&client -> acc -> mutex);
				client -> acc = NULL;

				printf("Ended customer session and unlocked account mutex\n");

			}

		} else if (streq(token, "exit")) { 

			write(socketDescriptor, request, sprintf(request, "Bank: Disconnecting from Server and ending Client thread.\n") + 1);


			if (client -> acc)
			{
				client -> acc -> active = false;
				pthread_mutex_unlock(&client -> acc -> mutex);

			}


			pthread_mutex_lock(&clients_mutex);
			removeLinkedNode(client, clients);
			pthread_mutex_unlock(&clients_mutex);
			closeClient(client);
			close(socketDescriptor);
			destroyTokenizer(tokenizer);

			break;

		} else {

			write(socketDescriptor, request, sprintf(request, "Invalid syntax. Proper commands:\nopen (account_name) \topens an account\nstart (account_name)\tstarts a customer session\ncredit (amount)\tcredits amount into account\ndebit (amount)\tdebits amount from account\nbalance\t\t\tprints account balance\nfinish\t\t\tfinishes customer session\nexit\t\t\texits client session\n") + 1);

		}

		destroyTokenizer(tokenizer);

	}

	return NULL;

}


void setTimer(int seconds)
{	

	struct itimerval timer;
	timer.it_value.tv_sec = seconds;
	timer.it_value.tv_usec = 0;
	timer.it_interval.tv_sec = seconds;
	timer.it_interval.tv_usec = 0;
	setitimer (ITIMER_REAL, &timer, NULL);

}


int main()
{
	struct sockaddr_in serverAddress;
	int listenFileDescrip = socket(AF_INET, SOCK_STREAM, 0);
	int connectionFileDescrip = 0;

	accounts = newMap(20, 1);
	clients = newLinkedList(NULL);

	memset(&serverAddress, '0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	struct sigaction sa;


	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = printStatus;
	sigaction (SIGALRM, &sa, NULL);
	setTimer(20);


	serverAddress.sin_port = htons(__SERVER_PORT__);

	if(bind(listenFileDescrip, (struct sockaddr*) &serverAddress, sizeof(serverAddress)))
	{

		perror("Unable to bind address/port.");
		return 0;

	}


	if(listen(listenFileDescrip, 100))
	{

		perror("Unable to make port as listen.");
		return 0;

	}


	while(1)
	{

		connectionFileDescrip = accept(listenFileDescrip, NULL, NULL);


		if(connectionFileDescrip == -1)
		{

			continue;

		}


		printf("Accepted new client connection.\n");

		clientConnection client = malloc(sizeof(*client));
		client -> connectionFileDescrip = connectionFileDescrip;
		client -> acc = 0;

		/*Adding client to list*/
		linkedNode node = newLinkedNode(client);


		pthread_mutex_lock(&clients_mutex);
		appendLinkedNode(node, clients);
		pthread_mutex_unlock(&clients_mutex);

	
		pthread_create(&client -> thread, NULL, clientHandler, client);
		sleep(1);

	}

	pthread_mutex_lock(&clients_mutex);
	deleteLinkedList(clients, closeClient);
	pthread_mutex_unlock(&clients_mutex);
	close(listenFileDescrip);
	close(connectionFileDescrip);

	return 0;

}

