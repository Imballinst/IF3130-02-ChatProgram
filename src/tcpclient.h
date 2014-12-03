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

/* Header fungsi dan prosedur */

void handleActions(int sockfd, char *prevmsg);
/* Melakukan handle aksi berdasarkan pesan yang dikirimkan sebelumnya. Contoh: sebelumnya mengirimkan pesan signup, maka akan dilakukan penanganan
 * untuk signup tersebut.
 * Param: integer socket dan string pesan sebelumnya.
 */

#endif