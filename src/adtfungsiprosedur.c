/* File     : adtfungsiprosedur.c
 * Author   : Try Ajitiono
 */

/* Header file */

#include "adtfungsiprosedur.h"

 /* Realisasi fungsi dan prosedur */

int checkExitMsg(char *msg) {
	int ret = 0; //ret bernilai 1 apabila pesan bernilai "exit"
	printf("checkExitMsg: %s", msg);
	if (msg[0] == 'e') {
		if (msg[1] == 'x') {
			if (msg[2] == 'i') {
				if (msg[3] == 't') {
					if (msg[4] == '\n') {
						ret = 1;
					}
				}
			}
		}
	}
	return ret;
}