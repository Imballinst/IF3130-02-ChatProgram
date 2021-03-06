/* File     : tcpserver.c
 * Author   : Try Ajitiono
 * Reference: http://robertoacevedo.net/blog/2012/12/03/socket-server/
 * 			  ...and many other websites, especially stackoverflow.com
 */

/* Header File */

#include "tcpserver.h"

/* Variabel-variabel global */

pthread_t threadid[NTHREADS]; //pool thread
pthread_mutex_t lock; //mutex lock
List L;

/* Program Utama */

int main(int argc, char *argv[]) {
	addServerLog("Server started");
	//deklarasi variabel
	L.first = NULL;
	bool running = true; //untuk status server running
	int serv_sockfd, new_sockfd; //socket descriptor lama dan baru untuk server
	struct addrinfo flags; //parameter yang digunakan untuk melakukan listen socket
	struct addrinfo *host_info; //resultset yang diset oleh getaddrinfo()
	pthread_attr_t attr; //atribut thread
	int i; //iterator thread
	//deklarasi struktur
	socklen_t addr_size; //ukuran cleint address
	struct sockaddr_storage client; //penyimpan socket address
	//membaca port
	if (argc < 2) { //apabila tidak menuliskan port saat run
		fprintf(stderr,"Error: no port provided\n");
		exit(-1);
	}
	//mengosongkan memori flags
	memset(&flags, 0, sizeof(flags));
	//pengisian struktur dari flags
	flags.ai_family = AF_UNSPEC; //IPv4 atau IPv6
	flags.ai_socktype = SOCK_STREAM; //TCP, sesuai spek
	flags.ai_flags = AI_PASSIVE; //set address
	//mengambil ip address host
	if (getaddrinfo(NULL, argv[1], &flags, &host_info) < 0) { //apabila gagal
		perror("Couldn't read host info for socket start");
		exit(-1);
	}
	//pembuatan socket server
	serv_sockfd = socket(host_info->ai_family, host_info->ai_socktype, host_info->ai_protocol);
	if (serv_sockfd < 0) { //apabila gagal
		perror("Error opening socket");
		exit(-1);
	}
	//pembuatan bind terhadap host
	if (bind(serv_sockfd, host_info->ai_addr, host_info->ai_addrlen) < 0) { //apabila gagal
		perror("Error on binding");
		exit(-1);
	}
	//membebaskan memori host_info
	freeaddrinfo(host_info);
	//pengisian atribut attr
	pthread_attr_init(&attr); //inisiasi atribut thread
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO); //scheduling diset agar FIFO
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //agar thread tidak saling menunggu (starving)
	//listen server dengan queue size tertentu apabila suatu request sedang ditangani
	listen(serv_sockfd, QUEUE_SIZE);
	//penanganan besarnya size dari socket address client
	addr_size = sizeof(client);
	i = 0; //inisiasi iterator
	//implementasi thread
	while (running) {
		if (i == NTHREADS) { //apabila thread sudah maksimum, reset ke nilai awal
		  i = 0;
		}
		//menerima koneksi dari socket descriptor server ke client dengan address size socket address client
		new_sockfd = accept(serv_sockfd, (struct sockaddr *) &client, &addr_size);
		if (new_sockfd < 0) { //apabila gagal
			perror("Error on accept");
			exit(-1);
		}
		//membuat thread baru
		addClientToList(&L,new_sockfd);
		pthread_create(&threadid[i++], &attr, &threadworker, (void *) new_sockfd);

	}
	printf("Server closing");
	addServerLog("Server closed");
	return 0;
}

void writeUsername(char *user, char *pass) {
	FILE *f = fopen("assets/users.txt","a"); //membuka file dengan tipe "append"
	//remove newline
	user = removeNewline(user);
	pass = removeNewline(pass);
	if (f) { //apabila tidak gagal
		printf("success opening file\n");
		fprintf(f,"%s\t", user);
		fprintf(f,"%s\n", pass);
		fclose(f);
	}
}

