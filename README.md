IF3130-02-ChatProgram
=====================

Tubes 2 Jarkom
=====================

Fitur:

+ Compile program
  Pertama-tama, ubah direktori ke root dari IF3130-02-ChatProgram, lalu jalankan step dibawah ini.
  sh ./compile.sh

+ Run program
  Pertama-tama, ubah direktori ke root dari IF3130-02-ChatProgram, lalu jalankan step dibawah ini.
  Server: sh ./runserver.sh
  Client: sh ./runclient.sh
  File dengan ekstensi .sh mirip dengan file .bat yang ada di Windows, alasan penggunaan: biar nggak ribet mengetik ulang
  nama aplikasi, port, dan IP Address berulang-ulang.

+ Protokol client
  Setiap client yang terhubung ke server akan mendapatkan socket yang unik.
  
+ Signup
  Setiap client yang ingin melakukan signup mengetik "signup" pada window client.
  
+ Login
  Setiap client dapat melakukan login dengan mengetik "login" pada window client, lalu mengetik username dan password 
  sebagaimana yang telah terdaftar di users.txt.
  
+ Logout
  Setiap client yang telah login dapat melakukan logout dengan mengetik logout pada window client.
