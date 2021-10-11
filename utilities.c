#include "utilities.h"


void update_flags(uint16_t r)
{
    if (__stack[r] == 0) {
        __stack[R_COND] = F_ZERO;
    } else if (__stack[r] >> 15)  // a 1 in the left-most bit indicates negative
    {
        __stack[R_COND] = F_NEG;
    } else {
        __stack[R_COND] = F_POS;
    }
}

uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

/*char* reg_to_str(uint16_t r)
{
  switch(r)
  {
    case 0:
      return "R0";
    case 1:
      return "R1";
    case 2:
      return "R2";
    case 3:
      return "R3";
    case 4:
      return "R4";
    case 5:
      return "R5";
    case 6:
      return "R6";
    case 7:
      return "R7";
  }
}

char* opcode_to_str(uint16_t op)
{
  switch(r)
  {
    case 0:
      return "BR";
    case 1:
      return "ADD";
    case 2:
      return "LT";
    case 3:
      return "ST";
    case 4:
      return "JSR";
    case 5:
      return "AND";
    case 6:
      return "LDR";
    case 7:
      return "STR";
    case 8:
      return "RTI";
    case 9:
      return "NOT";
    case 10:
      return "LDI";
    case 11:
      return "STI";
    case 12:
      return "RET";
    case 13:
      return "RES";
    case 14:
      return "LEA";
    case 15:
      return "TRAP";
  }
}
*/
