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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <argp.h>
#include "vm.h"
#include "utils.h"

bool debug = false;
const char *version = "lattia-vm 1.0";
static char doc[] = "A Simple stack based bytecode virtual machine.";
static struct option long_options[] = {
    {"help",    no_argument,       0, 'h'},
    {"version", no_argument,       0, 'v'},
    {"file",    required_argument, 0, 'f'},
    {"debug",   no_argument,       0, 'd'},
    {"code",    required_argument, 0, 'c'},
    {0, 0, 0, 0} 
};

struct arguments {
    enum { CHARACTER_MODE, WORD_MODE, LINE_MODE } mode;
    bool isCaseInsensitive;
};

void print_help() {
    printf("Usage: vm [options]\n");
    printf("  -h, --help       Show this help message\n");
    printf("  -v, --version    Show version info\n");
    printf("  -f, --file FILE  Load a bytecode file\n");
    printf("  -d, --debug      Enable debug mode\n");
    printf("  -c, --code CODE  Execute inline bytecode\n");
    exit(0);
}

int main(int argc, char *argv[]) {
  // define our vm
  VM vm;
  // initialize vm
  initVM(&vm);
  int opt;
  char* file_path = NULL;
  char* inline_code = NULL;

  // get the cmdline flags
  while ((opt = getopt_long(argc, argv, "hvf:dc:", long_options, NULL)) != -1) {
    switch (opt) {
      case 'h':
        print_help();
        break;
      case 'v':
        printf("%s\n", version);
        exit(0);
        break;
      case 'f':
        file_path = optarg;
        break;
     case 'd':
        debug = true;
        break;
     case 'c':
      inline_code = optarg;
      break;
    default:
      fprintf(stderr, "Unknown option. Use --help for usage.\n");
      exit(1);
    }
  }

  char* code = getBytecode(file_path);
  if (!code) {
    fprintf(stderr, "Error: Failed to load bytecode from file.\n");
    return EXIT_FAILURE;
  }
  uint8_t* code2;
  if (inline_code) {
    code2 = parseBytecode(&vm, inline_code, strlen(inline_code));
    if (debug)
      printf("Size of code is %d\n", vm.codeSize);

    exec(&vm, code2, vm.codeSize);
    free(code2);
    return EXIT_SUCCESS;
  } else {
    printf("Size of bytecode string is %lu\n", strlen(code));
    code2 = parseBytecode(&vm, code, strlen(code)); 

    if (debug)
      printf("Size of code is %d\n", vm.codeSize);
    exec(&vm, code2, vm.codeSize);
  }
  free(code);
  free(code2);
  return EXIT_SUCCESS;
}
