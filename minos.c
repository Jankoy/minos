#define NOB_IMPLEMENTATION
#include "nob.h"

#include <stdint.h>
#include <limits.h>

#define STR2INT_SUCCESS        0
#define STR2INT_INCONVERTIBLE  1
#define STR2INT_UNDERFLOW      2
#define STR2INT_OVERFLOW       3

int str2int(int *out, char *s, int base) {
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return STR2INT_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return STR2INT_UNDERFLOW;
    if (*end != '\0')
        return STR2INT_INCONVERTIBLE;
    *out = l;
    return STR2INT_SUCCESS;
}

size_t strip_ext(char *fname)
{
    char *end = fname + strlen(fname);

    while (end > fname && *end != '.' && *end != '\\' && *end != '/') {
        --end;
    }
    if ((end > fname && *end == '.') &&
        (*(end - 1) != '\\' && *(end - 1) != '/')) {
        *end = '\0';
    }

    return end - fname + 1;
}

typedef enum {
	OP_PUSH = 0,
	OP_POP,
	OP_PLUS,
	OP_MINUS,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_DUMP,
	OP_EQUAL
} Operation;

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
	Operation type;
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
} Stack;

Instruction push(Value x)
{
	Instruction i = {0};
	i.type = OP_PUSH;
	i.value = x;
	return i;
}

Instruction pop()
{
	Instruction i = {0};
	i.type = OP_POP;
	return i;
}

Instruction plus()
{
	Instruction i = {0};
	i.type = OP_PLUS;
	return i;
}

Instruction minus()
{
	Instruction i = {0};
	i.type = OP_MINUS;
	return i;
}

Instruction multiply()
{
	Instruction i = {0};
	i.type = OP_MULTIPLY;
	return i;
}

Instruction divide()
{
	Instruction i = {0};
	i.type = OP_DIVIDE;
	return i;
}

Instruction dump()
{
	Instruction i = {0};
	i.type = OP_DUMP;
	return i;
}

Instruction equal()
{
	Instruction i = {0};
	i.type = OP_EQUAL;
	return i;
}

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

void do_push(Stack * stack, Value v)
{
	nob_da_append(stack, v);
}

Value do_pop(Stack * stack)
{
	Value v = stack->items[stack->count - 1];
	nob_da_pop(stack);
	return v;
}

Value do_plus(Value a, Value b)
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

Value do_minus(Value a, Value b)
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

Value do_multiply(Value a, Value b)
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

Value do_divide(Value a, Value b)
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

void do_dump(Value v)
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

Value do_equal(Value a, Value b)
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

void interpretInstruction(Stack * stack, Instruction instruction)
{
	switch (instruction.type) {
		case OP_PUSH:
			do_push(stack, instruction.value);
			break;
		case OP_POP:
			do_pop(stack);
			break;
		case OP_PLUS:
			do_push(stack, do_plus(do_pop(stack), do_pop(stack)));
			break;
		case OP_MINUS:
			do_push(stack, do_minus(do_pop(stack), do_pop(stack)));
			break;
		case OP_MULTIPLY:
			do_push(stack, do_multiply(do_pop(stack), do_pop(stack)));
			break;
		case OP_DIVIDE:
			do_push(stack, do_divide(do_pop(stack), do_pop(stack)));
			break;
		case OP_DUMP:
			do_dump(do_pop(stack));
			break;
		case OP_EQUAL:
			do_push(stack, do_equal(do_pop(stack), do_pop(stack)));
			break;
		default:
			assert(false && "Unreachable");
			break;
	}
}

void interpretProgram(InstructionArray * instructions)
{
	Stack stack = {0};
	for (size_t i = 0; i < instructions->count; i++) {
		interpretInstruction(&stack, instructions->items[i]);
	}
	nob_da_free(stack);
}

void compileInstruction(Instruction instruction, FILE * out)
{
	switch (instruction.type) {
	case OP_PUSH:
		fprintf(out, "    push    %d\n", instruction.value.i32);
		break;
	case OP_POP:
		assert(false && "Not implemented");
		break;
	case OP_PLUS:
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    add     rax, rbx\n");
		fprintf(out, "    push    rax\n");
		break;
	case OP_MINUS:
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    sub     rax, rbx\n");
		fprintf(out, "    push    rax\n");
		break;
	case OP_MULTIPLY:
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    mul     rbx\n");
		fprintf(out, "    push    rax\n");
		break;
	case OP_DIVIDE:
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    div     rbx\n");
		fprintf(out, "    push    rax\n");
		break;
	case OP_DUMP:
		fprintf(out, "    pop     rdi\n");
		fprintf(out, "    call    dump\n");
		break;
	case OP_EQUAL:
		fprintf(out, "    mov     rdx, 1\n");
		fprintf(out, "    mov     rcx, 0\n");
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    cmp     rax, rbx\n");
		fprintf(out, "    cmove   rcx, rdx\n");
		fprintf(out, "    push    rcx\n");
		break;
	default:
		assert(false && "Unreachable");
		break;
	}
}

