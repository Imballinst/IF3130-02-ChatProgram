/* File     : adtfungsiprosedur.h
 * Author   : Try Ajitiono
 * Note		: ADT ini hanya digunakan HANYA untuk fungsi-fungsi/prosedur-prosedur yang dapat dipakai oleh server ATAU client. Fungsi atau prosedur yang hanya digunakan pada
 			  salah satu entitas, silahkan dimasukkan ke file yang bersangkutan.
 */

/* Pre-prosesor */

#ifndef ADTFUNGSIPROSEDUR_H
#define ADTFUNGSIPROSEDUR_H

 #define BUFFER_SIZE 256 //maksimum buffer message [0..255]
 #include <string.h>
 #include <errno.h>
 #include <stdbool.h>

/* Header fungsi dan prosedur */

int checkExitMsg(char *msg);
/* Melakukan cek apakah pesan yang dikirim merupakan pesan keluar dari aplikasi (hanya untuk client)
 * Param: string pesan. Return: integer (1) apabila pesan yang dikirim adalah "exit", (0) apabila bukan
 */

char* removeNewline(char *msg);
/* Menghilangkan newline dari akhir pesan
 * Param: string pesan. Return: pesan tanpa '\n'.
 */

bool isMessage(char *message);

bool isShowMessage(char *message);

#endif