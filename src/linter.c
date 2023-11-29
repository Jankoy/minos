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

static Instruction push(Value x)
{
	Instruction i = {0};
	i.type = TOK_PUSH;
	i.value = x;
	return i;
}

static Instruction plus()
{
	Instruction i = {0};
	i.type = TOK_PLUS;
	return i;
}

static Instruction minus()
{
	Instruction i = {0};
	i.type = TOK_MINUS;
	return i;
}

static Instruction multiply()
{
	Instruction i = {0};
	i.type = TOK_MULTIPLY;
	return i;
}

static Instruction divide()
{
	Instruction i = {0};
	i.type = TOK_DIVIDE;
	return i;
}

static Instruction dump()
{
	Instruction i = {0};
	i.type = TOK_DUMP;
	return i;
}

static Instruction equal()
{
	Instruction i = {0};
	i.type = TOK_EQUAL;
	return i;
}

static Instruction _if()
{
	Instruction i = {0};
	i.type = TOK_IF;
	return i;
}

static Instruction _else()
{
	Instruction i = {0};
	i.type = TOK_ELSE;
	return i;
}

static Instruction end()
{
	Instruction i = {0};
	i.type = TOK_END;
	return i;
}

void parseError(const char * filepath, size_t line_num, size_t column_num, const char * error)
{
	nob_log(NOB_ERROR, "%s:%zu:%zu: %s", filepath, line_num, column_num, error);
}

static bool lintInstructionsFromLine(const char * filepath, IndexStack * stack, Nob_String_View line, size_t line_num, InstructionArray * instructions)
{
	bool success = true;
	const char * start = line.data;

	while (line.count > 0 && success) {
		Nob_String_View token = nob_sv_chop_by_space(&line);

		if (nob_sv_eq(token, nob_sv_from_cstr("#"))) {
			break;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("+"))) {
			nob_da_append(instructions, plus());
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("-"))) {
			nob_da_append(instructions, minus());
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("*"))) {
			nob_da_append(instructions, multiply());
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("/"))) {
			nob_da_append(instructions, divide());
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("."))) {
			nob_da_append(instructions, dump());
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("="))) {
			nob_da_append(instructions, equal());
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("true"))) {
			nob_da_append(instructions, push(_bool(true)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("false"))) {
			nob_da_append(instructions, push(_bool(false)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("if"))) {
			nob_da_append(stack, instructions->count);
			nob_da_append(instructions, _if());
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("else"))) {
			if (stack->count > 0) {
				size_t index = stack->items[stack->count - 1];
				nob_da_pop(stack);
				instructions->items[index].value = u32((uint32_t) instructions->count + 1);
				nob_da_append(stack, instructions->count);
			} else {
				parseError(filepath, line_num, token.data - start + 1, "An 'else' statement without a preceding 'if' statement");
				success = false;
				continue;
			}
			nob_da_append(instructions, _else());
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("end"))) {
			if (stack->count > 0) {
				size_t index = stack->items[stack->count - 1];
				nob_da_pop(stack);
				if (instructions->items[index].type == TOK_IF || instructions->items[index].type == TOK_ELSE) {
					instructions->items[index].value = u32((uint32_t) instructions->count);
				}
			} else {
				parseError(filepath, line_num, token.data - start + 1, "An 'end' statement without a preceding block opening statement");
				success = false;
				continue;
			}
			nob_da_append(instructions, end());
			continue;
		}
		if (isdigit((int)*token.data)) {
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
			continue;
		}
		parseError(filepath, line_num, token.data - start + 1, nob_temp_sprintf("Unrecognized token: '"SV_Fmt"'", SV_Arg(token)));
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
