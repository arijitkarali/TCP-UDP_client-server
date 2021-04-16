#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */
#include	<sys/ioctl.h>
#define	MAXLINE		4096
  int main (int argc, char **argv)
  {
      int     sockfd, n,len;
      char     recvline [MAXLINE + 1];
      struct sockaddr_in servaddr;
      struct in_addr **pptr;
      struct in_addr *inetaddrp [2];
      struct in_addr inetaddr;
      struct hostent *hp;
      struct servent *sp;

      if (argc < 3)
         printf("usage: client <hostname> <service>");

      if ( (hp = gethostbyname (argv [1]) ) == NULL) {
         if (inet_aton (argv [1], &inetaddr) == 0) {
            printf("hostname error for %s:", argv [1]);
            exit(1);
         } 
         else {
            inetaddrp [0] = &inetaddr;
             inetaddrp [1] = NULL;
             pptr = inetaddrp;
         }
     } 
     else {
         pptr = (struct in_addr **) hp->h_addr_list;
     }

     if ( (sp = getservbyname (argv [2], "udp") ) == NULL){
        printf("getservbyname error for %s", argv [2] );
        exit(1);
        }

     for ( ; *pptr != NULL; pptr++) {
         sockfd = socket (AF_INET, SOCK_DGRAM, 0) ;  //udp socket

         bzero (&servaddr, sizeof (servaddr) ) ;
         servaddr.sin_family = AF_INET;
         servaddr.sin_port = sp->s_port;
         memcpy (&servaddr.sin_addr, *pptr, sizeof (struct in_addr) ) ;
         printf ("trying \n");

         if (connect (sockfd, (struct sockaddr*) &servaddr, sizeof (servaddr) ) == 0){
		printf("connected\n");
		    break;               /* success */
	 }
         printf("connect error");
        close(sockfd);
         exit(1);
      }
      if (*pptr == NULL){
          printf("unable to connect");
          exit(1);
          }
	
      while ( (n = read (sockfd, recvline, MAXLINE) ) > 0) {
          recvline[n] = 0;               /* null terminate */
          fputs (recvline, stdout);
      }
     	exit (0);
     
}
