/* File     : tcpserver.c
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
#include <stdbool.h>
#include "adtfungsiprosedur.h"

/* Pre-prosesor */

#define NTHREADS 50 //thread maksimum
#define QUEUE_SIZE 5 //maksimum client yang menunggu

/* Variabel-variabel global untuk mutex */

pthread_t threadid[NTHREADS]; //pool thread
pthread_mutex_t lock; //mutex lock
int counter = 0; //counter untuk banyaknya mutex digunakan
//untuk status server running
bool running = true;

/* Header fungsi dan prosedur */

void writeUsername();
/* Menuliskan username dan password ke dalam file eksternal.
 * User diminta memasukkan username yang belum ada di file eksternal, apabila sudah ada, akan diminta pengulangan.
 * Setelah melewati proses validasi username, pengguna akan diminta untuk menginput password, lalu akan masuk ke file eksternal.
 */
void *threadworker(void *arg);
/* Melakukan manajemen thread selama keberjalanan aplikasi
 * Param: argumen.
 * Argumen akan diubah menjadi integer yang merupakan nilai dari suatu socket.
 */
 int checkUsername(char *input);
/* Mengecek apakah username sudah ada di database (file eksternal) atau belum
 * Param: string username. Return: integer (1) apabila sudah ada, (0) apabila belum ada
 */
 void doActions(int sockfd, char *msg);
 /* Melakukan aksi berdasarkan pesan yang diterima
  * Param: integer socket dan string pesan.
  */

/* Program Utama */

int main(int argc, char *argv[]) {
	//deklarasi variabel
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
		pthread_create(&threadid[i++], &attr, &threadworker, (void *) new_sockfd);
		sleep(0); //memberikan thread beberapa waktu untuk melakukan proses di CPU
	}
	printf("Server closing\n");
	return 0;
}

void writeUsername() {
	char user[255], pass[255]; //digunakan untuk login
	FILE *f = fopen("assets/users.txt","a"); //membuka file dengan tipe "append"
	if (f) { //apabila tidak gagal
		do {
			printf("name    : ");
			scanf("%s",user);
		} while (checkUsername(user) == 1); //apabila username sudah ada, diulang terus
		printf("password: ");
		scanf("%s", pass);
		fprintf(f,"%s\t", user);
		fprintf(f,"%s\n", pass);
		fclose(f);
	}
}

int checkUsername(char *input) {
	char output[255]; //jumlah yang mungkin didapat dalam satu line di file .txt
	int ret = 0, i, stat = 1; //return, iterator, status looping
	FILE *f = fopen("assets/users.txt","r"); //buka file dalam bentuk "membaca"
	if (f) { //apabila tidak gagal
		fgets(output,255,f); //skip first line
		while (fgets(output,255,f) != NULL && ret == 0) {
			i = 0; //membaca dari karakter index ke-0
			while (input[i] != '\t' && output[i] != '\t' && stat == 1) { //bukan akhir tab username
				if (input[i] != output[i]) { //kalau tidak sama, langsung keluar
					stat = 0;
				}
				else { //kalau sama, lanjut
					i++;
					if (input[i] == NULL && output[i] == '\t') { //kalau lanjut sampai akhir
						ret = 1;
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
		rw = read(sockfd, buffer, BUFFER_SIZE); //melakukan pembacaan terhadap socket
		strcpy(response,buffer);
		if (rw < 0)  { //apabila pembacaan gagal
		    perror("Error reading form socket, exiting thread");
		    pthread_exit(0);
		}
		printf("New message received: %s", buffer); //apabila ada buffer message yang masuk
		//melakukan aksi berdasarkan pesan yang diberikan
		doActions(sockfd, buffer);
		//menuliskan kembali ke client
		bzero(buffer, BUFFER_SIZE); //mengosongkan memori buffer
		sprintf(buffer, "Acknowledgement from TID:0x%x", pthread_self()); //menerima ACK
		rw = write(sockfd, buffer, strlen(buffer)); //menuliskan pesan
		if (rw < 0)  { //apabila penulisan gagal
		    perror("Error writing to socket, exiting thread");
		    pthread_exit(0);
		}
		/* Critical section */
		//melakukan lock mutex
		printf("Requesting mutex lock...\n");
		pthread_mutex_lock (&lock);
		printf("Current counter value: %d, upping by 1...\n", counter);
		counter++;
		//melakukan unlock mutex setelah selesai melakukan operasi
		pthread_mutex_unlock (&lock);
		printf("Done! Mutex unlocked again, new counter value: %d\n", counter);
		} while (checkExitMsg(response) == 0);
	close(sockfd); //menutup socket untuk client
	//dealokasi
	free(buffer);
	free(response);
	//menutup thread
	printf("TID:0x%x served request, exiting thread\n", pthread_self());
	pthread_exit(0);
}

void doActions(int sockfd, char *msg) {
	int rw;
	char *buffer;
	buffer = malloc(BUFFER_SIZE);
	bzero(buffer, BUFFER_SIZE);
	if (strcmp(msg,"signup") == 0) { //signup, status = 1
		sprintf(buffer, "You really want to signup? (Y/N)\n");
		rw = write(sockfd, buffer, strlen(buffer));
		if (rw < 0) {
			perror("Error di dalam do actions, nulis ke buffer");
			pthread_exit(0);
		}
	} else if (strcmp(msg,"login") == 0) { //login, status = 2

	} else if (strcmp(msg,"logout") == 0) { //logout, status = 3

	}
}