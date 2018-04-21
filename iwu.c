/* sa/hsc                                        */
/* from RFProwl.c by rfp                         */
/* compile by : gcc iwu.c -lnet -o iwu           */

#define LIBNET_LIL_ENDIAN 1
#undef  LIBNET_BIG_ENDIAN 1

/* Random IP Spoofing Mode -- ultima -- synk4.c */
#define getrandom(min, max) ((rand() % (int)(((max)+1) - (min))) + (min))

#include <libnet.h>
#include <ctype.h>
#include <unistd.h>


u_long getaddr(char *name) {
   struct hostent *hep;
   hep=gethostbyname(name);
   if(!hep) {
      fprintf(stderr, "Unknown host %s\n", name);
      exit(1);
   }
   return *(unsigned long *)hep->h_addr;
}


int main(int argc, char **argv) {
    int sock, c, i, j, d;
    int byte1, byte2, byte3, byte4;
    char spoofaddr[32];
    u_long src_ip, dst_ip;
    int nbtimes, ttl, len=0, nread=0;
    char version[]="0.1";
    unsigned char *p, hex[2];
    unsigned char sig[1500];
    int verbose = 0;

    if( argc != 6 ) {
      printf("-=- IDS-wake-up by sa/hsc (version %s)-=-\n", version );
      printf("Usage  : ./iwu <srcIP>  <dstIP>  <nb> <ttl> <ip-datagram>\n");
      printf("example: ./iwu 10.0.0.1 20.0.0.2 200  4     \"4500 0018 00f2 0003 4011 73db 0101 0101 0202 0202 e63e 4494\"\n");              
      exit(EXIT_FAILURE);
    }

    srand(time(0)*getpid()*getppid());
    dst_ip=inet_addr(argv[2]);
    nbtimes=atoi(argv[3]);
    ttl=atoi(argv[4]);
    
    for( p=argv[5],j=0; *p && j<1500; p++ ) {
       if( isalnum(*p) ) {
         if( nread==0 ) { 
           nread  = 1;
           hex[0] = *p; 
         } else {
           nread  = 0;
           hex[1] = *p;
           sscanf(hex, "%x", &d);
           sig[j++] = (unsigned char)d;
           len += 1;
         }
       }
    }
    if( len <= 0 ) exit(EXIT_FAILURE);

    sock = libnet_open_raw_sock(IPPROTO_RAW);
    if( sock == -1 ) {
      perror("Socket problems: ");
      exit(EXIT_FAILURE);
    }

    if( ttl < 0 || ttl > 0xFF ) ttl = 66;
    if( nbtimes <= 0 ) nbtimes = 1;

    while( nbtimes-- ) {
      if( strcmp(argv[1], "0" ) == 0 ) {
        byte1 = getrandom(50, 255);
        byte2 = getrandom(0, 255);
        byte3 = getrandom(0, 255);
        byte4 = getrandom(0, 255);
        sprintf(spoofaddr, "%i.%i.%i.%i", byte1, byte2, byte3, byte4);
        src_ip = getaddr(spoofaddr);
      } else { 
        src_ip=inet_addr(argv[1]);
      }
      memcpy(sig+16,&dst_ip,4);
      memcpy(sig+12,&src_ip,4);
      memcpy(sig+8,&ttl,1);

      c = libnet_write_ip(sock, sig, len);  
      if( c < len ) printf("Write_ip choked\n");
      if( c == -1 ) return( EXIT_FAILURE );
    }
    return( EXIT_SUCCESS);
}

