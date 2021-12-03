#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char** argv) 
{
  int h = 0x0000;
  strncpy((char*)&h, argv[1], 4);
  printf("0x%x = %s\n",h,argv[1]);
  return 0;
}