void write_dump_function(FILE * out)
{
	fprintf(out, "dump:\n");
	fprintf(out, "    mov     r9, -3689348814741910323\n");
	fprintf(out, "    sub     rsp, 40\n");
	fprintf(out, "    mov     BYTE [rsp+31], 10\n");
	fprintf(out, "    lea     rcx, [rsp+30]\n");
	fprintf(out, ".L2:\n");
	fprintf(out, "    mov     rax, rdi\n");
	fprintf(out, "    lea     r8, [rsp+32]\n");
	fprintf(out, "    mul     r9\n");
	fprintf(out, "    mov     rax, rdi\n");
	fprintf(out, "    sub     r8, rcx\n");
	fprintf(out, "    shr     rdx, 3\n");
	fprintf(out, "    lea     rsi, [rdx+rdx*4]\n");
	fprintf(out, "    add     rsi, rsi\n");
	fprintf(out, "    sub     rax, rsi\n");
	fprintf(out, "    add     eax, 48\n");
	fprintf(out, "    mov     BYTE [rcx], al\n");
	fprintf(out, "    mov     rax, rdi\n");
	fprintf(out, "    mov     rdi, rdx\n");
	fprintf(out, "    mov     rdx, rcx\n");
	fprintf(out, "    sub     rcx, 1\n");
	fprintf(out, "    cmp     rax, 9\n");
	fprintf(out, "    ja      .L2\n");
	fprintf(out, "    lea     rax, [rsp+32]\n");
	fprintf(out, "    mov     edi, 1\n");
	fprintf(out, "    sub     rdx, rax\n");
	fprintf(out, "    xor     eax, eax\n");
	fprintf(out, "    lea     rsi, [rsp+32+rdx]\n");
	fprintf(out, "    mov     rdx, r8\n");
	fprintf(out, "    mov     rax, 1\n");
	fprintf(out, "    syscall\n");
	fprintf(out, "    add     rsp, 40\n");
	fprintf(out, "    ret\n");
}

void compileProgram(InstructionArray * instructions, const char * filepath)
{
	char nodot[32] = {0};
	char dotasm[32] = {0};
	char doto[32] = {0};
	
	{
		Nob_String_Builder out_filepath_sb = {0};
		
		nob_sb_append_cstr(&out_filepath_sb, filepath);
		nob_sb_append_null(&out_filepath_sb);
		size_t diff = strip_ext(out_filepath_sb.items);
		out_filepath_sb.count -= diff + 2;
		memcpy(nodot, out_filepath_sb.items, strlen(out_filepath_sb.items));
		
		nob_sb_append_cstr(&out_filepath_sb, ".asm");
		nob_sb_append_null(&out_filepath_sb);
		memcpy(dotasm, out_filepath_sb.items, strlen(out_filepath_sb.items));
		
		strip_ext(out_filepath_sb.items);
		out_filepath_sb.count -= 5;
		nob_sb_append_cstr(&out_filepath_sb, ".o");
		nob_sb_append_null(&out_filepath_sb);
		memcpy(doto, out_filepath_sb.items, strlen(out_filepath_sb.items));
		
		nob_sb_free(out_filepath_sb);
	}
	
	FILE * out = fopen(dotasm, "w");
	fprintf(out, "segment .text\n");
	fprintf(out, "\n");
	write_dump_function(out);
	fprintf(out, "\n");
	fprintf(out, "global _start\n");
	fprintf(out, "_start:\n");
	for (size_t i = 0; i < instructions->count; i++) {
		compileInstruction(instructions->items[i], out);
	}
	fprintf(out, "    mov     rax, 60\n");
	fprintf(out, "    mov     rdi, 0\n");
	fprintf(out, "    syscall\n");
	fclose(out);
	
	Nob_Cmd cmd = {0};
	nob_cmd_append(&cmd, "nasm");
	nob_cmd_append(&cmd, "-felf64", dotasm);
	if (!nob_cmd_run_sync(cmd)) exit(1);

	cmd.count = 0;
	nob_cmd_append(&cmd, "ld");
	nob_cmd_append(&cmd, "-o", nodot, doto);
	if (!nob_cmd_run_sync(cmd)) exit(1);
}

void parseError(const char * filepath, size_t line_num, size_t column_num, const char * error)
{
	nob_log(NOB_ERROR, "%s:%zu:%zu: %s", filepath, line_num, column_num, error);
}

