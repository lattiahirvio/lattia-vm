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

#include "utils.h"
#include "vm.h"
#include <argp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

bool debug = false;
const char *version = "lattia-vm 1.0";
static char doc[] = "A Simple stack based bytecode virtual machine.";
static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},       //
    {"version", no_argument, 0, 'v'},    //
    {"file", required_argument, 0, 'f'}, //
    {"debug", no_argument, 0, 'd'},      //
    {"code", required_argument, 0, 'c'}, //
    {0, 0, 0, 0},                        //
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
  char *file_path = NULL;
  char *lvasm = NULL;

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
      lvasm = optarg;
      break;
    default:
      fprintf(stderr, "Unknown option. Use --help for usage.\n");
      exit(1);
    }
  }
  test();
  return 0;

  if (file_path) {
    lvasm = readFileToStr(file_path);
    if (!lvasm) {
      fprintf(stderr, "Error: Failed to load bytecode from file.\n");
      return EXIT_FAILURE;
    }
  }

  if (!lvasm) {
    fprintf(stderr, "Error: Please specify code with -f or -c.\n");
    return EXIT_FAILURE;
  }

  uint8_t *bytecode;
  if (lvasm) {
    bytecode = parseStrToBytecode(&vm, lvasm, strlen(lvasm));
    if (debug)
      printf("Size of code is %d\n", vm.codeSize);

    exec(&vm, bytecode, vm.codeSize);
    free(bytecode);
    return EXIT_SUCCESS;
  }
  free(lvasm);
  return EXIT_SUCCESS;
}
