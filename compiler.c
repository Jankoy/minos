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

static void compileInstruction(size_t * label_count, Instruction instruction, FILE * out)
{
	switch (instruction.type) {
	case TOK_PUSH:
		fprintf(out, "    push    %d\n", instruction.value.i32);
		break;
	case TOK_PLUS:
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    add     rax, rbx\n");
		fprintf(out, "    push    rax\n");
		break;
	case TOK_MINUS:
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    sub     rax, rbx\n");
		fprintf(out, "    push    rax\n");
		break;
	case TOK_MULTIPLY:
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    mul     rbx\n");
		fprintf(out, "    push    rax\n");
		break;
	case TOK_DIVIDE:
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    div     rbx\n");
		fprintf(out, "    push    rax\n");
		break;
	case TOK_DUMP:
		fprintf(out, "    pop     rdi\n");
		fprintf(out, "    call    dump\n");
		break;
	case TOK_EQUAL:
		fprintf(out, "    mov     rdx, 1\n");
		fprintf(out, "    mov     rcx, 0\n");
		fprintf(out, "    pop     rbx\n");
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    cmp     rax, rbx\n");
		fprintf(out, "    cmove   rcx, rdx\n");
		fprintf(out, "    push    rcx\n");
		break;
	case TOK_IF:
		fprintf(out, "    pop     rax\n");
		fprintf(out, "    test    rax, rax\n");
		fprintf(out, "    jz      .L%zu\n", *label_count);
		break;
	case TOK_ELSE:
		assert(false && "Not implemented");
		break;
	case TOK_END:
		fprintf(out, ".L%zu:\n", *label_count++);
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
	size_t label_count = 0;
	for (size_t i = 0; i < instructions->count; i++) {
		compileInstruction(&label_count, instructions->items[i], out);
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
