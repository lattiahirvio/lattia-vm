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
bool wsat(const char **code) { return isspace((unsigned char)**code); }

// if str's first whole token matches the specified token, returns the length of
// the specified token. else returns 0.
//
// assumes both str and prefix are null-terminated
int firstTokenMatches(const char *str, const char *token) {
  size_t i = 0;
  while (token[i] && str[i] == token[i])
    i++;
  if (token[i])
    return 0;
  if (str[i] && !isspace((unsigned char)str[i]))
    return 0;
  return i;
}

// skips whitespace and comments
void parseWhitespace(const char **code) {
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
int32_t parseHex(const char **code) {
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
int32_t parseDec(const char **code) {
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
int32_t parseNumber(const char **code) {
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

char *parseString(const char **code) {
  if (debug)
    printf("Parsing String!\n");
  assert(**code == '"', "Error: Expected string literal somewhere.");
  (*code)++;
  const size_t MAX_SIZE = 1024;
  const char *limit = *code + MAX_SIZE;
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
    case '"':
      *next = '"';
      break;
    default:
      if (debug)
        printf("Warning: Invalid escape character found somewhere: %c", **code);
      *next = **code;
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

void parseData(VM *vm, const char **code) {
  if (strncmp(*code, ".data:", 6) != 0) {
    if (debug)
      printf("No .data section found.\n");
    return;
  }
  *code += 6;
  if (debug)
    printf("Found .data section!\n");

  while (true) {
    parseWhitespace(code);
    if (firstTokenMatches(*code, ".string")) {
      if (debug)
        printf("Found String! Adding to vm!\n");
      *code += 7;
      parseWhitespace(code);
      int location = parseNumber(code);
      parseWhitespace(code);
      char *value = parseString(code);
      vm->pool[location] = (const_t){location, 0, .Svalue = value};
      if (debug)
        printf("String at %d was \"%s\"\n", location, value);
    } else if (firstTokenMatches(*code, ".int")) {
      if (debug)
        printf("Found Int! Adding to vm!\n");
      *code += 4;
      parseWhitespace(code);
      int location = parseNumber(code);
      parseWhitespace(code);
      int32_t value = parseNumber(code);
      vm->pool[location] = (const_t){location, 1, .Ivalue = value};
      if (debug)
        printf("Int at %d was %d\n", location, value);
    } else {
      if (debug)
        printf("End of .data section...\n");
      return;
    }
  }
}

bool parseOperation(uint8_t **bytecodeCursor, const char **code) {
  // identify opcode
  int opcode = -1;
  for (int i = 0; i < NUM_OPCODES; i++) {
    int matchLength = firstTokenMatches(*code, OPCODES[i]);
    if (matchLength) {
      if (debug)
        printf("Found Opcode! %d: %s\n", i, OPCODES[i]);
      opcode = i;
      *code += matchLength;
      break;
    }
  }
  if (opcode == -1) {
    if (debug)
      printf("End of .code section...\n");
    return false;
  }

  **bytecodeCursor = opcode;
  (*bytecodeCursor)++;
  int numArgs = OPCODE_ARGS[opcode];
  if (debug)
    printf("Parsing %d arguments...\n", numArgs);
  for (int i = 0; i < numArgs; i++) {
    parseWhitespace(code);
    **bytecodeCursor = parseNumber(code);
    if (debug)
      printf("%d [0x%X]\n", **bytecodeCursor, **bytecodeCursor);
    (*bytecodeCursor)++;
  }
  return true;
}

void parseCode(uint8_t **bytecodeCursor, const char **code) {
  if (strncmp(*code, ".code:", 6) != 0) {
    if (debug)
      printf("No .code section found.\n");
    return;
  }
  *code += 6;
  if (debug)
    printf("Found .code section!\n");

  do
    parseWhitespace(code);
  while (parseOperation(bytecodeCursor, code));
}

uint8_t *parseStrToBytecode(VM *vm, const char *code) {
  uint8_t *bytecode = malloc(strlen(code));
  parseWhitespace(&code);
  parseData(vm, &code);
  parseWhitespace(&code);
  uint8_t *cursor = bytecode;
  parseCode(&cursor, &code);
  vm->codeSize = cursor - bytecode;
  if (debug) {
    printf("PRINTING BYTECODE:\n    ");
    for (int i = 0; i < 16; i++) {
      printf("%2X ", i);
    }
    for (int i = 0; i < vm->codeSize; i++) {
      if (i % 16 == 0) {
        printf("\n%02X: ", i);
      }
      printf("%02X ", bytecode[i]);
    }
    printf("\n");
  }
  return bytecode;
}

// === TESTS ===

void testString() {
  const char *code = "\"Hello, world!\\r\\n\\t\\0 \"";
  const char *s = parseString(&code);
  printf("STRING: %s", s);
}

void testToken() {
  const char *code = "PUSHP";
  assert(!firstTokenMatches(code, "PUSH"), "");
  assert(firstTokenMatches(code, "PUSHP"), "");
  printf("PASSED: Test Token\n");
}

void test() {
  testToken();

  const char *code = readFileToStr("bytecode/showcase.lvmasm");
  VM vm;
  initVM(&vm);
  parseStrToBytecode(&vm, code);
}