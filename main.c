#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "assembler.h"

int main(int argc, char** argv)
{
  uint16_t output[UINT16_MAX];
  uint16_t size = 0;
  node_t* node = parser(argv[1]);
  assembler(&node, output, &size);
  execute_instructions(output, size, 0x3000);
}
