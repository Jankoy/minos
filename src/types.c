#include "types.h"

#include "nob.h"

Value i32Value(int32_t n)
{
	Value v = {0};
	v.type = I32;
	v.i32 = n;
	return v;
}

Token makeToken(const char * filePath, size_t lineNum, size_t colNum, TokenType type)
{
	Token t = {0};
	t.filePath = filePath;
	t.lineNum = lineNum;
	t.colNum = colNum;
	t.type = type;
	return t;
}
