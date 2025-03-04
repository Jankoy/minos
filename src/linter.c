#include "linter.h"

#include "nob.h"
#include "error.h"
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
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return STR2INT_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return STR2INT_UNDERFLOW;
    if (*end != '\0')
        return STR2INT_INCONVERTIBLE;
    *out = l;
    return STR2INT_SUCCESS;
}

static Instruction instruct(const char * filePath, size_t lineNum, size_t colNum, TokenType type, Value x)
{
	Instruction i = {0};
	i.token = makeToken(filePath, lineNum, colNum, type);
	i.value = x;
	return i;
}

static bool lintInstructionsFromLine(const char * filePath, IndexStack * stack, Nob_String_View line, size_t lineNum, InstructionArray * instructions)
{
	bool success = true;
	const char * start = line.data;

	while (line.count > 0 && success) {
		Nob_String_View token = nob_sv_chop_by_space(&line);
		size_t colNum = token.data - start + 1;
		
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
				nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_PUSH, i32Value(num)));
			} else {
			    setError(ERROR_UNABLE_TO_COVERT_NUMBER, nob_temp_sprintf("Unable to convert '"SV_Fmt"' into a number", SV_Arg(token)));
				reportError(filePath, lineNum, token.data - start + 1, ERROR_UNABLE_TO_COVERT_NUMBER);
				nob_temp_reset();
				success = false;
			}
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("true"))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_PUSH, i32Value(true)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("false"))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_PUSH, i32Value(false)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("+"))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_PLUS, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("-"))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_MINUS, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("*"))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_MULTIPLY, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("/"))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_DIVIDE, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("."))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_DUMP, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("="))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_EQUAL, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("if"))) {
			nob_da_append(stack, instructions->count);
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_IF, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("else"))) {
			if (stack->count > 0) {
				size_t index = stack->items[stack->count - 1];
				if (instructions->items[index].token.type == TOK_IF) {
					stack->count--;
					instructions->items[index].value = i32Value((int32_t) instructions->count + 1);
					nob_da_append(stack, instructions->count);
				} else {
					reportError(filePath, lineNum, colNum, ERROR_MISMATCHED_IF_AND_ELSE);
					success = false;
					continue;
				}
			} else {
				reportError(filePath, lineNum, colNum, ERROR_MISMATCHED_IF_AND_ELSE);
				success = false;
				continue;
			}
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_ELSE, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("end"))) {
			if (stack->count > 0) {
				size_t index = stack->items[stack->count - 1];
				stack->count--;
				if (instructions->items[index].token.type == TOK_IF || instructions->items[index].token.type == TOK_ELSE) {
					instructions->items[index].value = i32Value((int32_t) instructions->count);
					Value v = i32Value((int32_t) instructions->count + 1); // I have to split this up because of pointer incrementation in the nob_da_append macro
					nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_END, v));
					continue;
				} else if (instructions->items[index].token.type == TOK_DO) {
					Value v = i32Value((int32_t) instructions->count + 1); // To save the instruction pointer
					nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_END, instructions->items[index].value));
					instructions->items[index].value = v;
					continue;
				} else {
					reportError(filePath, lineNum, token.data - start + 1, ERROR_OUT_OF_PLACE_END);
					success = false;
					continue;
				}
			} else {
				reportError(filePath, lineNum, token.data - start + 1, ERROR_OUT_OF_PLACE_END);
				success = false;
				continue;
			}
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("dup"))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_DUP, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr(">"))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_GT, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("while"))) {
			nob_da_append(stack, instructions->count);
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_WHILE, i32Value(0)));
			continue;
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("do"))) {
			size_t index = stack->items[stack->count - 1];
			stack->count--;
			if (instructions->items[index].token.type == TOK_WHILE) {
				nob_da_append(stack, instructions->count);
				nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_DO, i32Value(index)));
				continue;
			} else {
				reportError(filePath, lineNum, token.data - start + 1, ERROR_MISSING_DO_AFTER_WHILE);
				success = false;
				continue;
			}
		}
		if (nob_sv_eq(token, nob_sv_from_cstr("<"))) {
			nob_da_append(instructions, instruct(filePath, lineNum, colNum, TOK_GT, i32Value(0)));
			continue;
		}
		setError(ERROR_UNRECOGNIZED_TOKEN, nob_temp_sprintf("Unrecognized token: '"SV_Fmt"'", SV_Arg(token)));
		reportError(filePath, lineNum, token.data - start + 1, ERROR_UNRECOGNIZED_TOKEN);
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
