#include <stdio.h>
#include <limits.h>

#include "include/assembler.h"
#include "include/vm.h"

extern u16 start_address;

u16 data[USHRT_MAX];

int main(int argc, char** argv)
{
  
  if(argc < 2)
  {
    printf("need source file to execute");
  }
  const char* filename = argv[1];
  u16 len_of_data = 0;
  assembler(filename, &len_of_data);
  //for (int i = 12288; i < 12345; i++) printf("0x%04x, ", __stack[i]);
  execute_instructions(data, start_address);

}