void *threadworker(void *arg) {
	int sockfd, rw; //file deskriptor dan penampung return value read/write
	int status; //status client, apabila client menuliskan "exit" berarti keluar
	char *buffer, *response; //buffer untuk pesan dan response server
	sockfd = (int) arg; //mengambil sock file descriptor dari argumen yang dipassing
	//alokasi memori
	buffer = malloc(BUFFER_SIZE);
	response = malloc(BUFFER_SIZE);
	do {
  		bzero(buffer, BUFFER_SIZE); //mengosongkan memori buffer
  		bzero(response, BUFFER_SIZE); //mengosongkan memori response
  		/* LABEL READ 1 */
  		printf("read threadworker\n");
		rw = read(sockfd, buffer, BUFFER_SIZE); //melakukan pembacaan terhadap socket
		strcpy(response,buffer);
		if (rw < 0)  { //apabila pembacaan gagal
		    perror("Error reading form socket, exiting thread");
		    pthread_exit(0);
		}
		printf("New message received: %s", buffer); //apabila ada buffer message yang masuk
		//menuliskan kembali ke client
		bzero(buffer, BUFFER_SIZE); //mengosongkan memori buffer
		sprintf(buffer, "Acknowledgement from TID:0x%x\n", pthread_self()); //menerima ACK
		/* LABEL WRITE 1 */
		printf("Reply ke client: %s\n", buffer);
		rw = write(sockfd, buffer, strlen(buffer)); //menuliskan pesan
		if (rw < 0)  { //apabila penulisan gagal
		    perror("Error writing to socket, exiting thread");
		    pthread_exit(0);
		}
		//melakukan aksi berdasarkan pesan yang diberikan
		doActions(sockfd, response);
		//critical section
		//melakukan lock mutex
		pthread_mutex_lock (&lock);
		//melakukan unlock mutex setelah selesai melakukan operasi
		pthread_mutex_unlock (&lock);
		} while (checkExitMsg(response) == 0);
	removeClientFromList(&L,sockfd);
	close(sockfd); //menutup socket untuk client
	//dealokasi
	free(buffer);
	free(response);
	//menutup thread
	printf("TID:0x%x served request, exiting thread\n", pthread_self());
	pthread_exit(0);
}

bool checkUsername(char *input) {
	char output[255]; //jumlah yang mungkin didapat dalam satu line di file .txt
	input = removeNewline(input);
	int ret = false, i, stat = 1; //return, iterator, status looping
	FILE *f = fopen("assets/users.txt","r"); //buka file dalam bentuk "membaca"
	if (f) { //apabila tidak gagal
		while (fgets(output,255,f) != NULL && !ret) {
			i = 0; //membaca dari karakter index ke-0
			while (input[i] != NULL && output[i] != '\t' && stat == 1) { //bukan akhir tab username
				if (input[i] != output[i]) { //kalau tidak sama, langsung keluar
					stat = 0;
				}
				else { //kalau sama, lanjut
					i++;
					if (input[i] == NULL && output[i] == '\t') { //kalau lanjut sampai akhir
						ret = true;
						stat = 0;
					}
				}
			}
			stat = 1; //pengisian ulang stat dengan 1 agar dapat masuk ke loop
		}
		fclose(f);
	}
	return ret;
}

