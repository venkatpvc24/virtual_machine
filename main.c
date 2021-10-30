#include <stdio.h>
#include <limits.h>

#include "assembler.h"
#include "vm.h"

uint16_t __stack[USHRT_MAX];

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    printf("need source file to execute");
  }
  const char* filename = argv[1];
  u16 start_address = 0;
  u16 len_of_data = 0;
  assembler(filename, __stack, &len_of_data, &start_address);
  //__stack[0x3002] = 0xf025;
  //for (int i = start_address; i < 0x3002; i++) printf("0x%04x, ",__stack[i]);
  execute_instructions(__stack, start_address);
}
