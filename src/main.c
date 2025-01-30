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

bool debug = false;

// define any structs
typedef struct {
	int location; // the index in the pool
  int type;
  union {
	  char* Svalue; 
    int Ivalue;
  };
} const_t;

// ask for the stack and stack pointer in case the functions must be used for multiple stacks
int push(int32_t* stack, int* sp, int value) {
	// set our value in the index that the pointer points to
	stack[*sp] = value; 
	// increment the stack pointer to prepare room for the next value
	return (*sp)++;
}

// Pop utility function for the stack
int pop(int32_t* stack, int* sp) {
  if (*sp > 0) {
	  (*sp)--; // decrement the stack pointer
	    return stack[*sp]; // return the stack element pointed to
    }
  else {
    printf("\e[0;31mError: Stack underflow\e[0;37m\n");
    if (debug) {
      for (int i = 0; i < *sp+1; i++) {
        printf("%d ", stack[i]); // Use array indexing
      }
    }
  return 10;
  }
  return 0;
}

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

// execute instructions
void exec(int32_t* stack, int* sp, const_t* pool, int8_t* code, size_t size) {
	int pc = 0; // define our program counter
  if (debug)
    printf("Code is: %hhd, size is %lu\n", *code, sizeof(code));

	// make our loop
	while (pc < size) {
		// match opcodes
		int opcode = code[pc];
    if (debug) {
      printf("Opcode is %d\n", opcode);
      printf("Size is %zu\n", size);
      printf("pc is %d\n", pc);
      printf("sp is %d\n", *sp);
      for (int i = 0; i < *sp; i++) {
        printf("%d ", stack[i]); // Use array indexing
      }
      printf("\n");
    }
    //printf("%d\n", opcode); 
    if (*sp > 256) {
      printf("SP is %d, opcode is %d\n", *sp, opcode);
    }

		switch (opcode) {
			// SPRINT will take 2 bytes for the entry index
			case SPRINT: {
				int b1 = code[++pc];
				int b2 = code[++pc];

				// construct the index
				int idx = (b1 << 8) | b2;
				// get the constant from the pool using the index
				const_t entry = pool[idx];
				printf("%s\n", entry.Svalue);
				pc++; // advance pc to the next instruction
				break;
			}
      case DPRINT: {
        // Handle DPRINT instruction
        int b1 = code[++pc];
				int b2 = code[++pc];

				// construct the index
				int idx = (b1 << 8) | b2;
				// get the constant from the pool using the index
				const_t entry = pool[idx];

        if (debug)
          printf("DPRINT ran");
        if (entry.type != 1) {
          printf("\e[0;31mType mismatch! Please use SPrint for this.\e[0;37m\n");
        }

				printf("%d\n", entry.Ivalue);
				pc++; // advance pc to the next instruction
        break;
      }
      case DPRINTST: {
				printf("%d\n", pop(stack, sp));
				pc++; // advance pc to the next instruction
        break;
      }
      case PUSH: {
        int b1 = code[++pc];
        push(stack, sp, b1);
        //sp++;
				pc++; // advance pc to the next instruction
        break;
      }
      case PUSHP: {
				int b1 = code[++pc];
				int b2 = code[++pc];

				// construct the index
				int idx = (b1 << 8) | b2;
				// get the constant from the pool using the index
				const_t entry = pool[idx];
        if (debug)
          printf("DPRINT ran");
        if (entry.type != 1) {
          printf("\e[0;31mType mismatch! Please use SPrint for this.\e[0;37m\n");
        }
        push(stack, sp, entry.Ivalue);

				pc++; // advance pc to the next instruction
				break;
			}

      case POP: {
        // Handle POP instruction
        if (debug) {
          printf("Stack is %d, sp is %d\n", *stack, *sp);
        }
        int a = pop(stack, sp);
        if (debug)
          printf("After POP: sp = %d\n", *sp);
        pc++;
        break;
      }
      case ADD: {
        // Handle ADD instruction
        int b1 = pop(stack, sp);
        int b2 = pop(stack, sp);
        push(stack, sp, b1 + b2);
        pc++; // advance pc to the next instruction
        break;
      }
      case SUB: {
        // Handle SUB instruction
        int b1 = pop(stack, sp);
        int b2 = pop(stack, sp);
        push(stack, sp, b1 - b2);
				pc++; // advance pc to the next instruction
        break;
      }
      case MUL: {
        // Handle MUL instruction
        int b1 = pop(stack, sp);
        int b2 = pop(stack, sp);
        push(stack, sp, b1 * b2);
				pc++; // advance pc to the next instruction
        break;
      }
      case DIV: {
        // Handle DIV instruction
        int b1 = pop(stack, sp);
        int b2 = pop(stack, sp);
        if (b1 == 0 || b2 == 0) {
          printf("\e[0;31mError on SP %d: Cannot divide by 0. PC is %d\e[0;37m\n", *sp, pc);
          pc++;
          break;
        }
        push(stack, sp, b2 / b1);
				pc++; // advance pc to the next instruction
        break;
      }
      case JMP: {
        // Handle JMP instruction
        int target = code[++pc];
        if (target <= size)
          pc = target;
        else {
          printf("\e[0;31mError: Target out of bounds!\e[0;37m\n");
				  pc++; // advance pc to the next instruction
        }
        break;
      }
      case JNE: { // jump if not equal
        // Handle JNE instruction
        int b1 = pop(stack, sp);
        int b2 = pop(stack, sp);
        int target = code[++pc];
        if (b1 != b2 && target <= size) {
          pc = target;
        } else pc++; // advance pc to the next instruction
        break;
      }
      case JE: { // Jump if equal
        // Handle JE instruction
        int b1 = pop(stack, sp);
        int b2 = pop(stack, sp);
        int target = code[++pc];
        if (b1 == b2 && target <= size) {
          pc = target;
        } else pc++; // advance pc to the next instruction
        break;
      }
      case JG: { // Jump if greater
        // Handle JE instruction
        int b1 = pop(stack, sp);
        int b2 = pop(stack, sp);
        int target = code[++pc];
        if (b1 > b2 && target <= size) {
          pc = target;
        } else pc++; // advance pc to the next instruction
        break;
      }

    	case DUP: {
        int to_DUP = pop(stack, sp);
        push(stack, sp, to_DUP);
        push(stack, sp, to_DUP);
        pc++;
        break;
      }
      case SWAP: {
        int b1 = code[++pc];
        if (debug)
          printf("b1 is %d, SP is %d, sp-b1 is %d, at SP-1 %d\n", b1, *sp, stack[(*sp -1) -b1], stack[*sp-1]);
        if (*sp < 2) {
          printf("\e[0;31mError: Not enough values on stack to swap.\e[0;37m\n");
          return;
        }
        //swap(stack, sp, b1);
        if (b1 < 0) {
          printf("\e[0;31mError: Can't pass a negative number to SWAP\e[0;37m\n");
          printf("to_SWAP is %d\n", b1); 
          return;
        }
        if ((*sp -1) - b1 < 0 || b1 > *sp || b1 >= *sp) {
          printf("\e[0;31mError: SWAP value exceeds the size of the stack.\e[0;37m\n");
          printf("b1 is %d\n", b1); 
          return;
        }
        int top_of_stack = stack[*sp-1];
        int at_switch = stack[(*sp -1) - b1];
        stack[*sp - 1] = at_switch;
        stack[(*sp-1) - b1] = top_of_stack;
        if (debug) {
          printf("Swapped stack[%d] (%d) with stack[%d] (%d)\n",
            *sp - 1, stack[*sp - 1], *sp - 1 - b1, stack[*sp - 1 - b1]);
        }
        pc++;
        break;
      }

     	// finish executing the program
			case END:
				return;
			default:
        printf("\e[0;31mUnrecognized instruction at PC %d\e[0;37m\n", pc);
        pc++; // advance pc to the next instruction
				break;
		}
	}
  if (debug)
    printf("While loop over!\n");
}

