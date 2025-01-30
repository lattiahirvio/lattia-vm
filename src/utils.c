/* 
 *
 *    Copyright (C) 2025 lattiahirvio
 *
 *    This file is part of lattia-vm.
 *
 *    lattia-vmis free software: you can redistribute it and/or modify
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

uint8_t getBytecode(const char *filepath) {
  FILE *file;
  char characters;
  file = fopen(filepath, "r");
  if (NULL == file) {
    printf("\e[0;31mCould not open file.\e[0;37m\n");
    return EXIT_FAILURE;
  }

  int* list = malloc(2* sizeof(int));
  int maxSize = 256;
  int sizeOfList = 0;
  if (NULL == list) {
    printf("\e[0;31mCould not allocate list.\e[0;37m\n");
  }
  uint8_t code[256];
  while ((characters = fgetc(file)) != EOF) {
    code;
  }

  return code;
}

int parseNumber(const char *str) {
  if(strncasecmp(str, "0x", 2) == 0) {
    return (int) strtol(str, NULL, 16);
  } else {
    return atoi(str);
  }
}



