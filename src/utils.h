#pragma once

#include "vm.h"
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

int parseNumber(const char* str);
uint8_t getBytecode(const char* filepath);
