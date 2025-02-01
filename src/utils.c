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

char* getBytecode(const char *filepath) {
  FILE *file;
  char character;
  file = fopen(filepath, "r");
  if (NULL == file) {
    printf("\e[0;31mCould not open file.\e[0;37m\n");
    return NULL;
  }

  int maxSize = 256;
  int sizeOfList = 0;
  char* list = malloc(maxSize* sizeof(char));
  if (NULL == list) {
    printf("\e[0;31mCould not allocate list.\e[0;37m\n");
    fclose(file);
    return NULL;
  }

  while ((character = fgetc(file)) != EOF) {
    list[sizeOfList] = character;
    ++sizeOfList;
    if (sizeOfList == maxSize-1) {
      maxSize += 256;
      list = realloc(list, maxSize * sizeof(char));
      if (NULL == list) {
        printf("\e[0;31mCould not reallocate list.\e[0;37m\n");
        free(list);
        return NULL;
      }
    }
  }
  fclose(file);

  char code[sizeOfList];

  for (int i = 0; i < sizeOfList; i++) {
    code[i] = list[i];
  }
  free(list);

  return &code;
}

uint8_t* parseBytecode(const char* code) {

  char delimiter = " ";


}

int parseNumber(const char *str) {
  if(strncasecmp(str, "0x", 2) == 0) {
    return (int) strtol(str, NULL, 16);
  } else {
    return atoi(str);
  }
}

uint8_t getOpcodeFromChar(const char* opcode) {
  switch (opcode) {
    case "PUSH": {
      return PUSH;
    }
    case "PUSHP": {
      return PUSHP;
    }
    case "POP": {
      return POP;
    }
      case "ADD": {
      return ADD;
    }
    case "SUB": {
      return SUB;
    }
    case "MUL": {
      return MUL;
    }
    case "DIV": {
      return DIV;
    }
    case "JMP": {
      return JMP;
    }
    case "JNE": {
      return JNE;
    }
    case "JE": {
      return JE;
    }
    case "JG": {
      return JG;
    }
    case "DPRINT": {
      return DPRINT;
    }
    case "SPRINT": {
      return SPRINT;
    }
    case "DPRINTST": {
      return DPRINTST;
    }
    case "DUP": {
      return DUP;
    }
    case "SWAP": {
      return SWAP;
    }
    case "END": {
      return END;
    }
    case default: {
      printf("String is not a valid opcode identifier.\n")
      return NULL;
    }
  }
}
