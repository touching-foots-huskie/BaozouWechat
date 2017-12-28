// this file is used to describe the user and keyword structure.
# include "UK.h"
# include <fstream>
# include <iostream>
# include <string>
# include <map>

using namespace std;

map<int, int> read_data()
{
  FILE *fp;
  fp = fopen("/home/saturn/Harvey/BaozouWechat/serverData/data.txt","r");
  int a;
  int b;
  map<int, int> IdKey;
  while(1)
  {
    cout<<"Readone"<<endl;
    fscanf(fp, "%d,%d",&a, &b);
    IdKey[a] = b;
    cout<<a<<b<<endl;
    if(feof(fp)) break;
  }
  fclose(fp);
  return IdKey;
}
