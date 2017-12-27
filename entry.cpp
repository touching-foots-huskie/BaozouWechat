/**********************************
  This file is used to code and decode the entry object
  *********************************/

# include "entry.h"
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>

entry wrap_info(char* infos, int userId, int etype)
{
  // wrap the info into entries:
  entry_head EntryHead;
  EntryHead.etype = etype;
  EntryHead.userId = userId;
  EntryHead.bufferSize = sizeof(infos);

  entry Entry;
  Entry.EntryHead = EntryHead;

  Entry.data = (char*) malloc(EntryHead.bufferSize);
  strcpy(Entry.data, infos);
  return Entry;
}


int main()
{
  //Entry Heead;
  int userId = 255;
  char toWhom[] = "chn,otc";
  int toWhomSize = sizeof(toWhom);

  entry Entry;

  Entry = wrap_info(toWhom, userId, 0);
  //take out the data:
  printf("userId is %d\n", Entry.EntryHead.userId);
  printf("etype is %d\n", Entry.EntryHead.etype);
  printf("toWhom is %s\n", Entry.data);
  return 0;
}
