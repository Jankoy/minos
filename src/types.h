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
	TOK_END
} TokenType;

typedef struct {
	const char * filepath;
	size_t line_num;
	size_t col_num;
	TokenType type;
} Token;

typedef enum {
	ERR = 0,
	I32,
	U32,
	F32,
	BOOL
} ValueType;

typedef struct {
	union {
		int32_t i32;
		uint32_t u32;
		float f32;
		bool _bool;
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

Value i32(int32_t n);
Value u32(uint32_t n);
Value f32(float n);
Value _bool(bool b);

Token tok(const char * filepath, size_t line_num, size_t col_num, TokenType type);

void reportError(const char * filepath, size_t line_num, size_t column_num, const char * error);

#endif // _TYPES_H
