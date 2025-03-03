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

#include <string.h>
#include "vm.h"

void initVM(VM* vm) {
  vm->sp = 0;
  memset(vm->stack, 0, sizeof(vm->stack));
  memset(vm->pool, 0, sizeof(vm->pool));
}

// ask for the stack and stack pointer in case the functions must be used for multiple stacks
int push(VM* vm, int value) {
	// set our value in the index that the pointer points to
	vm->stack[vm->sp] = value; 
	// increment the stack pointer to prepare room for the next value
	return vm->sp++;
}

// Pop utility function for the stack
int pop(VM* vm) {
  if (vm->sp > 0) {
	  (vm->sp)--; // decrement the stack pointer
	    return vm->stack[vm->sp]; // return the stack element pointed to
    }
  else {
    printf("\e[0;31mError: Stack underflow\e[0;37m\n");
    if (debug) {
      for (int i = 0; i < vm->sp+1; i++) {
        printf("%d ", vm->stack[i]); // Use array indexing
      }
    }
    return 0;
  }
  return 0;
}

void swap(VM* vm, int to_SWAP) {
  if (to_SWAP < 0) {
    printf("\e[0;31mError: Can't pass a negative number to SWAP\e[0;37m\n");
    printf("to_SWAP is %d\n", to_SWAP); 
    return;
  }
  if (vm->sp - to_SWAP < 0) {
    printf("\e[0;31mError: SWAP value exceeds the size of the stack.\e[0;37m\n");
    printf("to_SWAP is %d\n", to_SWAP); 
    return;
  }
  int top_of_stack = vm->stack[vm->sp];
  int at_switch = vm->stack[vm->sp - to_SWAP];
  
  vm->stack[vm->sp] = at_switch;
  vm->stack[vm->sp - to_SWAP] = top_of_stack;
}

