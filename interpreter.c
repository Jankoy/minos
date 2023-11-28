#include "interpreter.h"

#include "nob.h"

static void do_push(ValueStack * stack, Value v)
{
	nob_da_append(stack, v);
}

static Value do_pop(ValueStack * stack)
{
	Value v = stack->items[stack->count - 1];
	nob_da_pop(stack);
	return v;
}

static Value do_plus(Value a, Value b)
{
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return i32(a.i32 + b.i32);
		case U32:
			return i32(a.i32 + (int32_t)b.u32);
		case F32:
			return i32(a.i32 + (int32_t)b.f32);
		case BOOL:
			return i32(a.i32 + (int32_t)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case U32:
		switch (b.type) {
		case I32:
			return u32(a.u32 + (uint32_t)b.i32);
		case U32:
			return u32(a.u32 + b.u32);
		case F32:
			return u32(a.u32 + (uint32_t)b.f32);
		case BOOL:
			return u32(a.u32 + (uint32_t)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case F32:
		switch (b.type) {
		case I32:
			return f32(a.f32 + (float)b.i32);
		case U32:
			return f32(a.f32 + (float)b.u32);
		case F32:
			return f32(a.f32 + b.f32);
		case BOOL:
			return f32(a.f32 + (float)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case BOOL:
		switch (b.type) {
		case I32:
			return _bool(a._bool + (bool)b.i32);
		case U32:
			return _bool(a._bool + (bool)b.u32);
		case F32:
			return _bool(a._bool + (bool)b.f32);
		case BOOL:
			return _bool(a._bool + b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static Value do_minus(Value a, Value b)
{
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return i32(a.i32 - b.i32);
		case U32:
			return i32(a.i32 - (int32_t)b.u32);
		case F32:
			return i32(a.i32 - (int32_t)b.f32);
		case BOOL:
			return i32(a.i32 - (int32_t)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case U32:
		switch (b.type) {
		case I32:
			return u32(a.u32 - (uint32_t)b.i32);
		case U32:
			return u32(a.u32 - b.u32);
		case F32:
			return u32(a.u32 - (uint32_t)b.f32);
		case BOOL:
			return u32(a.u32 - (uint32_t)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case F32:
		switch (b.type) {
		case I32:
			return f32(a.f32 - (float)b.i32);
		case U32:
			return f32(a.f32 - (float)b.u32);
		case F32:
			return f32(a.f32 - b.f32);
		case BOOL:
			return f32(a.f32 - (float)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case BOOL:
		switch (b.type) {
		case I32:
			return _bool(a._bool + (bool)b.i32);
		case U32:
			return _bool(a._bool + (bool)b.u32);
		case F32:
			return _bool(a._bool + (bool)b.f32);
		case BOOL:
			return _bool(a._bool + b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static Value do_multiply(Value a, Value b)
{
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return i32(a.i32 * b.i32);
		case U32:
			return i32(a.i32 * (int32_t)b.u32);
		case F32:
			return i32(a.i32 * (int32_t)b.f32);
		case BOOL:
			return i32(a.i32 * (int32_t)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case U32:
		switch (b.type) {
		case I32:
			return u32(a.u32 * (uint32_t)b.i32);
		case U32:
			return u32(a.u32 * b.u32);
		case F32:
			return u32(a.u32 * (uint32_t)b.f32);
		case BOOL:
			return u32(a.u32 * (uint32_t)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case F32:
		switch (b.type) {
		case I32:
			return f32(a.f32 * (float)b.i32);
		case U32:
			return f32(a.f32 * (float)b.u32);
		case F32:
			return f32(a.f32 * b.f32);
		case BOOL:
			return f32(a.f32 * (float)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case BOOL:
		switch (b.type) {
		case I32:
			return _bool(a._bool + (bool)b.i32);
		case U32:
			return _bool(a._bool + (bool)b.u32);
		case F32:
			return _bool(a._bool + (bool)b.f32);
		case BOOL:
			return _bool(a._bool + b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static Value do_divide(Value a, Value b)
{
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return i32(a.i32 / b.i32);
		case U32:
			return i32(a.i32 / (int32_t)b.u32);
		case F32:
			return i32(a.i32 / (int32_t)b.f32);
		case BOOL:
			return i32(a.i32 / (int32_t)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case U32:
		switch (b.type) {
		case I32:
			return u32(a.u32 / (uint32_t)b.i32);
		case U32:
			return u32(a.u32 / b.u32);
		case F32:
			return u32(a.u32 / (uint32_t)b.f32);
		case BOOL:
			return u32(a.u32 / (uint32_t)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case F32:
		switch (b.type) {
		case I32:
			return f32(a.f32 / (float)b.i32);
		case U32:
			return f32(a.f32 / (float)b.u32);
		case F32:
			return f32(a.f32 / b.f32);
		case BOOL:
			return f32(a.f32 / (float)b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case BOOL:
		switch (b.type) {
		case I32:
			return _bool(a._bool + (bool)b.i32);
		case U32:
			return _bool(a._bool + (bool)b.u32);
		case F32:
			return _bool(a._bool + (bool)b.f32);
		case BOOL:
			return _bool(a._bool + b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static void do_dump(Value v)
{
	switch (v.type) {
	case I32:
		printf("%d\n", v.i32);
		break;
	case U32:
		printf("%u\n", v.u32);
		break;
	case F32:
		printf("%f\n", v.f32);
		break;
	case BOOL:
		printf("%u\n", (uint32_t)v._bool);
		break;
	case ERR:
		nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}
}

static Value do_equal(Value a, Value b)
{
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return _bool(a.i32 == b.i32);
		case U32:
			return _bool(a.i32 == (int32_t)b.u32);
		case F32:
			return _bool(a.i32 == b.f32);
		case BOOL:
			return _bool(a.i32 == b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case U32:
		switch (b.type) {
		case I32:
			return _bool(a.u32 == (uint32_t)b.i32);
		case U32:
			return _bool(a.u32 == b.u32);
		case F32:
			return _bool(a.u32 == b.f32);
		case BOOL:
			return _bool(a.u32 == b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case F32:
		switch (b.type) {
		case I32:
			return _bool(a.f32 == b.i32);
		case U32:
			return _bool(a.f32 == b.u32);
		case F32:
			return _bool(a.f32 == b.f32);
		case BOOL:
			return _bool(a.f32 == b._bool);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case BOOL:
		switch (b.type) {
		case I32:
			return _bool(a._bool == b.i32);
		case U32:
			return _bool(a._bool == b.u32);
		case F32:
			return _bool(a._bool == b.f32);
		case BOOL:
			return _bool(a._bool == b._bool);
		case ERR:
			nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		nob_log(NOB_ERROR, "An error happened somewhere somehow, whoops");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static void do_if(ValueStack * stack, size_t end_index, size_t * ip)
{
	Value v = do_pop(stack);
	switch (v.type) {
		case I32:
			if (!v.i32) *ip = end_index;
			break;
		case U32:
			if (!v.u32) *ip = end_index;
			break;
		case F32:
			if (!v.f32) *ip = end_index;
			break;
		case BOOL:
			if (!v._bool) *ip = end_index;
			break;
		default:
			assert(false && "Unreachable");
			break;
	}
}

static void do_else()
{
	assert(false && "Not implemented");
}

static void do_end()
{
	
}

static void interpretInstruction(ValueStack * stack, Instruction instruction, size_t * ip)
{
	switch (instruction.type) {
		case TOK_PUSH:
			do_push(stack, instruction.value);
			break;
		case TOK_PLUS:
			do_push(stack, do_plus(do_pop(stack), do_pop(stack)));
			break;
		case TOK_MINUS:
			do_push(stack, do_minus(do_pop(stack), do_pop(stack)));
			break;
		case TOK_MULTIPLY:
			do_push(stack, do_multiply(do_pop(stack), do_pop(stack)));
			break;
		case TOK_DIVIDE:
			do_push(stack, do_divide(do_pop(stack), do_pop(stack)));
			break;
		case TOK_DUMP:
			do_dump(do_pop(stack));
			break;
		case TOK_EQUAL:
			do_push(stack, do_equal(do_pop(stack), do_pop(stack)));
			break;
		case TOK_IF:
			do_if(stack, instruction.value.u32, ip);
			break;
		case TOK_ELSE:
			do_else();
			break;
		case TOK_END:
			do_end();
			break;
			default:
			assert(false && "Unreachable");
			break;
	}
}

void interpretProgram(InstructionArray * instructions)
{
	ValueStack stack = {0};
	size_t ip = 0;
	for (ip = 0; ip < instructions->count; ip++) {
		interpretInstruction(&stack, instructions->items[ip], &ip);
	}
	nob_da_free(stack);
}
