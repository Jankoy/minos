#include "interpreter.h"

#include "nob.h"

static void do_push(ValueStack * stack, Value v)
{
	nob_da_append(stack, v);
}

static Value do_pop(Instruction instruction, ValueStack * stack)
{
	if (stack->count < 1) {
		reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
		exit(1);
	}
	
	Value v = stack->items[stack->count - 1];
	nob_da_pop(stack);
	return v;
}

static Value do_plus(Instruction instruction, Value a, Value b)
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static Value do_minus(Instruction instruction, Value a, Value b)
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static Value do_multiply(Instruction instruction, Value a, Value b)
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static Value do_divide(Instruction instruction, Value a, Value b)
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static void do_dump(Instruction instruction, Value v)
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
		reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}
}

static Value do_equal(Instruction instruction, Value a, Value b)
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
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
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static void do_if(Value value, size_t end_index, size_t * ip)
{
	if (!value.i32) *ip = end_index;
}

static void do_else(size_t end_index, size_t * ip)
{
	*ip = end_index;
}

static void do_end(size_t return_index, size_t * ip)
{
	if (return_index) *ip = return_index;
}

static Value do_gt(Instruction instruction, Value a, Value b)
{
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return _bool(a.i32 > b.i32);
		case U32:
			return _bool(a.i32 > (int32_t)b.u32);
		case F32:
			return _bool(a.i32 > b.f32);
		case BOOL:
			return _bool(a.i32 > b._bool);
		case ERR:
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case U32:
		switch (b.type) {
		case I32:
			return _bool(a.u32 > (uint32_t)b.i32);
		case U32:
			return _bool(a.u32 > b.u32);
		case F32:
			return _bool(a.u32 > b.f32);
		case BOOL:
			return _bool(a.u32 > b._bool);
		case ERR:
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case F32:
		switch (b.type) {
		case I32:
			return _bool(a.f32 > b.i32);
		case U32:
			return _bool(a.f32 > b.u32);
		case F32:
			return _bool(a.f32 > b.f32);
		case BOOL:
			return _bool(a.f32 > b._bool);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case BOOL:
		switch (b.type) {
		case I32:
			return _bool(a._bool > b.i32);
		case U32:
			return _bool(a._bool > b.u32);
		case F32:
			return _bool(a._bool > b.f32);
		case BOOL:
			return _bool(a._bool > b._bool);
		case ERR:
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static void do_dup(Value value, ValueStack * stack)
{
	do_push(stack, value);
	do_push(stack, value);
}

static void do_while()
{
	
}

static void do_do(Value value, size_t end_index, size_t * ip)
{
	if (!value.i32) *ip = end_index;
}

static Value do_lt(Instruction instruction, Value a, Value b)
{
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return _bool(a.i32 < b.i32);
		case U32:
			return _bool(a.i32 < (int32_t)b.u32);
		case F32:
			return _bool(a.i32 < b.f32);
		case BOOL:
			return _bool(a.i32 < b._bool);
		case ERR:
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case U32:
		switch (b.type) {
		case I32:
			return _bool(a.u32 < (uint32_t)b.i32);
		case U32:
			return _bool(a.u32 < b.u32);
		case F32:
			return _bool(a.u32 < b.f32);
		case BOOL:
			return _bool(a.u32 < b._bool);
		case ERR:
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case F32:
		switch (b.type) {
		case I32:
			return _bool(a.f32 < b.i32);
		case U32:
			return _bool(a.f32 < b.u32);
		case F32:
			return _bool(a.f32 < b.f32);
		case BOOL:
			return _bool(a.f32 < b._bool);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case BOOL:
		switch (b.type) {
		case I32:
			return _bool(a._bool < b.i32);
		case U32:
			return _bool(a._bool < b.u32);
		case F32:
			return _bool(a._bool < b.f32);
		case BOOL:
			return _bool(a._bool < b._bool);
		case ERR:
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	case ERR:
		reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
		exit(1);
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static void interpretInstruction(ValueStack * stack, Instruction instruction, size_t * ip)
{
	size_t orig_ip = *ip;
	switch (instruction.token.type) {
		case TOK_PUSH:
			do_push(stack, instruction.value);
			break;
		case TOK_PLUS:
			do_push(stack, do_plus(instruction, do_pop(instruction, stack), do_pop(instruction, stack)));
			break;
		case TOK_MINUS:
			do_push(stack, do_minus(instruction, do_pop(instruction, stack), do_pop(instruction, stack)));
			break;
		case TOK_MULTIPLY:
			do_push(stack, do_multiply(instruction, do_pop(instruction, stack), do_pop(instruction, stack)));
			break;
		case TOK_DIVIDE:
			do_push(stack, do_divide(instruction, do_pop(instruction, stack), do_pop(instruction, stack)));
			break;
		case TOK_DUMP:
			do_dump(instruction, do_pop(instruction, stack));
			break;
		case TOK_EQUAL:
			do_push(stack, do_equal(instruction, do_pop(instruction, stack), do_pop(instruction, stack)));
			break;
		case TOK_IF:
			do_if(do_pop(instruction, stack), instruction.value.u32, ip);
			break;
		case TOK_ELSE:
			do_else(instruction.value.u32, ip);
			break;
		case TOK_END:
			do_end(instruction.value.u32, ip);
			break;
		case TOK_DUP:
			do_dup(do_pop(instruction, stack), stack);
			break;
		case TOK_GT:
			do_push(stack, do_gt(instruction, do_pop(instruction, stack), do_pop(instruction, stack)));
			break;
		case TOK_WHILE:
			do_while();
			break;
		case TOK_DO:
			do_do(do_pop(instruction, stack), instruction.value.u32, ip);
			break;
		case TOK_LT:
			do_push(stack, do_lt(instruction, do_pop(instruction, stack), do_pop(instruction, stack)));
			break;
		default:
			assert(false && "Unreachable");
			break;
	}
	if (*ip == orig_ip) *ip += 1;
}

void interpretProgram(InstructionArray * instructions)
{
	ValueStack stack = {0};
	size_t ip = 0;
	while (ip < instructions->count) {
		interpretInstruction(&stack, instructions->items[ip], &ip);
	}
	nob_da_free(stack);
}
