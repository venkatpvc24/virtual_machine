#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "assembler.h"

int main(int argc, char** argv)
{
  printf("argv[0]: %s, argv[1]: %s\n", argv[0], argv[1]);
  node_t* node = parser(argv[1]);
  assembler(&node);

}
