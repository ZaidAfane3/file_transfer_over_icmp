About 
======
Implementation of file transfer over icmp, using the code written by @july5th.
This code was submitted as a project for Network Programming Course (11437) in Princess Sumaya University For Technology first semester year 2019-2020.



Important
=================
- Before installation a modification on unp.h must be done to sepcify the the OS Configuration file path.
- Make Sure that these libraries are included in "unp.h", the library used in Unix Network Programming: The Sockets Networking  API. W. R. Stevens, B. Fenner, A. M. Rudoff. Addison Wesley. 3rd Edition 2004.
    - #include <stdio.h>
    - #include <stdlib.h>
    - #include <string.h>
    - #include <sys/time.h>
    - #include <netinet/ip.h>
    - #include <netinet/ip_icmp.h>
    - #include <unistd.h>  
    - #include <getopt.h>



compile.sh
==============
Bash shell used to compile both sender and receiver files, the '-w' is added to the compile commands 
so that the warnings are ignored.



icmp_send.c (Sender side):
=================================
Sender Code:
- The arguments that must be set when running this code are Destination IP, Source IP, and
the file that we want to send. If the number of parameters are insufficient, the usage function is called 
and prints out all the parameters that must be sent in order to run the code.
- The file that we want to send is searched for and opened, otherwise an error message is displayed.
- Next, the socket is created and the socket options is set so that the kernel is informed not to fill the ip headers
since we will be filling them.
- The send_file() function is then called. This function performs the following:
    - Calculate packet size (using ip header,icmp header, and data size of the file).
    - Allocate memory for the packet using the packet size.
    - Fill the ip headers and icmp headers.
    - The packet is then sent to a socket address structure that was created, given that the data along with the headers do not exceed the buffer size.



icmp_receive.c (Receiver side):
==============================================
Reveiver Code: 
- An empty file created an opened so that we can write the received data to it later.
- A packet size is calculated (using ip header, icmp header, and data size of 2048), and then a packet is created and allocated memory using malloc.
- A socket is created, and structures for the IP header and ICMP header are also created.
- The receiver waits for the data to be received using the 'recvfrom()' function.
- When the packet is received, the data that was sent is extracted from it and the data size is calculated from the packet size (data size= packet size - header length -8).
- Finally, the data is written into the file that we created in the beginning.





