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

#include "vm.h"

bool debug = false;

void swap(int32_t* stack, int* sp, int to_SWAP) {
  if (to_SWAP < 0) {
    printf("\e[0;31mError: Can't pass a negative number to SWAP\e[0;37m\n");
    printf("to_SWAP is %d\n", to_SWAP); 
    return;
  }
  if (*sp - to_SWAP < 0) {
    printf("\e[0;31mError: SWAP value exceeds the size of the stack.\e[0;37m\n");
    printf("to_SWAP is %d\n", to_SWAP); 
    return;
  }
  int top_of_stack = stack[*sp];
  int at_switch = stack[*sp - to_SWAP];
  
  stack[*sp] = at_switch;
  stack[*sp - to_SWAP] = top_of_stack;
}

int main(int argc, char *argv[]) {
  // define our vm
  VM vm;
  // initialize vm
  initVM(&vm);

  // populate the pool here, this is equivalent to .data in assembly
  vm.pool[0] = (const_t){0, 0, .Svalue = "Welcome to the Virtual Machine!"};
  vm.pool[1] = (const_t){1, 0, .Svalue = "Adding numbers!"};
  vm.pool[2] = (const_t){2, 0, .Svalue = "Subtracting numbers!"};
  vm.pool[3] = (const_t){3, 0, .Svalue = "Multiplying numbers!"};
  vm.pool[4] = (const_t){4, 0, .Svalue = "Dividing numbers!"};
  vm.pool[7] = (const_t){7, 0, .Svalue = "It jumped!"};
  vm.pool[5] = (const_t){5, 1, .Ivalue = 214748364};
  vm.pool[6] = (const_t){6, 1, .Ivalue = 5};

  int8_t code[] = {
    // Print greeting
    SPRINT, 0x00, 0x00,        // Print "Welcome to the VM!"

    // Push numbers to stack and print them
    PUSH, 0x0A,                // Push 10 (decimal)
    PUSH, 0x05,                // Push 5 (decimal)
    DPRINTST,                  // Print top of stack

    // Add numbers and print result
    SPRINT, 0x00, 0x01,        // Print "Adding numbers!"
    ADD,                       // Add top two numbers on stack
    DPRINTST,                  // Print top of stack (result of ADD)

    // Subtract numbers and print result
    PUSH, 0x08,                // Push 8
    PUSH, 0xF,                 // Push 8
    SPRINT, 0x00, 0x02,        // Print "Subtracting numbers!"
    SUB,                       // Subtract top two numbers
    DPRINTST,                  // Print current stack (result of SUB)

    // Multiply numbers and print result
    PUSH, 0x03,                // Push 3
    PUSH, 0x04,                // Push 4
    SPRINT, 0x00, 0x03,        // Print "Multiplying numbers!"
    MUL,                       // Multiply top two numbers
    DPRINTST,                  // Print current stack (result of MUL)

    // Divide numbers and print result
    PUSH, 0x02,                // Push 2
    PUSH, 0xB,                // Push 2
    SPRINT, 0x00, 0x04,        // Print "Dividing numbers!"
    DIV,                       // Divide top two numbers
    DPRINTST,                  // Print current stack (result of DIV)

    // Conditional jump example
    PUSH, 0x01,                // Push 1 (true condition)
    PUSH, 0x01,                // Push 1
    JE, 0x32,            // Jump to offset 0x11 if equal
    SPRINT, 0x00, 0x07,        // Print "It jumped!" (skipped if jumped)

    // End the program
    END                        // Halt the virtual machine
  };

  int8_t code2[] = {
    PUSH, 0x01, // PUSH, sp = 1, stack[1] = 1
    DUP, // DUP, sp = 2, stack[2] = 1
    DPRINTST, // PRINT, sp = 1, stack[1] = 1
    PUSH, 0x01, // PUSH, sp = 2, stack[2] = 1, we do this because we want to add 1 on every iteration. We JMP here every time a loop begins
    ADD, // ADD, sp = 1, stack[1] = 2
    DUP, // DUP, sp = 2, stack[2] = 2
    DPRINTST, // PRINT, sp = 1, stack[1] = 2
    DUP, // DUP, sp = 2, stack[2] = 2
    PUSH, 10, // PUSH, sp = 3, stack[3] = 10, we push whatever number we want the loop to end on
    JNE, 0x04, // JNE, sp = 1, stack[1] = 2 (first iteration)
    END // Halt the virtual machine (on last iteration, when stack[1] = 10)
  };

  int8_t code3[] = {
    PUSH, 0x00, // stack: 0
    DPRINTST, // stack: stack empty
    PUSH, 0x01, // stack: 1
    DUP, DPRINTST, // stack: 1
    DUP, DPRINTST, // stack: 1
    DUP, DUP, // stack:  1 1 1
    ADD, // stack: 1 2
    DUP, DPRINTST, // stack: 1 2, 2nd iteration we JMP here
    DUP, // stack:1 2 2, 2nd iteration 2 3 3
    SWAP, 0x02, // stack: 2 2 1, 2nd iteration 3 2 3 
    ADD, //stack: 2 3, 2nd iteration  3 5
    DUP, // stack: 2 3 3, 2nd iteration 3 5 5
    PUSHP, 0x00, 0x05, // stack: 2 3 3 , 2nd iteration 3 5 5 0xFF 
    JG, 0x0C, // stack: 2 3 214748364, 2nd iteration 3 5 214748364
    END, // Halt the Virtual Machine, when the fibonacci number is greater than 214748364
  };

  int8_t code4[] = {
    PUSH, 0x00,
    JMP, 0x00,
    SPRINT, 0x00, 0x00,
    END
  };

  if (debug)
    printf("Size of code is %zu\n", sizeof(code));
  exec(&vm, code, sizeof(code));
  return EXIT_SUCCESS;
}
