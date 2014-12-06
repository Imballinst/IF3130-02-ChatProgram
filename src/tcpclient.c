/* File     : tcpclient.c
 * Author   : Try Ajitiono
 * Reference: http://robertoacevedo.net/blog/2012/12/03/socket-server/
 * 			  ...and many other websites, especially stackoverflow.com
 */

/* Header file */

#include "adtfungsiprosedur.h"
#include "tcpclient.h"

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
	printf("Connection established!\n");
	do {
		printf("Please enter a message: ");
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
	int rw;
	char *buffer, *response;
	if (strcmp("signup\n",prevmsg) == 0) { //signup
		char *user;
		//alokasi
		user = malloc(BUFFER_SIZE);
		response = malloc(BUFFER_SIZE);
		buffer = malloc(BUFFER_SIZE);
		//mengosongkan buffer
		bzero(user, BUFFER_SIZE);
		bzero(buffer, BUFFER_SIZE);
		bzero(response, BUFFER_SIZE);
		printf("Nama    :");
		//insert nama [1]
		fgets(buffer, BUFFER_SIZE-1, stdin);
		strcpy(user,buffer);
		rw = write(sockfd, buffer, strlen(buffer));
		if (rw < 0) {
			perror("Write nama ke server error");
			exit(-1);
		}
		//mengosongkan buffer
		bzero(buffer, BUFFER_SIZE);
		printf("Password:");
		//insert password [2]
		fgets(buffer, BUFFER_SIZE-1, stdin);
		rw = write(sockfd, buffer, strlen(buffer));
		if (rw < 0) {
			perror("Write password ke server error");
			exit(-1);
		}
		//membaca reply server [3]
		rw = read(sockfd, response, BUFFER_SIZE);
		if (rw < 0) {
			perror("Error membaca balasan server\n");
			exit(-1);
		}
		if (strcmp(response,"Username berhasil dibuat!\n") == 0) { //kalau berhasil
			createClientLogFolder(user);
		}
		printf("Reply dari server: %s\n", response);
		free(buffer);
		free(response);
		free(user);
	}
	else if (strcmp("login\n",prevmsg) == 0) { //login
		char *temp;
		temp = malloc(25);
		response = malloc(BUFFER_SIZE);
		buffer = malloc(BUFFER_SIZE);
		//mengosongkan buffer
		bzero(buffer, BUFFER_SIZE);
		bzero(response, BUFFER_SIZE);
		printf("Nama    :");
		//insert nama [1]
		fgets(buffer, BUFFER_SIZE-1, stdin);
		rw = write(sockfd, buffer, strlen(buffer));
		strcpy(temp,buffer);
		if (rw < 0) {
			perror("Write nama ke server error");
			exit(-1);
		}
		//mengosongkan buffer
		bzero(buffer, BUFFER_SIZE);
		printf("Password:");
		//insert password [2]
		fgets(buffer, BUFFER_SIZE-1, stdin);
		rw = write(sockfd, buffer, strlen(buffer));
		if (rw < 0) {
			perror("Write password ke server error");
			exit(-1);
		}
		//membaca reply server [3]
		rw = read(sockfd, response, BUFFER_SIZE);
		if (rw < 0) {
			perror("Error membaca balasan server\n");
			exit(-1);
		}
		printf("Reply dari server: %s\n", response);
		if(strcmp(response,"Sukses login!\n")==0){
			strcpy(user,temp);
			
		}
		free(buffer);
		free(response);
		free(temp);
	}
	else if (strcmp("logout\n",prevmsg) == 0) { //logout
		//do nothing
		bzero(user, 25);
	}
	else if(isMessage(prevmsg)){
		response = malloc(BUFFER_SIZE);
		//membaca respon server terkait ada atau tidak di server
		rw = read(sockfd, response, strlen(response));
		if (strcmp(response,"User tidak ada di database") == 0) { //tidak ada di database
			printf("%s\n",response);
		}
		else { //ada di database
			buffer = malloc(BUFFER_SIZE);
			//mengosongkan buffer
			bzero(buffer, BUFFER_SIZE);
			printf("Message    :");
			//insert message
			fgets(buffer, BUFFER_SIZE-1, stdin);
			rw = write(sockfd, buffer, strlen(buffer));
			if (rw < 0) {
				perror("Message failed.\n");
				exit(-1);
			}
			else {
				printf("Message sent.\n");
			}
			free(buffer);
		}
		free(response);
	}
	else if(isShowMessage(prevmsg)){
		char *user_;
		user_ = malloc(25);
		bzero(user_,25);
		int msgLength = strlen(prevmsg);
		int showUserLength = msgLength - 4;
		char *showUser = (char*) malloc(showUserLength);
		strncpy(showUser,prevmsg+5,showUserLength);
		strcpy(user_,user);
		user_ = removeNewline(user_);
		showUser = removeNewline(showUser);
		if(isUserExistDB(showUser)){
			char path[100] = "assets/client/chat_log/";
			strncat(path,user_,strlen(user_));
			strncat(path,"/",1);
			strncat(path,showUser,strlen(showUser));
			strncat(path,".txt",4);
			char pathDummy[100] = "assets/client/chat_log/";
			strncat(pathDummy,user_,strlen(user_));
			strncat(pathDummy,"/",1);
			strncat(pathDummy,"dummy.txt",9);
			FILE *FUser = fopen(path,"r");
			FILE *FDummy = fopen(pathDummy,"a");
			if(FUser){
				if(FDummy){ // tdak gagal membaca path pathDummy
					//ngebaca FUser dipindahin ke FDummy
					char line[256];
					printf("\n");
					while (fgets(line, sizeof(line), FUser)) {
						if(strcmp(line,"#\n") == 0){
							//strcat(isiMessage,"----- New Message(s) -----\n");
							printf("----- New Message(s) -----\n");
					    }
					    else{
					    	//strncat(isiMessage,line,strlen(line));
					    	printf("%s",line);
							fputs(line,FDummy);
						}
					}
				}
			}
			fclose(FUser);
			fclose(FDummy);
			FILE *FUser2 = fopen(path,"w");
			if(FUser2){
				fputs("",FUser2);
			}
			fclose(FUser2);
			FILE *FDummy2 = fopen(pathDummy,"r");
			FILE *FUser3 = fopen(path,"a");
			if(FUser3){
				if(FDummy2){ 
					char line_[256];
					while (fgets(line_, sizeof(line_), FDummy2)) {
						fputs(line_,FUser3);
					}
				}
			}
			fclose(FDummy2);
			fclose(FUser3);
			
			FILE *FDummy3 = fopen(pathDummy,"w");
			if(FDummy3){
				fputs("",FDummy3);
			}
			fclose(FDummy3);
			FILE *FUser_ = fopen(path,"a");
			if(FUser_){
				fputs("#\n",FUser_);
			}
			fclose(FUser_);
		}
		else{
			printf("Tidak ada user dengan nama tersebut.\n");
		}
	}
}

void createClientLogFolder(char *username) {
	char path[75] = "assets/client/chat_log/"; //path
	username = removeNewline(username);
	strncat(path,username,strlen(username)); //concat path dengan username
	if (stat(path,&st) == -1) { //apabila path belum ada, bikin baru
		mkdir(path,0777);
		printf("Successfully created client log folder %s\n", path);
	}
	bzero(path,75);
	char path2[50] = "assets/server/pending_messages/";
	strncat(path2,username,strlen(username));
	strncat(path2,".txt",4);
	FILE *f = fopen(path2,"w");
	if (f) {
		printf("a\n");
	}
	else {
		printf("b\n");
	}
	fclose(f);
}