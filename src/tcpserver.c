/* File  : tcpclient.c
 * Author: Try Ajitiono / 13512052
 */

#include <stdio.h> //input output
#include <sys/types.h> //data types
#include <sys/socket.h> //struct definition socket
#include <netinet/in.h> //const, structs, inet domain addr
#include <pthread.h> //threading
#include <stdlib.h> //null value

/* Header */

void error(char *msg);
int checkExitMsg(char *msg);
void write();
void read();
int checkUsername(char *input);

/* Main Program */

int main(int argc, char *argv[]) {
	int sockfd, newsockfd; //socket syscall, acc syscall
	int portno; //port number server acc connection
	int clilen; //size client address
	int n; //value for the read(), write() calls
	char buffer[256]; //reads char from socket conn
	//client address and server address
	struct sockaddr_in serv_addr, cli_addr;
	//user must provide port on program argument run
	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided");
		exit(1);
	}
	//create socket: domain, type, protocol
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//if socket fails to open
	if (sockfd < 0) {
		error("ERROR opening socket");
	}
	//set every buffer to zero
	bzero((char *) &serv_addr, sizeof(serv_addr));
	//set the port to 1st argument of the run sentence
	portno = atoi(argv[1]); //digits -> integer
	//set sockaddr_in attribute
	serv_addr.sin_family = AF_INET; //absolute
	serv_addr.sin_port = htons(portno); //port number
	serv_addr.sin_addr.s_addr = INADDR_ANY; //host ipaddr
	//socket use fail check
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	//socket file descriptor and number of queue allowed
	listen(sockfd,5);
	//client address recognized
	clilen = sizeof(cli_addr);
	//new file descriptor, client addr ptr, struct size
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");
	//initialize the buffer
	bzero(buffer,256);
	//read from socket, buffer 0-255, 256 size
	//read method block operation until client send smth
	n = read(newsockfd, buffer, 255);
	if (n <  0) error("ERROR reading from socket");
	//finishes reading, printing the message
	while (!checkExitMsg(buffer)) {
		printf("Here is the message: %s", buffer);
		//n determines if the message succesfully written
		//into the socket, length = 18
		n = write(newsockfd, "I got your message\n", 18);
		if (n < 0) error("ERROR writing to socket");
		//initialize the buffer
		bzero(buffer,256);
		//read from socket, buffer 0-255, 256 size
		//read method block operation until client send smth
		n = read(newsockfd, buffer, 255);
		if (n <  0) error("ERROR reading from socket");
	}
	return 0;
}

void error(char *msg) {
	//used when system call fails
	perror(msg);
	exit(1);
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

void write() {
	char user[255], pass[255];
	FILE *f = fopen("assets/users.txt","a");
	if (f) {
		do {
			printf("Masukin username: ");
			scanf("%s",user);
		} while (checkUsername(user) == 1);
		scanf("%s", pass);
		fprintf(f,"%s\t", user);
		fprintf(f,"%s\n", pass);
		fclose(f);
	}
}

void read() {
	char output[255];
	FILE *f = fopen("assets/users.txt","r");
	if (f) {
		while (fgets(output,255,f) != NULL) {
			printf("%s",output);
		}
		fclose(f);
	}
}

int checkUsername(char *input) {
	char output[255];
	int ret = 0, i, stat = 1, n = 1;
	FILE *f = fopen("assets/users.txt","r");
	if (f) {
		fgets(output,255,f); //skip first line
		while (fgets(output,255,f) != NULL && ret == 0) {
			i = 0;
			while (input[i] != '\t' && output[i] != '\t' && stat == 1) { //bukan akhir tab username
				if (input[i] != output[i]) { //kalau tidak sama, langsung keluar
					stat = 0;
				}
				else { //kalau sama, lanjut
					i++;
					if (input[i] == NULL && output[i] == '\t') { //kalau lanjut sampai akhir
						ret = 1;
						stat = 0;
						printf("Username sudah ada\n");
					}
				}
			}
			stat = 1;
		}
		fclose(f);
	}
	return ret;
}
