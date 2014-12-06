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

bool isShowMessage(char *message){
	char msg[] = "show ";
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

bool isUserExistDB(char *user){
	printf("huft");
	char output[255]; //jumlah yang mungkin didapat dalam satu line di file .txt
	user = removeNewline(user);
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
						ret = true;
					}
				}
			}
			stat = 1; //pengisian ulang stat dengan 1 agar dapat masuk ke loop
		}
		fclose(f);
	}
	return ret;
}