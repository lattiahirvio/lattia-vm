/* 
 *
 *    Copyright (C) 2025 lattiahirvio
 *
 *    This file is part of lattia-vm.
 *
 *    lattia-vm is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    any later version.
 *
 *    lattia-vm is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with lattia-vm.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "vm.h"
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

int parseNumber(const char* str);
char* getBytecode(const char* filepath);
uint8_t* parseBytecode(VM* vm, char* code, int codeSize);
int getOpcodeFromChar(const char* opcode);
void discardUntilNewline(char** str);