bool authenticate(char *user, char *pass) {
	char output[255]; //jumlah yang mungkin didapat dalam satu line di file .txt
	user = removeNewline(user);
	pass = removeNewline(pass);
	int ret = false, i, stat = 1, j = 0; //return, iterator, status looping, dan index password
	FILE *f = fopen("assets/users.txt","r"); //buka file dalam bentuk "membaca"
	if (f) { //apabila tidak gagal
		while (fgets(output,255,f) != NULL && !ret) {
			i = 0; //membaca dari karakter index ke-0
			while (user[i] != NULL && output[i] != '\t' && stat == 1) { //bukan akhir tab username
				if (user[i] != output[i]) { //kalau tidak sama, langsung keluar
					stat = 0;
				}
				else { //kalau sama, lanjut
					i++; //tambah 1 indeks
					if (user[i] == NULL && output[i] == '\t') { //kalau lanjut sampai akhir
						j = 0;
						i++;
						//cek password, kini i sudah berada di index password, melewati \t
						while (pass[j] != NULL && output[i] != '\n' && stat == 1) { //bukan end of line
							if (pass[j] != output[i]) { //kalau tidak sama, langsung keluar
								stat = 0;
							}
							else { //kalau sama
								j++; //tambah 1 indeks
								i++; //tambah 1 indeks juga
								if (pass[j] == NULL && output[i] == '\n') { //kalau lanjut sampai akhir
									ret = true;
								}
							}
						}
					}
				}
			}
			stat = 1; //pengisian ulang stat dengan 1 agar dapat masuk ke loop
		}
		fclose(f);
	}
	return ret;
}

void doActions(int sockfd, char *msg) {
	/* PREKONDISI: READ 1 WRITE 1 */
	if (strcmp(msg,"signup\n") == 0) { //message = signup
		signup(sockfd);
	} else if (strcmp(msg,"login\n") == 0) { //message = login
		login(&L,sockfd);
	} else if (strcmp(msg,"logout\n") == 0) { //message = logout
		logout(&L,sockfd);
	} else if (isMessage(msg)){ //message = message
		sendMessage(&L,sockfd,msg);
	} else if (isCreateGroup(msg)) {
		char grp[100] = "";
		strncpy(grp,msg+7,strlen(msg)-7);
		newGroup(grp);
	} else if (isJoinGroup(msg)) {
		char usr[25] = "";
		char grp[100] = "";
		strncpy(grp,msg+5,strlen(msg)-5);
		clientList *iter = (L).first;
		bool found = false;
		while (iter != NULL && !found) {
			if (iter->clientSocket == sockfd) {
				strcpy(usr,iter->username);
				found = true;
			}
			else {
				iter = iter->next;
			}
		}
		addUserToGroup(grp,usr);
	} else if (isLeaveGroup(msg)) {
		char usr[25] = "";
		char grp[100] = "";
		strncpy(grp,msg+6,strlen(msg)-6);
		clientList *iter = (L).first;
		bool found = false;
		while (iter != NULL && !found) {
			if (iter->clientSocket == sockfd) {
				strcpy(usr,iter->username);
				found = true;
			}
			else {
				iter = iter->next;
			}
		}
		delUser(grp,usr);
	}
}

void addClientToList(List *L, int sock) {
	clientList *C;
	C = malloc(sizeof(clientList));
	C->clientSocket = sock;
	C->next = NULL;
	if (isEmpty(L)) {
		(*L).first = C;
	}
	else {
		clientList *iter = (*L).first;
		while (iter->next != NULL) {
			iter = iter->next;
		}
		iter->next = C;
	}
	clientList *iter2 = (*L).first;
	printf("A client has been connected to this server with socket ID %d\n", sock);
	printf("Connected clients (with socket IDs): ");
	while (iter2 != NULL) {
		printf("%d ",iter2->clientSocket);
		iter2 = iter2->next;
	}
	printf("\n");
}

void removeClientFromList(List *L, int sock) {
	clientList *iter = (*L).first;
	clientList *prev = NULL;
	while (iter != NULL) { //penelusuran ke list
		if (iter->clientSocket == sock) {
			if (iter == (*L).first) { //apabila di head
				(*L).first = iter->next;
			}
			else { //bukan di head
				prev->next = iter->next;
			}
			iter = NULL;
			free(iter);
		}
		else {
			prev = iter;
			iter = iter->next;
		}
	}
	clientList *iter2 = (*L).first;
	printf("Client with socket number %d has been disconnected.\n", sock);
	printf("Remaining clients (with socket IDs): ");
	while (iter2 != NULL) {
		printf("%d ",iter2->clientSocket);
		iter2 = iter2->next;
	}
	printf("\n");
}

