/* File     : adtfungsiprosedur.c
 * Author   : Try Ajitiono
 */

/* Header file */

#include "adtfungsiprosedur.h"

 /* Realisasi fungsi dan prosedur */

int checkExitMsg(char *msg) {
	int ret = 0; //ret bernilai 1 apabila pesan bernilai "exit"
	printf("checkExitMsg: %s", msg);
	if (strcmp("exit\n",msg) == 0)
		ret = 1;
	return ret;
}