/*
    Transfer file by ICMP
    By: liu feiran
    Edited by Zaid Afaneh and Ghazi Abu-Khalaf for a university project  
*/
  

#include "unp.h"

typedef unsigned char u8;
typedef unsigned short int u16;

unsigned long daddr; // destination address in network format 
unsigned long saddr; // source address in network format 
int Data_size = 0, sent, sent_size, sockfd; 
char *packet; // pointer to the socket address in the memory 


// The size of Data being read from file to be send in bytes 
int max_buffer_size = 1024;

// Function to print the arguments needed for this code 
void usage(){
    printf("Usage: ICMP_File_VPN <DEST-IP> <SOURCE-IP> <FILE> <MAX-BUFFER-SIZE> <HELP>\n");
}


// function calculates the checksum 
unsigned short checksum(unsigned short *ptr, int nbytes)
{
    register long sum;
    u_short oddbyte;
    register u_short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *) & oddbyte) = *(u_char *) ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return (answer);
}


// Functions creates the socket 
int create_send_socket()
{
    // create a raw socket and in case it fails it end the code.  
    if ( (sockfd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
    {
        perror("could not create socket");
        return (1);
    }

    // in case of success above we set the socket options to tell the kernal we will provide the Network Header (IPv4)
    int on = 1;
    if (setsockopt (sockfd, IPPROTO_IP, IP_HDRINCL, (const char*) &on, sizeof (on)) == -1)
    {
        perror("setsockopt");
        return (2);
    }
    return(0);
}


// Function Send the file 
int send_file(FILE *file, int max_buffer_size)
{
    // Pointer to the address of the data in the memory 
    char *Data; 
    int buffer_size = 0;
    // Calculate total packet size
    Data_size = max_buffer_size;

    // Size of the packet will be the sum of the (IPv4 Header Size + ICMP Header Size + Data size {1024 bytes}) 
    int packet_size = sizeof (struct iphdr) + sizeof (struct icmphdr) + Data_size; 

    // Allocate memory with the size of the packet 
    // malloc function does memory allocation with the size provided and pointed to by the pointer *packet 
    char *packet = (char *) malloc (packet_size);

    // if the packet pointer equals Null this means that no memory was allocated and no space for the packet and ends the code 
    // Null = False 
    if (!packet)
    {
        perror("[*] Memory ba7");
        close(sockfd);
        return (1);
    }

    // Creates the IPv4 Header at the beginning of the allocated memory and point to it by a pointer 
    struct iphdr *ip = (struct iphdr *) packet;

    // Creates the ICMP Header directly after the end of the IPv4 Header and point to it by a pointer 
    struct icmphdr *icmp = (struct icmphdr *) (packet + sizeof (struct iphdr));

    // Zero all the fields in two headers 
    memset (packet, 0, packet_size);

    // Filling IPv4 header's field 
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons (packet_size);
    ip->id = 0x3713;
    ip->frag_off = 0;
    ip->ttl = 255;
    ip->protocol = IPPROTO_ICMP;
    ip->saddr = saddr;
    ip->daddr = daddr;

    // Filling ICMP header's field 
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.sequence = rand();
    icmp->un.echo.id = rand();

    icmp->checksum = 0;

    // Address structs creation and zero all fields (equivalent to the function bzero)
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = daddr;
    memset(&servaddr.sin_zero, 0, sizeof (servaddr.sin_zero));

    // Points the Data pointer to the address where the data field is 
    Data = packet + sizeof(struct iphdr) + sizeof(struct icmphdr);

    // Keep reading from the file till the reading size is zero and fill it in the data field 
    while((buffer_size = fread(Data, 1, max_buffer_size, file)) != 0) {

    	// Calculates the checksum of the icmp header and fill it in the field 
        icmp->checksum = checksum((unsigned short *)icmp, sizeof(struct icmphdr) + buffer_size);

        // Calculates the size of the packet before send it 
        packet_size = sizeof (struct iphdr) + sizeof (struct icmphdr) + buffer_size;
        printf("read %d ", buffer_size);
        
        // Send the packet in the allocated memory through the raw socket 
        if ( (sent_size = sendto(sockfd, packet, packet_size, 0, (struct sockaddr*) &servaddr, sizeof (servaddr))) < 1)
        {
            perror("send failed\n");
            return (2);
        }

        ++sent;
        printf("%d packets sent\r", sent);
        fflush(stdout);

        // Reset the Data pointer to the beginning of the data field  
        Data = packet + sizeof(struct iphdr) + sizeof(struct icmphdr);
        
    }
    return (0);
}

int main(int argc, char **argv) {

    int quit = 0;


    FILE *file; // Pointer of type file 

    // check arguments in case its less than the required ot it has "Help", prints Usage 
    for (int i = 0; i < argc; i++) {
        if (argv[i] == "help" || argc < 4 || argc == 1 ) {
            usage();
            return 0;
        }
    }

    // Convert address to network format 
    saddr = inet_addr(argv[2]);
    daddr = inet_addr(argv[1]);

    // Check if the file exists (accessible)
    if (access(argv[3], F_OK) != -1) {
        // if can be accessed then try to open it 
        file = fopen(argv[3], "rb");
        
        // in case of failure, end the code 
        if (!file) {
            printf("[!] The FILE is 7ardan, not opening :(\n");
            return 0;
        }

        printf("[*] File Found and Opened :D\n");
    }
    else{
        printf("[!] Where is the FILE :| ?\n");
        return 0;
    }
     

    // Call the functions to start the functionality of the code 
    create_send_socket();
    	printf ("[*] Socket Created Successfully \n");
    send_file(file, max_buffer_size);
		printf("[*] File Sent ... \n");
    free(packet);
		printf ("[*] Memory Freed ... \n");
    close(sockfd);
		printf ("[*] Code Finished Exceution.\n");	
    
    return 0;
   }
