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

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>

#include "utils.h"
#include "vm.h"

// called like printf - prints to stderr then immediately exits the program
void error(const char *format, ...) {
  // i hate varargs
  va_list args;
  va_start(args, format);
  fprintf(stderr, format, args);
  va_end(args);

  exit(EXIT_FAILURE);
}

// if (!condition) error(...)
void assert(bool condition, const char *format, ...) {
  if (condition)
    return;

  // i hate varargs
  va_list args;
  va_start(args, format);
  error(format, args);
  va_end(args);
}

// reads a whole file to a char*
//
// (ChatGPT)
char *readFileToStr(const char *filepath) {
  FILE *f = fopen(filepath, "rb");
  if (!f)
    return NULL;

  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return NULL;
  }

  long size = ftell(f);
  if (size < 0) {
    fclose(f);
    return NULL;
  }

  rewind(f);

  char *buf = malloc(size + 1);
  if (!buf) {
    fclose(f);
    return NULL;
  }

  size_t read = fread(buf, 1, size, f);
  fclose(f);

  if (read != (size_t)size) {
    free(buf);
    return NULL;
  }

  buf[size] = '\0';
  return buf;
}

// convenience function
bool wsat(char **code) { return isspace((unsigned char)**code); }

// skips whitespace and comments
void parseWhitespace(char **code) {
  while (true) {
    if (wsat(code)) {
      *code += 1;
    } else if (strncmp(*code, "//", 2) == 0) {
      while (**code && (*code)[0] != '\n') {
        *code += 1;
      }
    } else {
      break;
    }
  }
}

// parses a hex number until whitespace (assuming 0x is already skipped)
int32_t parseHex(char **code) {
  int out = 0;
  while (!wsat(code)) {
    char c = **code;
    out *= 16;
    if (c >= '0' && c <= '9') {
      out += c - '0';
    } else {
      c = toupper(c);
      assert(c >= 'A' && c <= 'F', "Error: Invalid hex literal somewhere.");
      out += c - 'A' + 10;
    }
    *code += 1;
  }
  return out;
}

// parses a decimal number until whitespace
int32_t parseDec(char **code) {
  int out = 0;
  while (!wsat(code)) {
    char c = **code;
    assert(c >= '0' && c <= '9', "Error: Invalid decimal literal somewhere.");
    out = out * 10 + c - '0';
    *code += 1;
  }
  return out;
}

// parses either a dec number or hex number if it starts with 0x
int32_t parseNumber(char **code) {
  int sign = 1;
  if (**code == '-') {
    (*code)++;
    sign = -1;
  }
  if (strncmp(*code, "0x", 2) == 0) {
    *code += 2;
    return parseHex(code) * sign;
  } else {
    return parseDec(code) * sign;
  }
}

char *parseString(char **code) {
  if (debug)
    printf("Parsing String!\n");
  assert(**code == '"', "Error: Expected string literal somewhere.");
  (*code)++;
  const size_t MAX_SIZE = 1024;
  char *limit = *code + MAX_SIZE;
  char *str = malloc(MAX_SIZE);
  char *next = str;
  while (**code && **code != '"' && *code < limit) {
    if (**code != '\\') {
      *next = **code;
      if (debug)
        printf("CHAR: %c [%d]\n", *next, *next);
      next++;
      (*code)++;
      continue;
    }
    (*code)++;
    switch (**code) {
    case '\\':
      *next = '\\';
      break;
    case 'r':
      *next = '\r';
      break;
    case 'n':
      *next = '\n';
      break;
    case 't':
      *next = '\t';
      break;
    case '0':
      *next = '\0';
      break;
    default:
      printf("Warning: Invalid escape character found somewhere: %c", **code);
    }
    if (debug)
      printf("CHAR: [%d]\n", *next);
    next++;
    (*code)++;
  }
  *next = '\0';
  next++;
  str = realloc(str, next - str);
  assert(**code == '"', "Error: Unterminated string literal.");
  (*code)++;
  return str;
}

void parseData(VM *vm, char **code) {
  if (strncmp(*code, ".data:", 6) != 0) {
    if (debug)
      printf("No .data section found.");
    return;
  }
  if (debug)
    printf("Found .data section!");

  while (true) {
    parseWhitespace(code);
    if (strncmp(*code, ".int", 4) == 0) {
      if (debug)
        printf("Found Int! Adding to vm!\n");
      *code += 4;
      parseWhitespace(code);
      int index = parseNumber(code);
      parseWhitespace(code);
      int32_t value = parseNumber(code);
      vm->pool[index] = (const_t){index, 1, .Ivalue = value};
      if (debug)
        printf("Int was %d\n", value);
    } else if (strncmp(*code, ".string", 7) == 0) {
      if (debug)
        printf("Found String! Adding to vm!\n");
      *code += 7;
      parseWhitespace(code);
      int index = parseNumber(code);
      parseWhitespace(code);
      char *value = parseString(code);
      vm->pool[index] = (const_t){index, 0, .Svalue = value};
      if (debug)
        printf("String was %s\n", value);
    } else {
      if (debug)
        printf("End of .data section...");
      return;
    }
  }
}

void testString() {
  char *code = "\"Hello, world!\\r\\n\\t\\0 \"";
  char *s = parseString(&code);
  printf("STRING: %s", s);
}

// what do you think this function does
void test() {
  testString();
  return;
  // char buf[] = "//test  \n\n// this is a comment\n  123 0xdE4dbe3f hello
  // world"; char *code = buf;
  char *code = readFileToStr("bytecode/fibonacci.lvmasm");
  parseWhitespace(&code);
  printf("%s", code);
  // int a = parseNumber(&code);
  // parseWhitespace(&code);
  // int b = parseNumber(&code);
  // parseWhitespace(&code);
  // printf("%d, 0x%x, '%s'", a, b, code);
}

uint8_t *parseStrToBytecode(VM *vm, char *code, int codeSize) {
  error("TODO: Parse and compile LVMASM to Bytecode");
  return NULL;
}

int getOpcodeFromChar(const char *opcode) {
  if (strcmp("PUSH", opcode) == 0) {
    return PUSH;
  }
  if (strcmp("PUSHP", opcode) == 0) {
    return PUSHP;
  }
  if (strcmp("POP", opcode) == 0) {
    return POP;
  }
  if (strcmp("ADD", opcode) == 0) {
    return ADD;
  }
  if (strcmp("SUB", opcode) == 0) {
    return SUB;
  }
  if (strcmp("MUL", opcode) == 0) {
    return MUL;
  }
  if (strcmp("DIV", opcode) == 0) {
    return DIV;
  }
  if (strcmp("JMP", opcode) == 0) {
    return JMP;
  }
  if (strcmp("JNE", opcode) == 0) {
    return JNE;
  }
  if (strcmp("JE", opcode) == 0) {
    return JE;
  }
  if (strcmp("JG", opcode) == 0) {
    return JG;
  }
  if (strcmp("DPRINT", opcode) == 0) {
    return DPRINT;
  }
  if (strcmp("SPRINT", opcode) == 0) {
    return SPRINT;
  }
  if (strcmp("DPRINTST", opcode) == 0) {
    return DPRINTST;
  }
  if (strcmp("DUP", opcode) == 0) {
    return DUP;
  }
  if (strcmp("SWAP", opcode) == 0) {
    return SWAP;
  }
  if (strcmp("END", opcode) == 0) {
    return END;
  }
  // printf("String is not a valid opcode identifier.\n");
  return -1;
}