bool isEmpty(List *L) {
	return ((*L).first == NULL);
}

void signup(int sockfd) {
	int rw;
	char *buffer, *nama, *pass;
	//alokasi
	buffer = malloc(BUFFER_SIZE);
	nama = malloc(BUFFER_SIZE);
	pass = malloc(BUFFER_SIZE);
	//mengosongkan buffer
	bzero(buffer, BUFFER_SIZE);
	bzero(nama, BUFFER_SIZE);
	bzero(pass, BUFFER_SIZE);
	//membaca inputan nama [1]
	/* LABEL READ 2 */
	rw = read(sockfd, buffer, BUFFER_SIZE);
	if (rw < 0) {
		perror("Error membaca input nama\n");
		exit(-1);
	}
	strcpy(nama,buffer);
	printf("Nama: %s", nama);
	bzero(buffer, BUFFER_SIZE);
	//membaca inputan password [2]
	/* LABEL READ 3 */
	rw = read(sockfd, buffer, BUFFER_SIZE);
	if (rw < 0) {
		perror("Error membaca input password\n");
		exit(-1);
	}
	strcpy(pass,buffer);
	printf("Pass: %s", pass);
	bzero(buffer, BUFFER_SIZE);
	if (checkUsername(nama)) { //apabila nama sudah ada, signup gagal
		//menuliskan error ke client
		sprintf(buffer, "Username sudah ada di database!\n");
	}
	else { //nama belum ada, signup berhasil
		//menuliskan ke database
		writeUsername(nama,pass);
		/* Buat file pending message */
		char path2[50] = "assets/server/pending_messages/";
		strncat(path2,nama,strlen(nama));
		strncat(path2,".txt",4);
		FILE *f = fopen(path2,"w");
		fclose(f);
		/* Selesai membuat */
		//menuliskan ke client [3]
		sprintf(buffer, "Username berhasil dibuat!\n");
	}
	/* LABEL WRITE 2 */
	rw = write(sockfd, buffer, strlen(buffer));
	if (rw < 0) {
		perror("Gagal menuliskan ACK signup\n");
		exit(-1);
	}
	free(buffer);
	free(nama);
	free(pass);
}

void login(List *L, int sockfd) {
	int rw;
	char *buffer, *nama, *pass;
	//alokasi
	buffer = malloc(BUFFER_SIZE);
	nama = malloc(BUFFER_SIZE);
	pass = malloc(BUFFER_SIZE);
	//mengosongkan buffer
	bzero(buffer, BUFFER_SIZE);
	bzero(nama, BUFFER_SIZE);
	bzero(pass, BUFFER_SIZE);
	//membaca inputan nama [1]
	/* LABEL READ 2 */
	rw = read(sockfd, buffer, BUFFER_SIZE);
	if (rw < 0) {
		perror("Error membaca input nama\n");
		exit(-1);
	}
	strcpy(nama,buffer);
	printf("Nama: %s", nama);
	bzero(buffer, BUFFER_SIZE);
	//membaca inputan password [2]
	/* LABEL READ 3 */
	rw = read(sockfd, buffer, BUFFER_SIZE);
	if (rw < 0) {
		perror("Error membaca input password\n");
		exit(-1);
	}
	strcpy(pass,buffer);
	printf("Pass: %s", pass);
	bzero(buffer, BUFFER_SIZE);
	//menuliskan ke client [3]
	if (authenticate(nama,pass)) {
		nama = removeNewline(nama);
		sprintf(buffer, "Sukses login!\n");
		/* LABEL WRITE 2 */
		rw = write(sockfd, buffer, strlen(buffer));
		addUsernameToList(L, sockfd, nama);
		sleep(1);
		retrievePendingMessage(nama,sockfd);
		char slog[50] = "";
		strcpy(slog,nama);
		strncat(slog," logged in",11);
		addServerLog(slog);
	}
	else {
		sprintf(buffer, "Gagal login!\n");	
		/* LABEL WRITE 2 */
		rw = write(sockfd, buffer, strlen(buffer));
	}
	if (rw < 0) {
		perror("Gagal menuliskan ACK login\n");
		exit(-1);
	}
	/* Proses menulis message pending dari server ke client */
	free(buffer);
	free(nama);
	free(pass);
}

