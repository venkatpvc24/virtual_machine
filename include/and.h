##ifndef AND_H
#defin AND_H


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
  uint8_t op_code;
  uint8_t dst_r;
  uint8_t src_r1;
  uint8_t mode;
  uint8_t src_r2;
} and_t;



and_t* decode_and(uint16_t byte);
void print_and(and_t* a);
int op_and(int num, ...);



#endif
