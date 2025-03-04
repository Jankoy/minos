#ifndef _ERROR_H
#define _ERROR_H

#include <stddef.h>

typedef enum {
    ERROR_SEGFAULT_POP_FROM_EMPTY_STACK,
    ERROR_UNABLE_TO_COVERT_NUMBER,
    ERROR_MISMATCHED_IF_AND_ELSE,
    ERROR_OUT_OF_PLACE_END,
    ERROR_MISSING_DO_AFTER_WHILE,
    ERROR_UNRECOGNIZED_TOKEN,
} Error;

void setError(Error error, const char * string);
void reportError(const char * filePath, size_t lineNum, size_t columnNum, Error error);

#endif //_ERROR_H
