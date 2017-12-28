# include <stdio.h>
# include <sys/socket.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <errno.h>
# include <string.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <map>
# include "entry.h"
# include <iostream>
# include <fstream>
# define MAXLINE 1024

using namespace std;

//Useful funcitons:
map<int, int> read_data()
{
  FILE *fp;
  fp = fopen("/home/saturn/Harvey/BaozouWechat/serverData/data.txt","r");
  int a;
  int b;
  map<int, int> IdKey;
  while(1)
  {
    fscanf(fp, "%d,%d",&a, &b);
    IdKey[a] = b;
    if(feof(fp)) break;
  }
  fclose(fp);
  return IdKey;
}

//define the core class in server:
//server is used in 101.6.59.208
class server_core
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

    //map: userId->Ip:
    map<int, char*> userIp;
    map<int, int> IdKey;
    // for sending
    int fd_connect(char* claddr);
    int send_packet(char* this_info, char* claddr);
    int fd_close();

    // for receiving:
    int fd_listen();
    int packet_catch(); // catch a packet
    int fd_listen_close();
    //functions: Constructor:
    server_core();

    //Other functions:
    void show_status(); 
    int login_process(entry* Entry);
    int msg_process(entry* Entry);
    int logout_process(entry* Entry);
};

server_core::server_core(void)
{
  this->servInetAddr = "101.6.161.78";
  // read the Idkey:
  this->IdKey = read_data();
};

int server_core::fd_connect(char* claddr)
{
  this->socketfd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&this->sockaddr, 0, sizeof(this->sockaddr));
  this->sockaddr.sin_family = AF_INET;
  this->sockaddr.sin_port = htons(10004);

  // presentation to number
  inet_pton(AF_INET, claddr, &this->sockaddr.sin_addr);

  // connect is used to connect the local and TCP server.
  // first make connection
  this->condition = (connect(this->socketfd, (struct sockaddr*) &this->sockaddr, sizeof(this->sockaddr)));

  if(this->condition<0)
  {
    printf("connect error %s errno: %d\n", strerror(errno), errno);
    exit(0);
  }

};

int server_core::send_packet(char* this_info, char* claddr)
{
  // send the data to another client.
  // connect first
  this->fd_connect(claddr);
  int actual_len = send(this->socketfd, this_info, 164, 0);
  if((actual_len) < 0)
  {
    //error no:
    printf("send mes error: %s eerno: %d", strerror(errno), errno);
    exit(0);
  }
  // close the target socket id
 this->fd_close();
};

int server_core::fd_close()
{
  close(this->socketfd);
  return 0;
};

// listen part:
int server_core::fd_listen()
{
  memset(&this->socklisten, 0, sizeof(this->socklisten));
  this->socklisten.sin_family = AF_INET;
  this->socklisten.sin_addr.s_addr = htonl(INADDR_ANY);
  this->socklisten.sin_port = htons(10003);

  this->listenfd= socket(AF_INET, SOCK_STREAM, 0);

  bind(this->listenfd, (struct sockaddr *) &this->socklisten, sizeof(this->socklisten));

  // open listening structure
  listen(this->listenfd, 1024);
  printf("Begin listening.\n");
}

int server_core::fd_listen_close()
{
  close(this->listenfd);
}

int server_core::packet_catch()
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
  close(connfd);

  //data process
  this->recvline[n] = '\0';
  
  entry* Entry = (entry*) this->recvline;
  if(Entry->EntryHead.etype == 0)
  {
    this->login_process(Entry);
  }
  else if(Entry->EntryHead.etype == 1)
  {
    this->msg_process(Entry);
  }
  else if(Entry->EntryHead.etype == 2)
  {
    printf("recieved logout\n");
  }
  else
  {
    printf("No such etype\n");
    exit(0);
  }

}

void server_core::show_status()
{
  printf("I am going to show the status of users.\n");
  map<int, char*>::iterator iter;
  for(iter=this->userIp.begin(); iter!=this->userIp.end(); iter++)
  {
    printf("Id: %d|%s\n", iter->first, iter->second);
  }
}

//login process:
int server_core::login_process(entry* Entry)
{
  // this function is used to process the login function:
  // response: Login sucess
  printf("recieved login\n"); 
  //judge first
  if(this->IdKey[Entry->EntryHead.Ad1] == Entry->EntryHead.Ad2)
  {
    printf("your key is right!\n");
  }
  else
  {
    printf("your key is wrong!\n");
    return 0; //stop early
  }

  this->userIp[Entry->EntryHead.userId] = (char*) malloc(sizeof(Entry->data)); 
  memcpy(this->userIp[Entry->EntryHead.userId], Entry->data, sizeof(Entry->data));
  printf("content is %s\n", Entry->data);
 }

int server_core::msg_process(entry* Entry)
{
  // this function is used to process the msg:
  printf("recieved msg!\n");
  printf("It is to %d|%s\n", Entry->EntryHead.towhom, this->userIp[Entry->EntryHead.towhom]);
  // transsend:
  this->send_packet(this->recvline, this->userIp[Entry->EntryHead.towhom]);
}

int server_core::logout_process(entry* Entry)
{
  // this function is used to process the login function:
  // response: Login sucess
  printf("recieved logout\n"); 
  int uId = Entry->EntryHead.userId;
  this->userIp.erase(uId);
  printf("%d has logged out\n", uId);
}

int main(int argc, char **argv)
{

  server_core test;

  test.fd_listen();
  test.packet_catch();
  test.packet_catch();
  test.packet_catch();
  test.fd_listen_close();

  test.show_status();
  //test.send_packet(stdin, 1024);
}