void logout(List *L, int sockfd) {
	clientList *iter = (*L).first;
	char uname[25];
	bool found = false;
	while (iter != NULL && !found) {
		if (iter->clientSocket == sockfd) {
			strcpy(uname,iter->username);
			found = true;
			bzero(iter->username,25);
		}
		else {
			iter = iter->next;
		}
	}
	if (found) {
		printf("User %s logged out successfully from client with socket ID %d\n", uname, sockfd);
		char slog[50] = "";
		strcpy(slog, uname);
		strncat(slog," logged out",12);
		addServerLog(slog);
	}
}

void addUsernameToList(List *L, int sockfd, char *user) {
	clientList *iter = (*L).first;
	bool found = false;
	printf("First client is: socket ID %d\n", iter->clientSocket);
	while (iter != NULL && !found) {
		printf("Socket id: Server %d vs Client %d\n", sockfd, iter->clientSocket);
		if (iter->clientSocket == sockfd) {
			found = true;
			strcpy(iter->username,user);
			printf("Success adding %s to client with socket ID %d\n", iter->username, sockfd);
			char slog[50] = "";
			strcpy(slog, iter->username);
			strncat(slog," logged out",12);
			addServerLog(slog);
		}
		else {
			iter = iter->next;
		}
	}
}

void addPendingMessage(char* src_client, char* dest_client, char* msg) {
	// misal <src_client> ngesend chat isiMessage ke <dest_client> yang offline
	// isiMessage disimpan di assets/server/pending_messages/<dest_client>.txt
	// isi txtnya: Source: <src_client> \n Message: <msg>
	char pathSender[100] = "assets/client/chat_log/";
	strncat(pathSender,src_client,strlen(src_client));
	strncat(pathSender,"/",1);
	strncat(pathSender,dest_client,strlen(dest_client));
	strncat(pathSender,".txt",4);
	FILE *FSender = fopen(pathSender,"a");
	if(FSender){
		fputs(msg,FSender);
	}
	fclose(FSender);

	char path[100] = "assets/server/pending_messages/";
	strncat(path,dest_client,strlen(dest_client));
	strncat(path,".txt",4);
	FILE *FServer = fopen(path,"a");
	if(FServer){
		char source[100] = "Source: ";
		strncat(source,src_client,strlen(src_client));
		fputs(source,FServer);
		char msg_[200] = "\n";
		strncat(msg_,msg,strlen(msg));
		fputs(msg_,FServer);
	}
	fclose(FServer);
}

