/* File     : adtfungsiprosedur.h
 * Author   : Try Ajitiono
 */

/* Pre-prosesor */

 #define BUFFER_SIZE 256 //maksimum buffer message [0..255]
 #include <string.h>

/* Header fungsi dan prosedur */

int checkExitMsg(char *msg);
/* Melakukan cek apakah pesan yang dikirim merupakan pesan keluar dari aplikasi (hanya untuk client)
 * Param: string pesan. Return: integer (1) apabila pesan yang dikirim adalah "exit", (0) apabila bukan
 */