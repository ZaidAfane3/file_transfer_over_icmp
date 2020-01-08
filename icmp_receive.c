/**
    Transfer file by ICMP
    By: liu feiran
    Edited by Zaid Afaneh and Ghazi Abu-Khalaf for a university project  
*/
  

#include "unp.h"


int main(int argc, char **argv)
{

  printf ( "[-] Code Started...\n");



  int buffer_size = 2048; // The size of the data that will be read from the received socket 
  // buffer_size is double the buffer size in the sender (1024)

  // Size of the packet to be reserved in the memory
  int packet_size = sizeof (struct iphdr) + sizeof (struct icmphdr) + buffer_size;
  

  int data_size; // Size of the Data received in the socket 
  
  int ihl, i, n; 
  char *data; // Pointer to the address of the data field.

  FILE *file = fopen("r.txt", "wb+"); // Open output file 
  printf ("[*] New Filed Opened ... \n"); 
  char *packet; // Pointer to  allocate memory to store received packet 
  int sockfd;

  // Allocate memory with the max size of the packet  
  if ( (packet = malloc(packet_size)) == NULL)
  {
  	fprintf(stderr, "[!] Could not allocate memory for packet\n");

	exit(-1);
  }

  printf ("[*] Memory Allocated ...\n");

  // Creates the IPv4 Header at the beginning of the allocated memory and point to it by a pointer 
  struct iphdr *ip = (struct iphdr *) packet;

  // Creates the ICMP Header directly after the end of the IPv4 Header and point to it by a pointer 
  struct icmphdr *icmp = (struct icmphdr *) (packet + sizeof (struct iphdr));

  // Create a raw socket and in case it fails it ends the code.
  if ( (sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
  {
    fprintf(stderr, "[!] Failed to initialize socket\n");
    free(packet);
    exit(-1);
  }

  printf ("[*] Socket Created Successfully \n"); 

  while(1) {
    fprintf (stdout , "[-] Waiting for file . . .  \n");

     // Keep receiving the socket till and error occurs
    if ((i = recvfrom(sockfd, packet, packet_size, 0, NULL, NULL)) == -1)
    {
      fprintf(stderr, "Failed to receive packets\n");
      free(packet);
      exit(-1);
    }

    printf ("[*] Received smth !!\n");
    ihl = (int)ip->ihl << 2; 
    data = (char *)(packet + 28); // Points the beginning of the Data 
    data_size = i - ihl - 8; // Calculate the Data Size the been received 
    fprintf(stdout, "[*] id: %x, all: %d, length %d \n", ntohs(ip->id), i, data_size); // Ignore this 
    fwrite(data, data_size, 1, file); // Write the Data from the packet to the file 
    // fflush write the data to file when it less than the size of the buffer (2048) so it wont wait till its full 
    fflush(file); 
  }
  close(file); 
  printf ("[-] Code finished Execution.\n");
  return(0);
}