void retrievePendingMessage(char *dest_client, int sockfd) {
	/* PREKONDISI: WRITE 2 READ 3 */
	char path[100] = "assets/server/pending_messages/";
	strncat(path,dest_client,strlen(dest_client));
	strncat(path,".txt",4);
	FILE *FServer = fopen(path,"r+");
	char *src_client;
	src_client = malloc(25);
	bzero(src_client,25);
	int src_clientLength;
	int sourceLength;
	/* Pengiriman ke client ybs */
	int rw;
	char *buffer;
	buffer = malloc(BUFFER_SIZE);
	bzero(buffer,BUFFER_SIZE);
	if(FServer){
		char line[256];
		if (fgets(line, sizeof(line), FServer) != NULL) { //file ada isinya
			sprintf(buffer,"Ada isinya");
			/* LABEL WRITE 3 */
			rw = write(sockfd, buffer, BUFFER_SIZE);
			//pointer ke awal file
			rewind(FServer);
			while (fgets(line, sizeof(line), FServer)){
				if(strstr(line,"Source: ") != NULL){
					//ngambil src_client dari Source: <src_client>
					sourceLength = strlen(line);
					int src_clientLength = sourceLength - 7;
					strncpy(src_client,line+8,src_clientLength);
				}
				else { //bukan line source, kini string line berisi pesan
					src_client = removeNewline(src_client);
					//mengirim src client
					bzero(buffer,BUFFER_SIZE);
					sprintf(buffer,src_client);
					printf("Source client: %s\n", buffer);
					/* LABEL WRITE 4 */
					rw = write(sockfd, buffer, BUFFER_SIZE);
					//mengirim dest client
					bzero(buffer,BUFFER_SIZE);
					sprintf(buffer,dest_client);
					printf("Dest client: %s\n", buffer);
					/* LABEL WRITE 5 */
					rw = write(sockfd, buffer, BUFFER_SIZE);
					//mengirimkan pesan ke client yang dituju
					bzero(buffer,BUFFER_SIZE);
					sprintf(buffer,line);
					printf("Pesan: %s\n", buffer);
					/* LABEL WRITE 6 */
					rw = write(sockfd, buffer, BUFFER_SIZE);
					strcpy(src_client,"");
				}
			}
			bzero(buffer,BUFFER_SIZE);
			sprintf(buffer,"Sudah habis");
			/* LABEL WRITE 7 */
			rw = write(sockfd, buffer, BUFFER_SIZE);
		}
		else { //tidak ada isinya
			bzero(buffer,BUFFER_SIZE);
			sprintf(buffer,"Tidak ada isinya");
			/* LABEL WRITE 3 */
			rw = write(sockfd, buffer, BUFFER_SIZE);
		}
	}
	//menghapus .txt di server karena sudah sampai di log user
	fclose(FServer);
	FILE *FServer2 = fopen(path,"w");
	if(FServer2){
		fputs("",FServer2);
	}
	fclose(FServer2);
}

void addChatToUserLog(char* src_client, char* dest_client, char* msg) {
	char pathSrc[100] = "assets/client/chat_log/";
	strncat(pathSrc,dest_client,strlen(dest_client));
	strncat(pathSrc,"/",1);
	strncat(pathSrc,src_client,strlen(src_client));
	strncat(pathSrc,".txt",4);

	char pathDest[100] = "assets/client/chat_log/";
	strncat(pathDest,src_client,strlen(src_client));
	strncat(pathDest,"/",1);
	strncat(pathDest,dest_client,strlen(dest_client));
	strncat(pathDest,".txt",4);
	FILE *fSrc = fopen(pathSrc,"a");
	FILE *fDest = fopen(pathDest,"a");
	//remove newline
	msg = removeNewline(msg);
	if(fDest){ // apabila tidak gagal
		if(fSrc){
			printf("success opening file\n");
			fprintf(fSrc,"%s\n", msg);
			fprintf(fDest,"%s\n", msg);
			fclose(fSrc);
			fclose(fDest);
		}
	}
}

int userSocketInClientList(List *L, char *user){
	clientList *iter = (*L).first;
	int online = -1;
	while ((iter != NULL) && (online == -1)) {
		if(strcmp(user,iter->username) == 0){
			online = iter->clientSocket;
		}
		else {
			iter = iter->next;
		}
	}
	return online;
}

