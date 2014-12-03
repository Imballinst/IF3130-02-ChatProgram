/* File     : tcpserver.h
 * Author   : Try Ajitiono
 */

/* Pre-prosesor */

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

/* Pre-prosesor */

#define NTHREADS 5 //thread maksimum
#define QUEUE_SIZE 5 //maksimum client yang menunggu

/* Tipe bentukan */

//pointer untuk setiap client
typedef struct clientList {
	int clientSocket;
	struct clientList *prev;
	struct clientList *next;
} clientList;

/* Header fungsi dan prosedur */

void writeUsername(char *user, char *pass);
/* Menuliskan username dan password ke dalam file eksternal.
 * User diminta memasukkan username yang belum ada di file eksternal, apabila sudah ada, akan diminta pengulangan.
 * Setelah melewati proses validasi username, pengguna akan diminta untuk menginput password, lalu akan masuk ke file eksternal.
 * Param: string username, string password.
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
void addClientToList(clientList* cList, int sock);
/* Menambahkan socket yang dimiliki client ke dalam list. Digunakan setelah login.
 * Param: integer socket client.
 */
void removeClientFromList(clientList* cList, int sock);
/* Menghapus client dari list dengan mencari dari nilai socketnya. Digunakan setelah logout.
 * Param: integer socket client.
 */

#endif