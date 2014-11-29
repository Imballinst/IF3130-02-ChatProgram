/* File     : tcpclient.c
 * Author   : Try Ajitiono
 * Reference: http://robertoacevedo.net/blog/2012/12/03/socket-server/
 * 			  ...and many other websites, especially stackoverflow.com
 */

/* Header file */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include "adtfungsiprosedur.h"

/* Header fungsi dan prosedur */

void handleActions(int sockfd, char *prevmsg);
/* Melakukan handle aksi berdasarkan pesan yang dikirimkan sebelumnya. Contoh: sebelumnya mengirimkan pesan signup, maka akan dilakukan penanganan
 * untuk signup tersebut.
 * Param: integer socket dan string pesan sebelumnya.
 */

/* Program Utama */

int main(int argc, char *argv[]) {
	//deklarasi variabel
	int sockfd, rw; //file deskriptor dan penampung return value read/write
	char *buffer, *comparison, *response; //buffer pesan, comparison, dan server response untuk dimasukkan ke fungsi
	//deklarasi struktur
	struct addrinfo flags; //parameter yang digunakan untuk melakukan listen socket
	struct addrinfo *server_info; //resultset yang diset oleh getaddrinfo()
	//apabila salah command ketika run
	if (argc < 3) { //apabila tidak memiliki argumen address server dan port
		fprintf(stderr, "Usage: ./client <hostname/address> <port>");
		exit(-1);
	}
	//inisialisasi buffer dan comparison
	buffer = malloc(BUFFER_SIZE);
	comparison = malloc(BUFFER_SIZE);
	response = malloc(BUFFER_SIZE);
	//mengosongkan memori flags
	memset(&flags, 0, sizeof(flags));
	//mengisi struktur flags
	flags.ai_family = AF_UNSPEC; //IPv4 atau IPv6
	flags.ai_socktype = SOCK_STREAM; //TCP, sesuai spek
	flags.ai_flags = AI_PASSIVE; //set address
	//mengambil ip address server
	if (getaddrinfo(argv[1], argv[2], &flags, &server_info) < 0) { //apabila gagal
		perror("Couldn't find host");
		exit(-1);
	}
	//membuat socket
	sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	//connect ke socket server
	if (connect(sockfd, server_info->ai_addr, server_info->ai_addrlen) < 0)	{ //apabila gagal
		perror("Couldn't conenct...");
		exit(-1);
	}
	//koneksi sukses
	do {
		printf("Connection established, please enter a message:\n");
		//mengosongkan buffer
		bzero(buffer, BUFFER_SIZE);
		bzero(comparison, BUFFER_SIZE);
		//mengisi buffer dengan pesan
		fgets(buffer, BUFFER_SIZE - 1, stdin);
		strcpy(comparison,buffer);
		//write isi pesan ke dalam buffer lalu dimasukkan ke socket untuk dikirim
		rw = write(sockfd, buffer, strlen(buffer));
		if (rw < 0)	{ //apabila gagal
			perror("Failed to send message");
			exit(-1);
		}
		//mengosongkan buffer lagi
		bzero(buffer, BUFFER_SIZE);
		//membaca isi buffer
		rw = read(sockfd, buffer, BUFFER_SIZE);
		if (rw < 0) { //apabila gagal
			perror("Error reading from socket");
			exit(-1);
		}
		printf("The message is: %s\n", buffer); //menerima pesan ACK
		handleActions(sockfd, comparison);
	} while (checkExitMsg(comparison) == 0); //selama bukan "exit"
	//dealokasi memori
	free(buffer);
	free(comparison);
	free(response);
	freeaddrinfo(server_info);
	//menutup socket setelah exit
	close(sockfd);
	return 0;
}

void handleActions(int sockfd, char *prevmsg) {
	printf("Handle actions! Prev msg: %s", prevmsg);
	int rw;
	char *nama, *password;
	if (strcmp("signup\n",prevmsg) == 0) {
		nama = malloc(BUFFER_SIZE);
		password = malloc(BUFFER_SIZE);
		bzero(nama, BUFFER_SIZE);
		bzero(password, BUFFER_SIZE);
		printf("Nama:");
		fgets(nama, BUFFER_SIZE-1, stdin);
		rw = write(sockfd, nama, strlen(nama));
		if (rw < 0) {
			perror("Write nama ke server error");
			exit(-1);
		}
	}
}