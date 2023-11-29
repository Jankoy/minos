#ifndef _LINTER_H
#define _LINTER_H

#include "types.h"

void parseError(const char * filepath, size_t line_num, size_t column_num, const char * error);
bool lintInstructionsFromFile(const char * filepath, InstructionArray * instructions);	

#endif // _LINTER_H
