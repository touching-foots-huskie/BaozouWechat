/**********************************
  This file is used to code and decode the entry object
  *********************************/

# include "entry.h"
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <iostream>
using namespace std;

entry wrap_info(const char* infos, int userId, int etype)
{
  // wrap the info into entries:
  entry_head EntryHead;
  EntryHead.etype = etype;
  EntryHead.userId = userId;

  entry Entry;
  Entry.EntryHead = EntryHead;
  strcpy(Entry.data, infos);
  return Entry;
};