// execute instructions
void exec(VM* vm, uint8_t* code, size_t size) {
	int pc = 0; // define our program counter
  if (debug)
    printf("Code is: %hhd, size is %lu\n", *code, sizeof(code));

	// make our loop
	while (pc <= size) {
		// match opcodes
		int opcode = code[pc];
    if (debug) {
      printf("Opcode is %d\n", opcode);
      printf("Size is %zu\n", size);
      printf("pc is %d\n", pc);
      printf("sp is %d\n", vm->sp);
      for (int i = 0; i < vm->sp; i++) {
        printf("%d ", vm->stack[i]); // Use array indexing
      }
      printf("\n");
    }
    //printf("%d\n", opcode); 
    if (vm->sp > 256) {
      printf("SP is %d, opcode is %d\n", vm->sp, opcode);
    }

		switch (opcode) {
			// SPRINT will take 2 bytes for the entry index
			case SPRINT: {
				int b1 = code[++pc];
				int b2 = code[++pc];

				// construct the index
				int idx = (b1 << 8) | b2;
				// get the constant from the pool using the index
				const_t entry = vm->pool[idx];
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
				const_t entry = vm->pool[idx];

        if (debug)
          printf("DPRINT ran\n");
        if (entry.type != 1) {
          printf("\e[0;31mType mismatch! Please use SPrint for this.\e[0;37m\n");
          printf("Location: %d\n", entry.location);
          printf("Type: %d\n", entry.type);
          printf("value: %d\n", entry.Ivalue);
        }

				printf("%d\n", entry.Ivalue);
				pc++; // advance pc to the next instruction
        break;
      }
      case DPRINTST: {
				printf("%d\n", pop(vm));
				pc++; // advance pc to the next instruction
        break;
      }
      case PUSH: {
        int b1 = code[++pc];
        push(vm, b1);
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
				const_t entry = vm->pool[idx];
        if (debug)
          printf("DPRINT ran");
        if (entry.type != 1) {
          printf("\e[0;31mType mismatch! Please use SPrint for this.\e[0;37m\n");
          printf("Location: %d\n", entry.location);
          printf("Type: %d\n", entry.type);
          printf("value: %d\n", entry.Ivalue);
        }
        push(vm, entry.Ivalue);

				pc++; // advance pc to the next instruction
				break;
			}

      case POP: {
        // Handle POP instruction
        if (debug) {
          printf("Stack is %d, sp is %d\n", vm->stack, vm->sp);
        }
        int a = pop(vm);
        if (debug)
          printf("After POP: sp = %d\n", vm->sp);
        pc++;
        break;
      }
      case ADD: {
        // Handle ADD instruction
        int b1 = pop(vm);
        int b2 = pop(vm);
        push(vm, b1 + b2);
        pc++; // advance pc to the next instruction
        break;
      }
      case SUB: {
        // Handle SUB instruction
        int b1 = pop(vm);
        int b2 = pop(vm);
        push(vm, b1 - b2);
				pc++; // advance pc to the next instruction
        break;
      }
      case MUL: {
        // Handle MUL instruction
        int b1 = pop(vm);
        int b2 = pop(vm);
        push(vm, b1 * b2);
				pc++; // advance pc to the next instruction
        break;
      }
      case DIV: {
        // Handle DIV instruction
        int b1 = pop(vm);
        int b2 = pop(vm);
        if (b1 == 0 || b2 == 0) {
          printf("\e[0;31mError on SP %d: Cannot divide by 0. PC is %d\e[0;37m\n", vm->sp, pc);
          pc++;
          break;
        }
        push(vm, b2 / b1);
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
        int b1 = pop(vm);
        int b2 = pop(vm);
        int target = code[++pc];
        if (b1 != b2 && target <= size && target > 0) {
          pc = target;
        } else pc++; // advance pc to the next instruction
        break;
      }
      case JE: { // Jump if equal
        // Handle JE instruction
        int b1 = pop(vm);
        int b2 = pop(vm);
        int target = code[++pc];
        if (b1 == b2 && target <= size && target > 0) {
          pc = target;
        } else pc++; // advance pc to the next instruction
        break;
      }
      case JG: { // Jump if greater
        // Handle JE instruction
        int b1 = pop(vm);
        int b2 = pop(vm);
        int target = code[++pc];
        if (b1 > b2 && target <= size && target > 0) {
          pc = target;
        } else pc++; // advance pc to the next instruction
        break;
      }

    	case DUP: {
        int to_DUP = pop(vm);
        push(vm, to_DUP);
        push(vm, to_DUP);
        pc++;
        break;
      }
      case SWAP: {
        int b1 = code[++pc];
        if (debug)
          printf("b1 is %d, SP is %d, sp-b1 is %d, at SP-1 %d\n", b1, vm->sp, vm->stack[(vm->sp -1) -b1], vm->stack[vm->sp-1]);
        if (vm->sp < 2) {
          printf("\e[0;31mError: Not enough values on stack to swap.\e[0;37m\n");
          break;
        }
        //swap(stack, sp, b1);
        if (b1 < 0) {
          printf("\e[0;31mError: Can't pass a negative number to SWAP\e[0;37m\n");
          printf("to_SWAP is %d\n", b1); 
          break;
        }
        if ((vm->sp -1) - b1 < 0 || b1 > vm->sp || b1 >= vm->sp) {
          printf("\e[0;31mError: SWAP value exceeds the size of the stack.\e[0;37m\n");
          printf("b1 is %d\n", b1); 
          break;
        }
        int top_of_stack = vm->stack[vm->sp-1];
        int at_switch = vm->stack[(vm->sp -1) - b1];
        vm->stack[vm->sp - 1] = at_switch;
        vm->stack[(vm->sp-1) - b1] = top_of_stack;
        if (debug) {
          printf("Swapped stack[%d] (%d) with stack[%d] (%d)\n",
            vm->sp - 1, vm->stack[vm->sp - 1], vm->sp - 1 - b1, vm->stack[vm->sp - 1 - b1]);
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
