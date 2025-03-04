#include "error.h"

#include "nob.h"

static const char *errorLookup[] = {
    [ERROR_SEGFAULT_POP_FROM_EMPTY_STACK] = "Segmentation fault, tried to use an operation that pops off of the stack while the stack was empty",
    [ERROR_UNABLE_TO_COVERT_NUMBER] = "Internal error 1",
    [ERROR_MISMATCHED_IF_AND_ELSE] = "An 'else' without a preceding 'if'",
    [ERROR_OUT_OF_PLACE_END] = "An 'end' statement can only close an 'if', 'else', or 'do' statement",
    [ERROR_MISSING_DO_AFTER_WHILE] = "A 'do' statement must come immediately after a 'while' statement",
    [ERROR_UNRECOGNIZED_TOKEN] = "Internal error 2",
};

void setError(Error error, const char * string)
{
    errorLookup[error] = string;
}

void reportError(const char * filePath, size_t lineNum, size_t columnNum, Error error)
{
	nob_log(NOB_ERROR, "%s:%zu:%zu: %s", filePath, lineNum, columnNum, errorLookup[error]);
}
