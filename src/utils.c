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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>

#include "utils.h"
#include "vm.h"

char* getBytecode(const char *filepath) {
  FILE *file;
  if (debug)
    printf("Getting bytecode\n");
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

  char* code = malloc(sizeOfList * sizeof(char));

  for (int i = 0; i < sizeOfList; i++) {
    code[i] = list[i];
  }
  free(list);

  if (debug) {
    for (int i = 0; i < sizeOfList; i++) {
      printf("%c", code[i]); // Use array indexing
    }
    printf("\n");
  }

  return code;
}

uint8_t* parseBytecode(VM* vm, char* code, int codeSize) {
  int a, b, sizeOfCode = 0;
  char* text;
  bool isInComment = false, isInData = false, isInCode = false;
  //uint8_t bytecode[codeSize];
  uint8_t* bytecode = malloc(codeSize * sizeof(uint8_t));
  if (!bytecode) {
    printf("Memory allocation failed!\n");
    return NULL;
  }

  if (debug)
    printf("Parsing bytecode\n");
  text = strtok(code, " ");
  while (text != NULL) {
    if (debug)
      printf("Current token is %s\n", text);
    if (strncasecmp(text, "//", 2) == 0) {
      isInComment = true;
    }
    else if (strncasecmp(text, ".data:", 6) == 0) {
      if (debug)
        printf("is in data!\n");
      isInData = true;
      text = strtok(NULL, " ");
      if (!text) continue;
    }
    else if (strncasecmp(text, ".code:", 6) == 0) {
      if (debug)
        printf("is in code!\n");
      isInData = false;
      isInCode = true;
      text = strtok(NULL, " ");
      if (!text) continue;
    }
    else if (isInComment) {
      if (debug)
        printf("is in a comment!\n");
      strtok(NULL, "\n");
      isInComment = false;
      text = strtok(NULL, " ");
      continue;
    }
    else if (isInData) {
      if (debug)
        printf("is in a comment!\n");
      if (strncasecmp(text, ".string", 7) == 0) {
        if (debug)
          printf("Found string! Adding to vm!\n");
        text = strtok(NULL, " ");
        a = parseNumber(text);
        char* string = strtok(NULL, "\"");
        vm->pool[a] = (const_t){a, 0, .Svalue = string};
        text = strtok(NULL," ");
        if (!text) continue;
      }
      else if (strncasecmp(text, ".int", 4) == 0) {
        if (debug)
          printf("Found Int! Adding to vm!\n");
        text = strtok(NULL, " ");
        if (debug)
          printf("%s", text);
        a = parseNumber(text);
        text = strtok(NULL, " ");
        if (debug)
          printf("%s", text);
        b = parseNumber(text);
        if (debug)
          printf("%s", text);
        vm->pool[a] = (const_t){a, 1, .Ivalue = b};
        text = strtok(NULL," ");
        if (!text) continue;
      }
    }
    else if (isInCode) {
      b = getOpcodeFromChar(text);
      if (debug)
        printf("Getting opcode %s\n", text);
      if (b != -1) { // we check if it is an opcode
        bytecode[sizeOfCode] = b;
        ++sizeOfCode; //increment a to keep up with bytecode size...
      } else { // if it is not an opcode, we know it is a byte
        bytecode[sizeOfCode] = parseNumber(text);
        ++sizeOfCode; //increment a to keep up with bytecode size...
      }
    }
    text = strtok(NULL, " ");
  }

  uint8_t* codetoRet = malloc(sizeOfCode * sizeof(uint8_t));
  if (codetoRet == NULL) {
    printf("Failed to allocate bytecode!\n");
    return NULL;
  }

  memcpy(codetoRet, bytecode, sizeOfCode);
  free(bytecode);
  vm->codeSize = 30;

  if (debug) {
    printf("The code to run is:\n"); // Use array indexing
    for (int i = 0; i < sizeOfCode; i++) {
      printf("0x%02x ", codetoRet[i]); // Use array indexing
    }
    printf("\n");
  }

  return codetoRet;
}

int parseNumber(const char *str) {
  if(strncasecmp(str, "0x", 2) == 0) {
    return (int) strtol(str, NULL, 16);
  } else {
    return atoi(str);
  }
}

int getOpcodeFromChar(const char* opcode) {
  if (strcmp("PUSH", opcode) == 0) {
    return PUSH;
  }
  if(strcmp("PUSHP", opcode) == 0) {
    return PUSHP;
  }
  if(strcmp("POP", opcode) == 0) {
    return POP;
  }
  if(strcmp("ADD", opcode) == 0) {
    return ADD;
  }
  if(strcmp("SUB", opcode) == 0) {
    return SUB;
  }
  if(strcmp("MUL", opcode) == 0) {
    return MUL;
  }
  if(strcmp("DIV", opcode) == 0) {
    return DIV;
  }
  if(strcmp("JMP", opcode) == 0) {
    return JMP;
  }
  if(strcmp("JNE", opcode) == 0) {
    return JNE;
  }
  if(strcmp("JE", opcode) == 0) {
    return JE;
  }
  if(strcmp("JG", opcode) == 0) {
    return JG;
  }
  if(strcmp("DPRINT", opcode) == 0) {
    return DPRINT;
  }
  if(strcmp("SPRINT", opcode) == 0) {
    return SPRINT;
  }
  if(strcmp("DPRINTST", opcode) == 0) {
    return DPRINTST;
  }
  if(strcmp("DUP", opcode) == 0) {
    return DUP;
  }
  if(strcmp("SWAP", opcode) == 0) {
    return SWAP;
  }
  if(strcmp("END", opcode) == 0) {
    return END;
  }
  //printf("String is not a valid opcode identifier.\n");
  return -1;
}
