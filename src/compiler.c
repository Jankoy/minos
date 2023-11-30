#include "compiler.h"

#include "nob.h"

static size_t strip_ext(char *fname)
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

static void compileInstruction(size_t * stack_count, size_t ip, Instruction instruction, FILE * out)
{
	fprintf(out, ".INSTRUCTION_%zu:\n", ip);
	switch (instruction.token.type) {
	case TOK_PUSH:
		switch (instruction.value.type) {
		case I32:
			fprintf(out, "    push    %d\n", instruction.value.i32);
			break;
		case U32:
			fprintf(out, "    push    %u\n", instruction.value.u32);
			break;
		case F32:
			fprintf(out, "    push    %f\n", instruction.value.f32);
			break;
		case BOOL:
			fprintf(out, "    push    %d\n", instruction.value._bool);
			break;
		case ERR:
			assert(false && "Unreachable");
			break;
		default:
			assert(false && "Unreachable");
			break;
		}
		*stack_count += 1;
		break;
	case TOK_PLUS:
		if (*stack_count < 2) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		*stack_count -= 2;
		fprintf(out, "    add     rax, rbx\n");
		fprintf(out, "    push    rax\n");
		*stack_count += 1;
		break;
	case TOK_MINUS:
		if (*stack_count < 2) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		*stack_count -= 2;
		fprintf(out, "    sub     rax, rbx\n");
		fprintf(out, "    push    rax\n");
		*stack_count += 1;
		break;
	case TOK_MULTIPLY:
		if (*stack_count < 2) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		*stack_count -= 2;
		fprintf(out, "    mul     rbx\n");
		fprintf(out, "    push    rax\n");
		*stack_count += 1;
		break;
	case TOK_DIVIDE:
		if (*stack_count < 2) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		*stack_count -= 2;
		fprintf(out, "    div     rbx\n");
		fprintf(out, "    push    rax\n");
		*stack_count += 1;
		break;
	case TOK_DUMP:
		if (*stack_count < 1) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rdi\n");
		*stack_count -= 1;
		fprintf(out, "    call    dump\n");
		break;
	case TOK_EQUAL:
		fprintf(out, "    mov     rcx, 0\n");
		fprintf(out, "    mov     rdx, 1\n");
		if (*stack_count < 2) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		*stack_count -= 2;
		fprintf(out, "    cmp     rax, rbx\n");
		fprintf(out, "    cmove   rcx, rdx\n");
		fprintf(out, "    push    rcx\n");
		*stack_count += 1;
		break;
	case TOK_IF:
		if (*stack_count < 1) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rax\n");
		*stack_count -= 1;
		fprintf(out, "    test    rax, rax\n");
		fprintf(out, "    jz      .INSTRUCTION_%u\n", instruction.value.u32);
		break;
	case TOK_ELSE:
		fprintf(out, "    jmp     .INSTRUCTION_%u\n", instruction.value.u32);
		break;
	case TOK_END:
		if (instruction.value.u32 && instruction.value.u32 < ip + 1)
			fprintf(out, "    jmp     .INSTRUCTION_%u\n", instruction.value.u32);
		break;
	case TOK_DUP:
		if (*stack_count < 1) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rax\n");
		*stack_count -= 1;
		fprintf(out, "    push    rax\n");
		fprintf(out, "    push    rax\n");
		*stack_count += 2;
		break;
	case TOK_GT:
		fprintf(out, "    mov     rcx, 0\n");
		fprintf(out, "    mov     rdx, 1\n");
		if (*stack_count < 2) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		*stack_count -= 2;
		fprintf(out, "    cmp     rax, rbx\n");
		fprintf(out, "    cmovg   rcx, rdx\n");
		fprintf(out, "    push    rcx\n");
		*stack_count += 1;
		break;
	case TOK_WHILE:
		break;
	case TOK_DO:
		if (*stack_count < 1) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rax\n");
		*stack_count -= 1;
		fprintf(out, "    test    rax, rax\n");
		fprintf(out, "    jz      .INSTRUCTION_%u\n", instruction.value.u32);
		break;
	case TOK_LT:
		fprintf(out, "    mov     rcx, 0\n");
		fprintf(out, "    mov     rdx, 1\n");
		if (*stack_count < 2) {
			reportError(instruction.token.filepath, instruction.token.line_num, instruction.token.col_num, "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty");
			exit(1);
		}
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		*stack_count -= 2;
		fprintf(out, "    cmp     rax, rbx\n");
		fprintf(out, "    cmovl   rcx, rdx\n");
		fprintf(out, "    push    rcx\n");
		*stack_count += 1;
		break;
	default:
		assert(false && "Unreachable");
		break;
	}
}

static void write_dump_function(FILE * out)
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
	char out_filepath[32] = {0};
	
	memcpy(out_filepath, filepath, strlen(filepath));
	strip_ext(out_filepath);		
	
	FILE * out = fopen("tmp.asm", "w");
	fprintf(out, "segment .text\n");
	fprintf(out, "\n");
	write_dump_function(out);
	fprintf(out, "\n");
	fprintf(out, "global _start\n");
	fprintf(out, "_start:\n");
	size_t stack_count = 0;
	for (size_t i = 0; i < instructions->count; i++) {
		compileInstruction(&stack_count, i, instructions->items[i], out);
	}
	fprintf(out, ".INSTRUCTION_%zu:\n", instructions->count);
	fprintf(out, ".EXIT:\n");
	fprintf(out, "    mov     rax, 60\n");
	fprintf(out, "    mov     rdi, 0\n");
	fprintf(out, "    syscall\n");
	fclose(out);
	
	Nob_Cmd cmd = {0};
	nob_cmd_append(&cmd, "nasm");
	nob_cmd_append(&cmd, "-felf64", "tmp.asm");
	if (!nob_cmd_run_sync(cmd)) exit(1);

	cmd.count = 0;
	nob_cmd_append(&cmd, "ld");
	nob_cmd_append(&cmd, "-o", out_filepath, "tmp.o");
	if (!nob_cmd_run_sync(cmd)) exit(1);
}
