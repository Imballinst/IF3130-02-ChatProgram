/* File  : tcpclient.c
 * Author: Try Ajitiono / 13512052
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg) {
	perror(msg);
	exit(0);
}

int checkExitMsg(char *msg) {
	int i = 0, ret = 0;
	if (msg[0] == 'e') {
		if (msg[1] == 'x') {
			if (msg[2] == 'i') {
				if (msg[3] == 't') {
					ret = 1;
				}
			}
		}
	}
	return ret;
}

int main(int argc, char *argv[]) {
	//for reference, look tcpserver.c
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	//pointer to hostent structure, defines host computer
	//on the internet
	struct hostent *server;
	//common: start the program with arguments
	char buffer[256];
	if (argc < 3) {
		fprintf(stderr, "Usage %s hostname port", argv[0]);
		exit(0);
	}
	//set port number and socket file descriptor
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	//get hostname address
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host");
		exit(0);
	}
	//the same as in tcpserver.c
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	//copy the length bytes from server host address
	//to s_addr in sin_addr in serv_addr variable
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	//if connection to server fails
	if (connect (sockfd, &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");
	printf("Please enter the message: ");
	//set buffer to zero
	bzero(buffer,256);
	//get the message input
	fgets(buffer, 255, stdin);
	while (!checkExitMsg(buffer)) {
		//n returns 1 if success writing into socket file descriptor
		n = write(sockfd, buffer, strlen(buffer));
		//if fail
		if (n < 0)
			error("ERROR writing to socket");
		//set buffer to zero
		bzero(buffer,256);
		//n returns 1 if success reading from buffer
		n=  read(sockfd, buffer, 255);
		if (n < 0)
			error("ERROR reading from socket");
		printf("%s", buffer);
		printf("Please enter the message: ");
		//set buffer to zero
		bzero(buffer,256);
		//get the message input
		fgets(buffer, 255, stdin);
	}
	return 0;
	//lanjut http://www.linuxhowtos.org/C_C++/socket.htm
}