bool readInstructionsFromLine(const char * filepath, Nob_String_View line, size_t line_num, InstructionArray * instructions)
{
	bool success = true;
	const char * start = line.data;

	while (line.count > 0 && success) {
		Nob_String_View token = nob_sv_chop_by_space(&line);

		if (nob_sv_eq(token, nob_sv_from_cstr("#"))) {
			break;
		} else if (nob_sv_eq(token, nob_sv_from_cstr("+"))) {
			nob_da_append(instructions, plus());
		} else if (nob_sv_eq(token, nob_sv_from_cstr("-"))) {
			nob_da_append(instructions, minus());
		} else if (nob_sv_eq(token, nob_sv_from_cstr("*"))) {
			nob_da_append(instructions, multiply());
		} else if (nob_sv_eq(token, nob_sv_from_cstr("/"))) {
			nob_da_append(instructions, divide());
		} else if (nob_sv_eq(token, nob_sv_from_cstr("."))) {
			nob_da_append(instructions, dump());
		} else if (nob_sv_eq(token, nob_sv_from_cstr("="))) {
			nob_da_append(instructions, equal());
		} else if (nob_sv_eq(token, nob_sv_from_cstr("true"))) {
			nob_da_append(instructions, push(_bool(true)));
		} else if (nob_sv_eq(token, nob_sv_from_cstr("false"))) {
			nob_da_append(instructions, push(_bool(false)));
		} else if (isdigit((int)*token.data)) {
			Nob_String_Builder token_copy = {0};
			nob_sb_append_buf(&token_copy, token.data, token.count);
			nob_sb_append_null(&token_copy);
			int num = 0;
			int result = str2int(&num, token_copy.items, 10);
			nob_sb_free(token_copy);
			if (result == STR2INT_SUCCESS) {
				nob_da_append(instructions, push(i32(num)));
			} else {
				parseError(filepath, line_num, token.data - start + 1, nob_temp_sprintf("Unable to convert '"SV_Fmt"' into a number", SV_Arg(token)));
				nob_temp_reset();
				success = false;
			}
		} else {
			parseError(filepath, line_num, token.data - start + 1, nob_temp_sprintf("Unrecognized token: '"SV_Fmt"'", SV_Arg(token)));
			nob_temp_reset();
			success = false;
		}
	}

	return success;
}

bool readInstructionsFromFile(const char * filepath, InstructionArray * instructions)
{
	Nob_String_Builder file = {0};
	if (!nob_read_entire_file(filepath, &file)) return false;

	bool success = true;
	
	Nob_String_View content = {
		.data = file.items,
		.count = file.count	
	};
	
	size_t line_num = 1;
	
	content = nob_sv_trim_left(content);
	while (content.count > 0 && success) {
		Nob_String_View line = nob_sv_chop_by_delim(&content, '\n');
		success = readInstructionsFromLine(filepath, line, line_num++, instructions);
		content = nob_sv_trim_left(content);
	}
	
	nob_sb_free(file);
	return success;
}

int main(int argc, char** argv)
{
	const char * program = nob_shift_args(&argc, &argv);
	
	if (argc < 1) {
		nob_log(NOB_INFO, "Usage: %s <run/compile> <args>", program);
		nob_log(NOB_ERROR, "No subcommand is provided");
		return 1;
	}
	const char * subcommand = nob_shift_args(&argc, &argv);

	if (strcmp(subcommand, "run") == 0) {
		if (argc < 1) {
			nob_log(NOB_INFO, "Usage: %s <run/compile> <args>", program);
			nob_log(NOB_ERROR, "No input file path is provided");
			return 1;
		}
		const char * filepath = nob_shift_args(&argc, &argv);

		InstructionArray instructions = {0};
		if (!readInstructionsFromFile(filepath, &instructions)) return 1;
		interpretProgram(&instructions);
		nob_da_free(instructions);
	} else if (strcmp(subcommand, "compile") == 0) {
		if (argc < 1) {
			nob_log(NOB_INFO, "Usage: %s <run/compile> <args>", program);
			nob_log(NOB_ERROR, "No input file path is provided");
			return 1;
		}
		const char * filepath = nob_shift_args(&argc, &argv);

		InstructionArray instructions = {0};
		if (!readInstructionsFromFile(filepath, &instructions)) return 1;
		compileProgram(&instructions, filepath);
		nob_da_free(instructions);
	} else {
		nob_log(NOB_INFO, "Usage: %s <run/compile> <args>", program);
		nob_log(NOB_ERROR, "Invalid subcommand provided");
		return 1;
	} 
	
	return 0;
}
