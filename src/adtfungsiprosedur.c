/* File     : adtfungsiprosedur.c
 * Author   : Try Ajitiono
 */

/* Header file */

#include "adtfungsiprosedur.h"

 /* Realisasi fungsi dan prosedur */

int checkExitMsg(char *msg) {
	int ret = 0; //ret bernilai 1 apabila pesan bernilai "exit"
	if (strcmp("exit\n",msg) == 0)
		ret = 1;
	return ret;
}

char* removeNewline(char *msg) {
	int len = strlen(msg);
	if (msg[len - 1] == '\n')
		msg[len - 1] = '\0'; //change to NULL char
	return msg;
}