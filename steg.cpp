#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

/**
 * Some of this code is adapted from:
 * http://stackoverflow.com/questions/2881200/linux-can-recvmsg-be-used-to-receive-the-ip-tos-of-every-incoming-packet
 */
using namespace std;

// NOTES:
// Threads for simultaneous input/output for chat.



//////////////////THIS PROGRAM RECEIVES UDP MESSAGES AND DISPLAYS THE SECRET TEXT --- SIT ON ALICE

int main()
{
  int fd;
  struct sockaddr_in addr;
  struct msghdr msg; 
  struct iovec iov[1];  
  int one = 1;
  char pkt[1024];
  char buf[CMSG_SPACE(sizeof(int))];
  struct cmsghdr *cmsg;
  int *ttlptr=NULL;


//DGRAM = UDP 0 is for Flags (none)

  if ((fd = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
    perror( "socket" );
    return 1;
  }

  if (setsockopt(fd,IPPROTO_IP,IP_RECVTTL,&one,sizeof(int)) == -1) {
    perror( "setsockopt" );
    return 1;
  }

  /* listen on port 9999 */
  bzero(&addr,sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr=htonl(INADDR_ANY);
  addr.sin_port=htons(20226);
  bind(fd,(struct sockaddr *)&addr,sizeof(addr));


//fd can listen and send
// need two sockets-- one for sending one for receiving -- they can be on the same port.

while(true) // This is our listen thread
{

	
  /* initialize the message for recvmsg */
  memset(&msg, '\0', sizeof(msg));
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  iov[0].iov_base = pkt; /* starting point of buffer */
  iov[0].iov_len = sizeof(pkt);

  msg.msg_control = buf; // Assign buffer space for control header + header data/value
  msg.msg_controllen = sizeof(buf); //just initializing it

  for( ;; ) {
    /* actually receive the messages */
    if (recvmsg(fd, &msg, 0) == -1) {
      perror( "recvmsg" );
      close(fd);
      return 1;
    }
    
    /* loop through control messages */
    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg,cmsg)) {
      if ((cmsg->cmsg_level == IPPROTO_IP) && 
	  (cmsg->cmsg_type == IP_TTL) &&
	  (cmsg->cmsg_len) ){
	ttlptr = (int *) CMSG_DATA(cmsg);
	printf("received_ttl = %d \n", *ttlptr );
	break;
      }
    }
  }

}

  /* we really never get here */
  close( fd );

  return 0;
}