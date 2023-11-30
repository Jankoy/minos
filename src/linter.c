#include "linter.h"

#include "nob.h"
#include <limits.h>

#define STR2INT_SUCCESS        0
#define STR2INT_INCONVERTIBLE  1
#define STR2INT_UNDERFLOW      2
#define STR2INT_OVERFLOW       3

static int str2int(int *out, char *s, int base) {
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

static Instruction instruct(const char * filepath, size_t line_num, size_t col_num, TokenType type, Value x)
{
	Instruction i = {0};
	i.token = tok(filepath, line_num, col_num, type);
	i.value = x;
	return i;
}

static bool lintInstructionsFromLine(const char * filepath, IndexStack * stack, Nob_String_View line, size_t line_num, InstructionArray * instructions)
{
	bool success = true;
	const char * start = line.data;

	while (line.count > 0 && success) {
		Nob_String_View token = nob_sv_chop_by_space(&line);
		size_t col_num = token.data - start + 1;
		
		if (token.data[0] == '#') {
			break;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("#"))) {
			break;
		}
		if (isdigit((int)*token.data)) {
			Nob_String_Builder token_copy = {0};
			nob_sb_append_buf(&token_copy, token.data, token.count);
			nob_sb_append_null(&token_copy);
			int num = 0;
			int result = str2int(&num, token_copy.items, 10);
			nob_sb_free(token_copy);
			if (result == STR2INT_SUCCESS) {
				nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_PUSH, i32(num)));
			} else {
				reportError(filepath, line_num, token.data - start + 1, nob_temp_sprintf("Unable to convert '"SV_Fmt"' into a number", SV_Arg(token)));
				nob_temp_reset();
				success = false;
			}
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("true"))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_PUSH, _bool(true)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("false"))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_PUSH, _bool(false)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("+"))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_PLUS, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("-"))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_MINUS, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("*"))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_MULTIPLY, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("/"))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_DIVIDE, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("."))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_DUMP, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("="))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_EQUAL, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("if"))) {
			nob_da_append(stack, instructions->count);
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_IF, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("else"))) {
			if (stack->count > 0) {
				size_t index = stack->items[stack->count - 1];
				if (instructions->items[index].token.type == TOK_IF) {
					nob_da_pop(stack);
					instructions->items[index].value = u32((uint32_t) instructions->count + 1);
					nob_da_append(stack, instructions->count);
				} else {
					reportError(filepath, line_num, col_num, "An 'else' statement without a preceding 'if' statement");
					success = false;
					continue;
				}
			} else {
				reportError(filepath, line_num, col_num, "An 'else' statement without a preceding 'if' statement");
				success = false;
				continue;
			}
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_ELSE, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("end"))) {
			if (stack->count > 0) {
				size_t index = stack->items[stack->count - 1];
				nob_da_pop(stack);
				if (instructions->items[index].token.type == TOK_IF || instructions->items[index].token.type == TOK_ELSE) {
					instructions->items[index].value = u32((uint32_t) instructions->count);
					Value v = u32((uint32_t) instructions->count + 1); // I have to split this up because of pointer incrementation in the nob_da_append macro
					nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_END, v));
					continue;
				} else if (instructions->items[index].token.type == TOK_DO) {
					Value v = u32((uint32_t) instructions->count + 1); // To save the instruction pointer
					nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_END, instructions->items[index].value));
					instructions->items[index].value = v;
					continue;
				} else {
					reportError(filepath, line_num, token.data - start + 1, "An 'end' statement can only close an 'if', 'else', or 'do' statement");
					success = false;
					continue;
				}
			} else {
				reportError(filepath, line_num, token.data - start + 1, "An 'end' statement without a preceding 'if', 'else', or 'do' statement");
				success = false;
				continue;
			}
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("dup"))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_DUP, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr(">"))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_GT, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("while"))) {
			nob_da_append(stack, instructions->count);
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_WHILE, u32(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("do"))) {
			size_t index = stack->items[stack->count - 1];
			nob_da_pop(stack);
			if (instructions->items[index].token.type == TOK_WHILE) {
				nob_da_append(stack, instructions->count);
				nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_DO, u32(index)));
				continue;
			} else {
				reportError(filepath, line_num, token.data - start + 1, "A 'do' statement must come immediately after a 'while' statement");
				success = false;
				continue;
			}
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("<"))) {
			nob_da_append(instructions, instruct(filepath, line_num, col_num, TOK_GT, u32(0)));
			continue;
		}
		reportError(filepath, line_num, token.data - start + 1, nob_temp_sprintf("Unrecognized token: '"SV_Fmt"'", SV_Arg(token)));
		nob_temp_reset();
		success = false;
	}
	
	return success;
}

bool lintInstructionsFromFile(const char * filepath, InstructionArray * instructions)
{
	Nob_String_Builder file = {0};
	if (!nob_read_entire_file(filepath, &file)) return false;

	bool success = true;
	
	Nob_String_View content = {
		.data = file.items,
		.count = file.count	
	};

	IndexStack stack = {0};
	size_t line_num = 1;
	
	content = nob_sv_trim(content);
	while (content.count > 0 && success) {
		Nob_String_View line = nob_sv_chop_by_delim(&content, '\n');
		success = lintInstructionsFromLine(filepath, &stack, line, line_num++, instructions);
		content = nob_sv_trim(content);
	}

	nob_da_free(stack);
	nob_sb_free(file);
	return success;
}
