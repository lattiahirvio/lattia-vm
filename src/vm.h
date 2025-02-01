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

#pragma once

#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Define a lot of bytes for the bytecode
#define PUSH 0x00
#define PUSHP 0x01
#define POP 0x02
#define ADD 0x03
#define SUB 0x04
#define MUL 0x05
#define DIV 0x06
#define JMP 0x07
#define JNE 0x08
#define JE 0x09
#define JG 0x0A
#define DPRINT 0x0B
#define SPRINT 0x0C
#define DPRINTST 0x0D
#define DUP 0x0E
#define SWAP 0x0F
#define END 0x10

// define global variables
extern bool debug;

// define any structs
typedef struct {
	int location; // the index in the pool
  int type;
  union {
    char* Svalue; 
    int Ivalue;
  };
} const_t;


typedef struct {
  int32_t stack[256];
  int sp;
  const_t pool[128];
} VM;

// function declarations
void exec(VM* vm, int8_t* code, size_t size);
void initVM(VM* vm);
int push(VM* vm, int value);
int pop(VM* vm);

