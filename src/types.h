#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
	TOK_PUSH = 0,
	TOK_PLUS,
	TOK_MINUS,
	TOK_MULTIPLY,
	TOK_DIVIDE,
	TOK_DUMP,
	TOK_EQUAL,
	TOK_IF,
	TOK_ELSE,
	TOK_END,
	TOK_DUP,
	TOK_GT,
	TOK_WHILE,
	TOK_DO,
	TOK_LT
} TokenType;

typedef struct {
	const char * filePath;
	size_t lineNum;
	size_t colNum;
	TokenType type;
} Token;

typedef enum {
	I32,
} ValueType;

typedef struct {
	union {
		int32_t i32;
	};
	ValueType type;
} Value;

typedef struct {
	Token token;
	Value value;
} Instruction;

typedef struct {
		Instruction * items;
		size_t count;
		size_t capacity;
} InstructionArray;

typedef struct {
	Value * items;
	size_t count;
	size_t capacity;
} ValueStack;

typedef struct {
	Token * items;
	size_t count;
	size_t capacity;
} TokenStack;

typedef struct {
	size_t * items;
	size_t count;
	size_t capacity;
} IndexStack;

Value i32Value(int32_t n);

Token makeToken(const char * filePath, size_t lineNum, size_t colNum, TokenType type);

#endif // _TYPES_H
