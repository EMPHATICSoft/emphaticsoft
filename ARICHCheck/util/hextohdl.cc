#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char** argv) 
{
  int h = strtol(argv[1], NULL, 0);
  char hc[5];
  memset(hc, 0, 5*sizeof(char));
  strncpy(hc, (const char*)&h, 4);
  printf("0x%x = %s\n",h,hc);
  return 0;
}
