//Users/burd691 /* Sample UDP client */
// C++ VERSION

#include<pthread.h>

#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
//#include  <thread>
#include <string>

using namespace std;

static void* receive_data(void *arg);
static void*  send_data(void *arg); // prototype

int main(int argc, char**argv)
{

pthread_t first;
pthread_t second;

cout<<"Program Started"<<endl;
int result = pthread_create(&first,NULL,&send_data,NULL); 
if(result != 0)
{
cout<<"Error creating first thread!"<<endl;
return 1;
}

int result2 = pthread_create(&second,NULL,&receive_data,NULL);
if(result2 != 0)
{
cout<<"Error creating second thread"<<endl;
return 1;
}

pthread_join(first, NULL); // MAIN thread must wait for first to finish.
pthread_join(second,NULL);


return 0;
}

void* receive_data(void *arg)
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
  }

  if (setsockopt(fd,IPPROTO_IP,IP_RECVTTL,&one,sizeof(int)) == -1) {
    perror( "setsockopt" );
  }

  /* listen on port 20226 */
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

string full_message;
  for( ;; ) {
/* actually receive the messages */
    if (recvmsg(fd, &msg, 0) == -1) {
      perror( "recvmsg" );
      close(fd);
    }
    
    /* loop through control messages */
    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg,cmsg)) {
      if ((cmsg->cmsg_level == IPPROTO_IP) && 
          (cmsg->cmsg_type == IP_TTL) &&
          (cmsg->cmsg_len) ){
        ttlptr = (int *) CMSG_DATA(cmsg);

        char  recmsg = (char) *ttlptr;

	if(recmsg  == '\n')
{
	full_message.push_back(recmsg);
	cout<<"Received: "<<full_message;
	full_message.clear(); // empty for next message        
}
	else // add to the full message
		full_message.push_back(recmsg);
break;
      }
    }
  }

}

  /* we really never get here */
  close( fd );
}



void* send_data(void *arg)
{
cout<<"Hello world!"<<endl;

string  input_destination;

cout<<"Please enter the destination (i.e. bob.cs-georgetown.net) :";
cin>>input_destination;
cout<<endl;


fstream infile;
	infile.open("taleoftwocities.txt");

if (infile.is_open() == false)
{
	std::cout<<"Error opening file"<<endl;
	
}

//////// START INTERNET CODE

struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];
   char *dest;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(input_destination.c_str(), NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    }

    printf("IP addresses for %s:\n\n", input_destination.c_str());

    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("  %s: %s\n", ipver, ipstr);
	dest = ipstr;
    }

    freeaddrinfo(res); // free the linked list

printf(dest);






////// END INTERNET CODE
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   char sendline[1000];
   char recvline[1000];



   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(dest);
   servaddr.sin_port=htons(20226);



   while (fgets(sendline, 10000,stdin) != NULL)
   {
	for(int i =0; i<strlen(sendline); i++)
{
	int value  = (int)sendline[i];
        void *optval = (int  *)value;

int return_val = setsockopt(sockfd, IPPROTO_IP, IP_TTL, &optval, sizeof(int)) ; 

if(return_val != 0)
printf(strerror(errno));


	char textdata[128];
	infile.getline(textdata,128);

      sendto(sockfd,textdata,strlen(textdata),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));

}
   }
}
