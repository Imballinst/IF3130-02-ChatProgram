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
#include <time.h>
#include "adtfungsiprosedur.h"

/* Pre-prosesor */

#define NTHREADS 5 //thread maksimum
#define QUEUE_SIZE 5 //maksimum client yang menunggu

/* Tipe bentukan */

//pointer untuk setiap client
typedef struct clientList {
	int clientSocket;
	char username[25];
	struct clientList *next;
} clientList;

typedef struct {
	struct clientList *first;
} List;

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
bool checkUsername(char *input);
/* Mengecek apakah username sudah ada di database (file eksternal) atau belum. Dipakai ketika signup.
 * Param: string username. Return: true apabila sudah ada, false apabila belum ada.
 */
bool authenticate(char *user, char *pass);
/* Mengecek apakah password matching dengan database atau belum.
 * Param: string username, string password. Return: true apabila benar, false apabila salah.
 */
void doActions(int sockfd, char *msg);
 /* Melakukan aksi berdasarkan pesan yang diterima
  * Param: integer socket dan string pesan.
  */
void addClientToList(List *L, int sock);
/* Menambahkan socket yang dimiliki client ke dalam list. Digunakan setelah terkoneksi.
 * Param: integer socket client.
 */
void removeClientFromList(List *L, int sock);
/* Menghapus client dari list dengan mencari dari nilai socketnya. Digunakan setelah diskoneksi.
 * Param: integer socket client.
 */
bool isEmpty(List *L);
/* Mengecek apakah List kosong atau tidak dengan melihat firstnya.
 * Param: List L.
 */
void signup(int sockfd);
/* Melakukan signup ke dalam database.
 * Param: integer socket.
 */
void login(List *L, int sockfd);
/* Melakukan login ke server dengan username dan password tertentu.
 * Param: List L, string username, string password.
 */
void logout(List *L, int sockfd);
/* Melakukan logout dari server.
 * Param: List L, string username.
 */
void addUsernameToList(List *L, int sockfd, char *user);
/* Melakukan penambahan string username ke dalam list clients. Digunakan setelah login.
 * Param: List L, integer socket, string user.
 */
void addPendingMessage(char* src_client, char* dest_client, char* msg);
/* Menambahkan pesan yang pending ke dalam direktori assets/server/pending_messages/<dest_client>.txt.
 * Isi dari file tersebut adalah <src_client> dan <msg>. Digunakan saat client yang dituju sedang offline.
 * Param: username yang mengirim, username yang dituju, string pesan.
 */
void retrievePendingMessage(char *dest_client, int sockfd);
/* Mengolah pesan yang pending dari direktori assets/server/pending_messages/<dest_client>.txt ke user <dest_client> yang baru saja online.
 * Digunakan saat client baru saja online, maka server akan mencari text file dengan nama client tersebut.
 * Param: username yang mengirim, username yang dituju, string pesan.
 */
void addChatToUserLog(char* src_client, char* dest_client, char* msg);
/* Menambahkan chat yang terjadi antara dua buah client ke dalam assets/client/chat_log/<dest_client>/<src_client>.txt DAN assets/client/chat_log/<src_client>/<dest_client>.txt
 * Tujuan dibuat dua buah seperti itu agar user source dan destination dapat mengakses chat tersebut, apabila cuma salah satu, user yang satu lagi tidak akan dapat mengakses chat log.
 * Digunakan saat client A baru selesai menulis ke buffer write DAN client B saat baru selesai menulis ke buffer read.
 * Param: username yang mengirim, username yang dituju, string pesan.
 */
int userSocketInClientList(List *L, char *user);
/* Melakukan cek apakah user sedang ada di dalam List atau tidak
 * Param: List L dan string username
 */
void sendMessage(List *L, int sockfd, char *message);
/* Mengirimkan pesan ke user yang memiliki socket tertentu yang online maupun offline/
 * Param: List L, integer socket, string pesan
 */
void showMessage(List *L, int sockfd, char *message);
/* Show message di layar dari orang tertentu
 * Param: List L, integer socket, string pesan
 */
void addServerLog(char *log);
/* Menambahkan log ke server log, apabila belum ada folder/filenya, buat baru.
 * Param: string log.
 */

#endif