#include "types.h"

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
