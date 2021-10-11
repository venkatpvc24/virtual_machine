#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>
#include "LC.h"

void update_flags(uint16_t r);
uint16_t sign_extend(uint16_t r, int bit_count);
// char* enum_to_str(uint16_t r);
// char* opcode_to_str(uint16_t op)

#endif
