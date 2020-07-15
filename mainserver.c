
/*
* Assignment 2: Data Transformation Services
* Juan Bondad 30050828
* 
* This is the main server which uses TCP as the transport layer protocol for data transfer with the main client
* UDP is used for connection with the micro services.
*
* Originally Written by Carey Williamson January 13, 2012 */

/* Include files for C socket programming and stuff */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>

/* Global manifest constants */
#define MAX_MESSAGE_LENGTH 100
#define MYPORTNUM 5550
#define MAX_BUFFER_SIZE 100
#define SERVER_IP "127.0.0.1" 

/* Optional verbose debugging output */
#define DEBUG 1

/* Global variable */
int childsockfd;

/* Transform the message */
void transformMessage(char* command, char* messagein, char*messageout){
		for(int i = 0; command[i] != '\0'; i++){
			unsigned int PORTNUM = 0;
			if(command[i] == '1'){ // assign portnumber based on command
				PORTNUM = 5551;
			}
			else if(command[i] == '2'){
				PORTNUM = 5552;
			}
			else if(command[i] == '3'){
				PORTNUM = 5553;
			}
			else if(command[i] == '4'){
				PORTNUM = 5554;
			}
			else if(command[i] == '5'){
				PORTNUM = 5555;
			}
			else if(command[i] == '6'){
				PORTNUM = 5556;
			}
			struct sockaddr_in si_server;
			struct sockaddr *server;
			int s, i, len = sizeof(si_server);
			char buf[MAX_BUFFER_SIZE];
			int readBytes;


			if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
			{
			printf("Could not set up a socket!\n");
			}
			
			memset((char *) &si_server, 0, sizeof(si_server));
			si_server.sin_family = AF_INET;
			si_server.sin_port = htons(PORTNUM);

			server = (struct sockaddr *) &si_server;
            // send message
			if (sendto(s, messagein, strlen(messagein), 0, server, sizeof(si_server)) == -1)
			{
				printf("sendto failed\n");
			}
            // recieve message
			if ((readBytes=recvfrom(s, messageout, MAX_BUFFER_SIZE, 0, server, &len))==-1)
			{
				printf("Read error!\n");
			}
			printf("Message: '%s'\n", messageout);
			strcpy(messagein,messageout);
			close(s);

		}

}

/* This is a signal handler to do graceful exit if needed */
void catcher( int sig )
  {
    close(childsockfd);
    exit(0);
  }

/* Main program for server */
int main()
  {
    struct sockaddr_in server;
    static struct sigaction act;
    char messagein[MAX_MESSAGE_LENGTH];
    char messageout[MAX_MESSAGE_LENGTH];
    int parentsockfd;
    int i, j;
    int pid;
    char c;

    /* Set up a signal handler to catch some weird termination conditions. */
    act.sa_handler = catcher;
    sigfillset(&(act.sa_mask));
    sigaction(SIGPIPE, &act, NULL);

    /* Initialize server sockaddr structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(MYPORTNUM);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* set up the transport-level end point to use TCP */
    if( (parentsockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1 )
      {
	fprintf(stderr, "socket() call failed!\n");
	exit(1);
      }

    /* bind a specific address and port to the end point */
    if( bind(parentsockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in) ) == -1 )
      {
	fprintf(stderr, "bind() call failed!\n");
	exit(1);
      }

    /* start listening for incoming connections from clients */
    if( listen(parentsockfd, 5) == -1 )
      {
	fprintf(stderr, "listen() call failed!\n");
	exit(1);
      }

    /* initialize message strings just to be safe (null-terminated) */
    bzero(messagein, MAX_MESSAGE_LENGTH);
    bzero(messageout, MAX_MESSAGE_LENGTH);
    fprintf(stderr, "server listening on TCP port %d...\n\n", MYPORTNUM);

    /* Main loop: server loops forever listening for requests */
    for( ; ; )
      {
	/* accept a connection */
	if( (childsockfd = accept(parentsockfd, NULL, NULL)) == -1 )
	  {
	    fprintf(stderr, "wordlen-TCPserver: accept() call failed!\n");
	    exit(1);
	  }

	/* try to create a child process to deal with this new client */
	pid = fork();

	/* use process id (pid) returned by fork to decide what to do next */
	if( pid < 0 )
	  {
	    fprintf(stderr, "wordlen-TCPserver: fork() call failed!\n");
	    exit(1);
	  }
	else if( pid == 0 )
	  {
	    /* the child process is the one doing the "then" part */

	    /* don't need the parent listener socket that was inherited */
	    close(parentsockfd);

	    /* obtain the message from this client */
	    while( recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0) > 0 )
	      {
		/* print out the received message */
		printf("Child process received word: %s\n", messagein);

		char *command = strrchr(messagein,' ') + 1; // seperate command from message
		printf("Command: %s\n", command);

		size_t m_len = strlen(messagein); // get size of message
		size_t cmd_len = strlen(command); // get size of command
		if(m_len >= cmd_len){
			messagein[m_len-cmd_len-1] = 0; // remove command from message
		}
		printf("Original Message: '%s'\n", messagein);
		transformMessage(command,messagein,messageout);

		printf("Message After Data Transformation: '%s'\n", messageout);

		




#ifdef DEBUG
		printf("Child about to send message: %s\n", messageout);
#endif

		/* send the result message back to the client */
		send(childsockfd, messageout, strlen(messageout), 0);

		/* clear out message strings again to be safe */
		bzero(messagein, MAX_MESSAGE_LENGTH);
		bzero(messageout, MAX_MESSAGE_LENGTH);
	      }

	    /* when client is no longer sending information to us, */
	    /* the socket can be closed and the child process terminated */
		
	    close(childsockfd);
	    exit(0);
	  } /* end of then part for child */
	else
	  {
	    /* the parent process is the one doing the "else" part */
	    fprintf(stderr, "Created child process %d to handle that client\n", pid);
	    fprintf(stderr, "Parent going back to job of listening...\n\n");

	    /* parent doesn't need the childsockfd */
	    close(childsockfd);
	  }
      }
  }
