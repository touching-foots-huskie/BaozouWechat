# include <stdio.h>
# include <sys/socket.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <errno.h>
# include <string.h>
# include <arpa/inet.h>
# include <unistd.h>

# define MAXLINE 1024

using namespace std;

//define the core class in server:
class client_core
{
  public: 
    const char* servInetAddr;  // address of the server
    // socket address is a address
    struct sockaddr_in sockaddr;       //socket adress
    struct sockaddr_in socklisten;  //listen address
    int n;
    int condition;

    // sending socket
    int socketfd;                      //socket fd

    // listen socket
    int listenfd;
    int connfd;

    // save buffer:
    char recvline[MAXLINE], sendline[MAXLINE];

    // for sending
    int fd_connect();
    int send_packet(FILE* stream, int size=1024);
    int fd_close();

    // for recieving:
    int fd_listen();
    int packet_catch(); // catch a packet
    int fd_listen_close();
    //functions: Constructor:
    client_core();

    //Other funcitons:
};

client_core::client_core(void)
{
  this->servInetAddr = "101.6.59.208";
};

int client_core::fd_connect()
{
  // sending port using 10003
  this->socketfd = socket(AF_INET, SOCK_STREAM, 0);
    
  memset(&this->sockaddr, 0, sizeof(this->sockaddr));
  this->sockaddr.sin_family = AF_INET;
  this->sockaddr.sin_port = htons(10003);
  
  // presentation to number
  inet_pton(AF_INET, this->servInetAddr, &this->sockaddr.sin_addr);

  // connect is used to connect the local and TCP server.
  // first make connection
  this->condition = (connect(this->socketfd, (struct sockaddr*) &this->sockaddr, sizeof(this->sockaddr)));
  
  if(this->condition<0)
  {
    printf("connect error %s errno: %d\n", strerror(errno), errno);
    exit(0);
  }

};

int client_core::send_packet(FILE* stream, int size)
{
  // connect first
  this->fd_connect();
  printf("send message to server \n");

  fgets(this->sendline, size, stream);

  if((send(this->socketfd, this->sendline, strlen(this->sendline), 0)) < 0)
  {
    //error no:
    printf("send mes error: %s eerno: %d", strerror(errno), errno);
    exit(0);
  }
  // close the target socket id 
  printf("finish sending! \n");
  this->fd_close();
  return 0; 
};

int client_core::fd_close()
{
  close(this->socketfd);
  return 0;
};

// listen
int client_core::fd_listen()
{
  memset(&this->socklisten, 0, sizeof(this->socklisten));
  this->socklisten.sin_family = AF_INET;
  this->socklisten.sin_addr.s_addr = htonl(INADDR_ANY);
  this->socklisten.sin_port = htons(10004);

  this->listenfd= socket(AF_INET, SOCK_STREAM, 0);

  bind(this->listenfd, (struct sockaddr *) &this->socklisten, sizeof(this->socklisten));

  // open listening structure
  listen(this->listenfd, 1024);
  printf("Begin listening.\n");
}

int client_core::fd_listen_close()
{
  close(this->listenfd);
}

int client_core::packet_catch()
{
  //this function is catching for one time
  //initial:
  int confd;
  int n;  //the length of the actual content
  //begin catching packet:
  
  connfd = accept(this->listenfd, (struct sockaddr*) NULL, NULL);
  
  //error:

  if(connfd==-1)
  {
    printf("accept socket error: %s errno :%d\n", strerror(errno), errno);
  }
  n = recv(connfd, this->recvline, MAXLINE, 0);
  this->recvline[n] = '\0';
  printf("recv msg from client: %s", this->recvline);
  close(connfd);
}

int main(int argc, char **argv)
{
  //echo test:
  // client1 is used to send msg:  
  client_core test;
  test.fd_listen();
  // printf("start listen!\n");
  test.packet_catch();
  test.send_packet(stdin, 1024);
  test.fd_listen_close();
}


