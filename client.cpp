# include <stdio.h>
# include <sys/socket.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <errno.h>
# include <string.h>
# include <arpa/inet.h>
# include <unistd.h>
# include "entry.h"
# include <iostream>
# include <map>
# include <boost/algorithm/string.hpp> 

# define MAXLINE 1024

using namespace std;
using namespace boost;

//define the core class in server:
class client_core
{
  public:
    const char* servInetAddr;  // address of the server
    int userId;
    string name;
    //friend list:
    map<string, int> friend_list;
    map<int, string> iv_friend_list;
    // socket address is a address
    struct sockaddr_in sockaddr;       //socket address
    struct sockaddr_in socklisten;  //listen address
    int n;
    int condition;

    const char* MynetAddr;
    // sending socket
    int socketfd;                      //socket fd

    // listen socket
    int listenfd;
    int connfd;

    // save buffer:
    char recvline[MAXLINE], sendline[MAXLINE];
    char content[164];
    // for sending
    int fd_connect();
    int send_packet(char* stream);
    int fd_close();

    // for recieving:
    int fd_listen();
    int packet_catch(); // catch a packet
    int fd_listen_close();
    //functions: Constructor:
    client_core();

    //Other functions:
    char* loginWrap();
    char* sendMsg();
    char* logoutWrap();
};

client_core::client_core(void)
{
  this->MynetAddr = "101.6.161.78";
  this->servInetAddr = "101.6.161.78";
  this->userId = 255; //I know
  this->name = "chn";
  // add friend list:
  this->friend_list["otc"] = 100; //only using one friend.
  this->friend_list["chn"] = 255; //only using one friend.
  this->iv_friend_list[100] = "otc";
  this->iv_friend_list[255] = "chn";
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

int client_core::send_packet(char* this_info)
{
  // connect first
  this->fd_connect();

  // fgets(this->sendline, size, stream);
  //copy the content:
  //strcpy(this->sendline, this_info);

  int actual_len = send(this->socketfd, this_info, 164, 0);
  if(actual_len < 0)
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
  entry* Entry = (entry*) this->recvline;
  
  cout<<"msg from:"<< this->iv_friend_list[Entry->EntryHead.userId]<<endl;
  printf("%s", Entry->data);
  close(connfd);
  return 0;
}

//loginfo:

char* client_core::loginWrap()
{

  printf("Your ID:\n");
  char id[20];
  char kw[20];
  int myid;
  int mykw;
  fgets(id, 1024, stdin);
  //int ID = (int)id;
  //printf("%d\n", atoi(id));
  myid = atoi(id);
  printf("Your Keyword:\n");
  fgets(kw, 1024, stdin);
  mykw = atoi(kw);
  // add into ad part:
  entry Entry = wrap_info(this->MynetAddr, this->userId, 0);
  Entry.EntryHead.Ad1 = myid;
  Entry.EntryHead.Ad2 = mykw;
  memset(this->content, '0', 164);
  memcpy(this->content, &Entry, 164);
  return this->content;
}

char* client_core::sendMsg()
{
  //enter from stdin:
  char name[1024];
  printf("you want to send to whom?\n");
  fgets(name, 1024, stdin);
  string s = string(name);
  trim(s);
  int toWhom = this->friend_list[s];
  printf("Id is %d\n", toWhom);
  printf("write what you want to say:\n");
  fgets(this->sendline, 1024, stdin);
  printf("%s", this->sendline);
  entry Entry = wrap_info(this->sendline, this->userId, 1);
  Entry.EntryHead.towhom = toWhom;
  memset(this->content, '0', 164);
  memcpy(this->content, &Entry, 164);
  return this->content;
}

char* client_core::logoutWrap()
{
  char infos[] = "Logging out\n";
  entry Entry = wrap_info(infos, this->userId, 2);
  memset(this->content, '0', 164);
  memcpy(this->content, &Entry, 164);
  return this->content;
}

int main(int argc, char **argv)
{
  client_core test;
  //login first:
  char* login_data = test.loginWrap();
  test.send_packet(login_data);

  
  char* msg_data = test.sendMsg();
  test.send_packet(msg_data);
  
  // listen to the response:
  test.fd_listen();
  test.packet_catch();
  test.fd_listen_close();
  
  //logout:
  char* logout_data = test.logoutWrap();
  test.send_packet(logout_data);
  
}


