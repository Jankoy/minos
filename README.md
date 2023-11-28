# Minos

Uses a modified version of the nob build system/library/framework by [Tsoding](https://github.com/tsoding/)

## Build

The program can be built by running any C compiler on the nob.c file then running './nob'.

For example:
```bash
cc -o nob nob.c
./nob
```

## Use

You can use the Minos executable in two ways, you can run a .minos file in the interpreter with './minos run file.minos' or you can compile a native linux executable with './minos compile file.minos'.

## Syntax

Minos is a stack-based language like Porth or Forth, you can push numbers to the stack and then perform operations with them.

For example:
```minos
34 35 + .
500 80 - .
```

In the above program the numbers 34 and 35 are pushed onto the stack before being popped off and added together, the sum is then pushed back onto the stack.
The '.' is the dump instruction, it pops the top item off of the stack and displays it to the screen.
The next line pushes the numbers 500 and 80 to the stack before subtracting them and then dumping the difference to the screen.

Currently Minos has 6 valid commands; push (A number literal), add (+), subtract (-), multiply (*), divide (/), and dump (.).
