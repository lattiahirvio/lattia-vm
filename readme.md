# lattia-vm
lattia-vm is a stack-based bytecode virtual machine written in C. The VM supports 16 instructions, which include arithmetic, jumps, stack manipulation, and printing. The VM also has a constant pool, that you can define variables and such in.

## Running the VM
To run the VM, you need to clone the repository with the command `git clone https://github.com/lattiahirvio/lattia-vm.git`. After git clone, you can compile the code by running the build script `./build.sh`, which will build and run the tests for you. After that, you can run the executable with ./lattia-vm. The VM was developed on Linux, and thus has not been tested on Windows.

The VM has 5 arguments as of writing; `--help`, `--version`, `--file`, `--debug`, and `--code`. Help and version are self-explanatory, but the file, debug, and code flags require a bit of explaining. The file flag sets the file path of the file the VM tries to load. Three example files are provided in the `/bytecode/` directory. The code flag lets you provide code to the vm without requiring an external file. The debug flag enables the debug tools the VM has for the bytecode. The debug flag is very helpful when debugging the bytecode you're trying to run...

## Inspiration
The project was inspired by this [stack based bytecode vm tutorial](https://trillium-2.gitbook.io/re-manual/generic/bytecode-vms). The tutorial is incomplete as of writing, but provides a solid base for understanding and writing a virtual machine. I extended the instruction set presented in the tutorial, as it was insufficient for my needs. Along with extending the instruction set, I implemented multiple other features that were missing from the tutorial, such as every instruction other than SPRINT.

Other useful resources for the project were [this thread on CS stackexchange](https://cs.stackexchange.com/questions/64410/how-can-a-stack-based-vm-be-turing-complete), the [Ethereum VM opcode documentation](https://github.com/crytic/evm-opcodes), and Wikipedia page on the [Pushdown automaton](https://en.wikipedia.org/wiki/Pushdown_automaton), and generally wikipedia pages on computing theory.

## Reasoning
The reason I wrote this program is to learn more about how the stack works, and how virtual machines operate. During this project I learned a great deal about C, virtual machine, and computation theory. I am quite sure that as of now, the VM is also insecure, so I plan on writing a VM escape for the VM at some point. 

## Features I plan to implement
The VM is now in a state where I can publish it, but it is nowhere near feature complete. It requires the ability to load bytecode from files, and a way to pass a filepath as a command line argument. Other command line arguments will be added, too. Additionally as I mentioned before, I am quite sure it is possible to write bytecode that escapes the VM, due to the program not checking if you are trying to push out of the stack. In other words, the security on this is not on point. 

## Documentation
Now for the fun part. As mentioned before, the VM supports 16 different instructions, which belong in a few categories. Arithmetic operations supported are `ADD`, `SUB`, `MUL` and `DIV`. The jump instructions are `JMP`(obviously), `JNE`, `JE`, and `JG`. Stack manipulation happens by using `PUSH`, `PUSHP`, `POP`, `DUP` and `SWAP`.

The VM also includes debugging functionality for the VM. This functionality can be enabled by modifying the `debug` boolean to `true` and recompiling the VM. Eventually this functionality will be an argument you can pass to the VM. The debug functionality includes printing out the stack, what instruction was ran, size of the program, where the stack pointer is, and so on.

The VM additionally includes 3 example programs I wrote. At the moment, the VM cannot load code from a file, so the example programs (and all code) written for the VM needs to be coded inside the VM code. In the future I will add a feature to load programs from text files though. 

The programs are well commented, so you should be able to understand how the programs work. In case you just want to see the programs, here they are;

### Example program #1, showcases most instructions
```C
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
```

### Example program #2, count to 10
```C
   PUSH, 0x01, // PUSH, sp = 1, stack[1] = 1
    DUP, // DUP, sp = 2, stack[2] = 1
    DPRINTST, // PRINT, sp = 1, stack[1] = 1
    PUSH, 0x01, // PUSH, sp = 2, stack[2] = 1, we do this because we want to add 1 on every iteration. We JMP here every time a loop begins
    ADD, // ADD, sp = 1, stack[1] = 2
    DUP, // DUP, sp = 2, stack[2] = 2
    DPRINTST, // PRINT, sp = 1, stack[1] = 2
    DUP, // DUP, sp = 2, stack[2] = 2
    PUSH, 10, // PUSH, sp = 3, stack[3] = 10, we push whatever number we want the loop to end on
    JNE, 0x04, // JNE, sp = 1, stack[1] = 2 
    END // Halt the virtual machine (on last iteration, when stack[1] = 10)

```

### Example program #3, fibonacci numbers
```C
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
```
In this last program, we cheat a little bit by just printing the first 3 numbers, but this makes our job easier so I do it this way.

## Instruction set
Here is the full instruction set, with descriptions and argument counts for each instruction. The instruction set could probably be even smaller, but these are the instructions I ended up going with.

| Opcode | Name | Description | Arguments |
|--------|------|-------------|-----------|
| `0x00` | PUSH | Pushes a number to the stack | 1 |
| `0x01` | PUSHP | Pushes a number to the stack from the pool. The arguments are the pool indexes. Generally use `0x00` as the first argument. | 2 |
| `0x02` | POP | Subtracts stack pointer by 1. Technically, the top number on the stack is still there, but will be overwritten when you push something new. | - |
| `0x03` | ADD | Pops the top 2 numbers on the stack, adds them together, and pushes the result to the stack. | - |
| `0x04` | SUB | Pops the top 2 numbers on the stack, subtracts them, and pushes the result to the stack. | - |
| `0x05` | MUL | Pops the top 2 numbers on the stack, multiplies them together, and pushes the result to the stack. | - |
| `0x06` | DIV | Pops the top 2 numbers on the stack, divides them, and pushes the result to the stack. | - |
| `0x07` | JMP | Unconditional jump. The argument is the index of the code the instruction jumps to. | 1 |
| `0x08` | JNE | Conditional negative comparison jump. It compares the top 2 numbers on the stack, and if they are not equal, jumps to the address specified in the argument. | 1 |
| `0x09` | JE | Conditional positive comparison jump. It compares the top 2 numbers on the stack, and if they are equal, jumps to the address specified in the argument. | 1 |
| `0x0a` | JG | Conditional greater-than comparison jump. It compares the top 2 numbers on the stack, and if the number on top of the stack is greater than the 2nd top number, jumps to the address specified in the argument. | 1 |
| `0x0b` | DPRINT | Prints a number from the constant pool. The argument logic works the same as with PUSHP. Make sure the constant you are trying to print is of type 1. | 2 |
| `0x0c` | SPRINT | Prints a string from the constant pool. The argument logic works the same as with PUSHP. Make sure the constant you are trying to print is of type 0. | 2 |
| `0x0d` | DPRINTST | Pops the top number from the stack and prints it. | - |
| `0x0e` | DUP | Duplicates the number that is at the top of the stack. | - |
| `0x0f` | SWAP | Swaps the number on top of the stack with the number at the offset specified in the argument. For example, `SWAP 0x01` swaps the top 2 numbers with each other. | 1 |
| `0x10` | END | Terminates the program. | - |
