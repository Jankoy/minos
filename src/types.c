#include "types.h"

#include "nob.h"

Value i32(int32_t n)
{
	Value v = {0};
	v.type = I32;
	v.i32 = n;
	return v;
}

Value u32(uint32_t n)
{
	Value v = {0};
	v.type = U32;
	v.u32 = n;
	return v;
}

Value f32(float n)
{
	Value v = {0};
	v.type = F32;
	v.f32 = n;
	return v;
}

Value _bool(bool b)
{
	Value v = {0};
	v.type = BOOL;
	v._bool = b;
	return v;
}

Token tok(const char * filepath, size_t line_num, size_t col_num, TokenType type)
{
	Token t = {0};
	t.filepath = filepath;
	t.line_num = line_num;
	t.col_num = col_num;
	t.type = type;
	return t;
}

void reportError(const char * filepath, size_t line_num, size_t column_num, const char * error)
{
	nob_log(NOB_ERROR, "%s:%zu:%zu: %s", filepath, line_num, column_num, error);
}
