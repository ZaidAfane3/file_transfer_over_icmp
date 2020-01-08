#! /bin/bash 

sudo gcc icmp_send.c -w -o Send_File 
sudo gcc icmp_receive.c -w -o Recv_File 

chmod +x Send_File
chmod +x Recv_File

echo "[*] Compiled ... "
