# Minos

Uses a modified version of the nob build system/library/framework by [Tsoding](https://github.com/tsoding/)

## Build

### Dependencies: sudo apt install nasm

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
420 = .
```

In the above program the numbers 34 and 35 are pushed onto the stack before being popped off and added together, the sum is then pushed back onto the stack.
The '.' is the dump instruction, it pops the top item off of the stack and displays it to the screen.
The next line pushes the numbers 500 and 80 to the stack before subtracting them and then dumping the difference to the screen.
The third line pushes 420 to the stack then pops two items off of the stack and compares them, then dumps them.

Currently Minos has 14 valid commands:
	push (Any number literal),
	add (+),
	subtract (-),
	multiply (*),
	divide (/),
	dump (.), 
	equal (=),
	if,
	else,
	end,
	duplicate (dup),
	greater than (>),
	while, 
	do,
	and less than (<).

An example of if-else statements:

```minos
34 35 + 69 = if
	1 if
		69 .
	else
		42 .
	end
else
	420 .
end
```

The code above will dump 69 to the output, something to be aware of, if statements in Minos are syntactically 'reversed' from standard if statements.
The conditions are generally before the if statement and not after, considering they have to be pushed onto the stack for the if to pop them off.

There are also while loops which follow a similar principle.

```minos
5
while dup 0 > do
	dup .
	1 -
end
```
=======