void sendMessage(List *L, int sockfd, char *message){
	int rw;
	int msgLength = strlen(message);
	int userLength = msgLength - 7;
	char *user = (char*) malloc(userLength);
	strncpy(user,message+8,userLength);
	printf("user yang dikirim message: %s", user);
	/* Proses validasi */
	if(isUserExistDB(user)){
		char *buffer;
		//alokasi
		buffer = malloc(BUFFER_SIZE);
		//mengosongkan buffer
		bzero(buffer, BUFFER_SIZE);
		sprintf(buffer, "User ada di database\n");
		/* LABEL WRITE 2 */
		rw = write(sockfd,buffer,BUFFER_SIZE);
		if (rw < 0) {
			printf("Gagal menulis user ada ke client\n");
		}
		/* Setelah proses validasi */
		char *isiMessage;
		//alokasi
		buffer = malloc(BUFFER_SIZE);
		isiMessage = malloc(BUFFER_SIZE);
		//mengosongkan buffer
		bzero(buffer, BUFFER_SIZE);
		bzero(isiMessage, BUFFER_SIZE);
		//membaca inputan isiMessage
		/* LABEL READ 2 */
		rw = read(sockfd, buffer, BUFFER_SIZE);
		printf("Message: %s", message);
		printf("isiMessage: %s", buffer);
		if (rw < 0) {
			perror("Error membaca input isi message\n");
			exit(-1);
		}
		// masukin user sender ke isi messagenya
		clientList *iter = (*L).first;
		char sender[25] = "";
		while ((iter != NULL) && (strlen(sender)==0)){
			if(sockfd==iter->clientSocket){
				strcpy(sender,iter->username);
			}
			else {
				iter = iter->next;
			}
		}
		char sender_[25] = "";
		strcpy(sender_,sender);
		strcat(sender, ": ");
		// masukin waktu ke isi messagenya
		time_t rawtime;
		struct tm *info;
		char date[80];
		time( &rawtime );
		info = localtime( &rawtime );
		strftime(date,80,"[%x - %I:%M%p] ", info);
		strcat(date,sender);
		strcat(date,buffer);
		strcpy(buffer,date);
		strcpy(isiMessage,buffer);
		printf("Message: %s", isiMessage);
		//mengirim pesan
		int sockToWho = userSocketInClientList(L,user);
		if(sockToWho != -1){ // user nya online
			printf("User online\n");
			// online, kirim isiMessage ke user:
			// isiMessage ditaruh ke log, terus buffernya diisi sama string "New message from usernya siapa"
			// naruh isi Message di lognya sender sama user:
			addChatToUserLog(sender_,user,isiMessage);
			char newMsg[100] = "New messsage from ";
			strcat(newMsg,sender_);
			strcpy(buffer,newMsg);
			/* LABEL WRITE 3 */
			rw = write(sockToWho, buffer, strlen(buffer));
			if (rw < 0) {
				char *buffer;
				//alokasi
				buffer = malloc(BUFFER_SIZE);
				//mengosongkan 
				bzero(buffer, BUFFER_SIZE);
				perror("Gagal mengirim pesan\n");
				exit(-1);
			}
			free(buffer);
			free(isiMessage);
		}
		else{ // usernya offline
			// misal userA ngesend chat isiMessage ke userB yang offline
			// isiMessage disimpan di assets/server/pending_messages
			printf("User offline\n");
			addPendingMessage(sender_,user,isiMessage);
		}
	}
	else{
		char *buffer;
		//alokasi
		buffer = malloc(BUFFER_SIZE);
		//mengosongkan buffer
		bzero(buffer, BUFFER_SIZE);
		sprintf(buffer, "User tidak ada di database\n");
		/* LABEL WRITE 2 */
		rw = write(sockfd,buffer,BUFFER_SIZE);
		if (rw < 0) {
			printf("Gagal menulis user tidak ada ke client\n");
		}
	}
}

void showMessage(List *L, int sockfd, char *message){
	printf("ada yang melihat message\n");
}

void addServerLog(char *log) {
	char path2[50] = "assets/server/server_log.txt";
	FILE *f = fopen(path2,"a");
	if (f) {
		time_t rawtime;
		struct tm *info;
		char date[80];
		time( &rawtime );
		info = localtime( &rawtime );
		strftime(date,80,"[%x - %I:%M%p] ", info);
		fprintf(f,"%s: %s\n", date, log);
	}
	fclose(f);
}

