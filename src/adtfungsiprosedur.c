/* File     : adtfungsiprosedur.c
 * Author   : Try Ajitiono
 * Note		: ADT ini hanya digunakan HANYA untuk fungsi-fungsi/prosedur-prosedur yang dapat dipakai oleh server ATAU client. Fungsi atau prosedur yang hanya digunakan pada
 			  salah satu entitas, silahkan dimasukkan ke file yang bersangkutan.
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

bool isMessage(char *message){
	char msg[] = "message ";
	bool same = true;
	int i = 0;
	while(same && (i <= 7)){
		if(message[i]==msg[i]){
			i++;
		}
		else{
			same = false;
		}
	}
	return same;
}