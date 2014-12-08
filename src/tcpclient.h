/* File     : tcpclient.h
 * Author   : Try Ajitiono
 */

/* Pre-prosesor */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "adtfungsiprosedur.h"

/* Tipe bentukan dan struktur */

 struct stat st = {0}; //untuk masalah pembuatan direktori
 char user[25];
/* Header fungsi dan prosedur */

void handleActions(int sockfd, char *prevmsg);
/* Melakukan handle aksi berdasarkan pesan yang dikirimkan sebelumnya. Contoh: sebelumnya mengirimkan pesan signup, maka akan dilakukan penanganan
 * untuk signup tersebut.
 * Param: integer socket dan string pesan sebelumnya.
 */

void addChatLog(char* src_client, char* dest_client, char* msg);
/* Menambahkan chat yang terjadi antara dua buah client ke dalam assets/client/chat_log/<dest_client>/<src_client>.txt DAN assets/client/chat_log/<src_client>/<dest_client>.txt
 * Tujuan dibuat dua buah seperti itu agar user source dan destination dapat mengakses chat tersebut, apabila cuma salah satu, user yang satu lagi tidak akan dapat mengakses chat log.
 * Digunakan saat client A baru selesai menulis ke buffer write DAN client B saat baru selesai menulis ke buffer read.
 * Param: username yang mengirim, username yang dituju, string pesan.
 */

void createClientLogFolder(char *username);
/* Membuat folder client di direktori /assets/client/chat_log/<username> untuk menampung chat log kedepannya. Digunakan setelah signup.
 * Didalam folder tersebut akan ada file-file .txt yang namanya username-username lain yang pernah melakukan chat dengan client <username>
 * Param: string username.
 */

#endif