/* GROUP */

void newGroup(char* group) {
	char path[100] = "assets/server/groups/";
	//remove newline
	group = removeNewline(group);
	strncat(path,group,strlen(group));
	strncat(path,"/",1);
	if (stat(path,&st) == -1) { //apabila path belum ada, bikin baru
		mkdir(path,0777);
		printf("Successfully created group log folder %s\n", path);
	}
	char path2[100] = "";
	strcpy(path2,path);
	strncat(path,"participants.txt",16);
	strncat(path2,"chat.txt",8);
	FILE *f1 = fopen(path,"a");
	FILE *f2 = fopen(path2,"a");
	if (f1 && f2) {
		printf("Sukses membuat participants dan chat teks\n");
	}
	fclose(f1);
	fclose(f2);
}


bool isUserInGroup(char* group, char* user){
	//remove newline
	group = removeNewline(group);
	user = removeNewline(user);
	int ret = false;
	char path[100] = "assets/server/groups/";
	strncat(path,group,strlen(group));
	strncat(path,"/",1);
	strncat(path,"participants.txt",16);
	FILE *f = fopen(path,"r"); //buka file dalam bentuk "membaca"
	char line[256];
	while ((fgets(line, sizeof(line), f)) && !ret) {
		if(strcmp(line,user) == 0){
			ret = true;
		}
	}
	fclose(f);
	return ret;
}

void addUserToGroup(char* group, char* user){
	//remove newline
	group = removeNewline(group);
	user = removeNewline(user);
	if(!isUserInGroup(group,user)){
		char path[100] = "assets/server/groups/";
		strncat(path,group,strlen(group));
		strncat(path,"/",1);
		strncat(path,"participants.txt",16);
		FILE *f = fopen(path,"a");
		if(f){
			fprintf(f,"%s\n",user);
		}
		fclose(f);
	}
	else{
		printf("User sudah berada di dalam group ini. Tidak ditambahkan\n");
	}
}

void delUser(char* group, char* user){
	//remove newline
	group = removeNewline(group);
	user = removeNewline(user);
	if(isUserInGroup(group,user)){
		char path[100] = "assets/server/groups/";
		strncat(path,group,strlen(group));
		strncat(path,"/",1);
		strncat(path,"participants.txt",16);
		//source file
		FILE *source = fopen(path,"r");
		//dummy file
		FILE *dummy = fopen("assets/server/dummy.txt","w"); 
	   	char line[256];
		while (fgets(line, sizeof(line), source)){
			strcat(user,"\n");
			if(strcmp(line,user) != 0){
				fputs(line,dummy);
			}
		}
		//close
		fclose(dummy);
		fclose(source);
		//open dengan write dan read
		source = fopen(path,"w");
		dummy = fopen("assets/server/dummy.txt","r");
		char line2[256];
		while (fgets(line2, sizeof(line2), dummy)){
			fputs(line2,source);
		}
		//close
		fclose(source);
		fclose(dummy);
		//reset dummy
		FILE *dummy2 = fopen("assets/server/dummy.txt","w");
		if(dummy2){
			fputs("",dummy);
		}		
		fclose(dummy2);
	}
}

bool isCreateGroup(char *message){
	char msg[] = "create ";
	bool same = true;
	int i = 0;
	while(same && (i <= 6)){
		if(message[i]==msg[i]){
			i++;
		}
		else{
			same = false;
		}
	}
	return same;
}

bool isJoinGroup(char *message){
	char msg[] = "join ";
	bool same = true;
	int i = 0;
	while(same && (i <= 4)){
		if(message[i]==msg[i]){
			i++;
		}
		else{
			same = false;
		}
	}
	return same;
}

bool isLeaveGroup(char *message){
	char msg[] = "leave ";
	bool same = true;
	int i = 0;
	while(same && (i <= 5)){
		if(message[i]==msg[i]){
			i++;
		}
		else{
			same = false;
		}
	}
	return same;
}