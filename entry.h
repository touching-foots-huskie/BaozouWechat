/****************************
 Author: Harvey Chang
 Created At: 2017年12月26日 星期二 23时52分26秒
 Filename: entry.h
 Description:
   " This file is used to describe the data structure, we used in communicating"
****************************/
#ifndef __HEADER__
#define __HEADER__
# include<string>
# include<iostream>
# include <stdlib.h>
# include <stdio.h>
using namespace std;

struct entry_head
{
  // entry is the msg communicating between server and client.
  int etype;
  /*
    0: login
    1: msg
    2: logout
   */
  int userId;
  // userId is a 8-bit number, for we only have a limited users
  int towhom;
  int Ad1;  //additional data:
  int Ad2;
  int Ad3;
};

struct entry
{
  entry_head EntryHead;
  char data[140];
};

entry wrap_info(const char* infos, int userId, int etype);

#endif