int main(int argc, char *argv[]) {
  // define any variables!
  int32_t stack[256];
  int sp = 0;
  const_t pool[128];
  // populate the pool here, this is equivalent to .data in assembly
  pool[0] = (const_t){0, 0, .Svalue = "Welcome to the Lagoon Virtual Machine!"};
  pool[1] = (const_t){1, 0, .Svalue = "Adding numbers!"};
  pool[2] = (const_t){2, 0, .Svalue = "Subtracting numbers!"};
  pool[3] = (const_t){3, 0, .Svalue = "Multiplying numbers!"};
  pool[4] = (const_t){4, 0, .Svalue = "Dividing numbers!"};
  pool[7] = (const_t){7, 0, .Svalue = "It jumped!"};
  pool[5] = (const_t){5, 1, .Ivalue = 214748364};
  pool[6] = (const_t){6, 1, .Ivalue = 5};

  int8_t code[] = {
    // Print greeting
    SPRINT, 0x00, 0x00,        // Print "Welcome to the VM!"

    // Push numbers to stack and print them
    PUSH, 0x0A,                // Push 10 (decimal)
    PUSH, 0x05,                // Push 5 (decimal)
    DPRINTST,                  // Print current stack

    // Add numbers and print result
    SPRINT, 0x00, 0x01,        // Print "Adding numbers!"
    ADD,                       // Add top two numbers on stack
    DPRINTST,                  // Print current stack (result of ADD)

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

  if (debug)
    printf("Size of code is %zu\n", sizeof(code));
  exec(stack, &sp, pool, code3, sizeof(code3));
  return EXIT_SUCCESS;
}
