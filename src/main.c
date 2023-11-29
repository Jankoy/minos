#define NOB_IMPLEMENTATION
#include "nob.h"

#include "types.h"
#include "linter.h"
#include "interpreter.h"
#include "compiler.h"

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
		if (!lintInstructionsFromFile(filepath, &instructions)) return 1;
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
		if (!lintInstructionsFromFile(filepath, &instructions)) return 1;
		compileProgram(&instructions, filepath);
		nob_da_free(instructions);
	} else {
		nob_log(NOB_INFO, "Usage: %s <run/compile> <args>", program);
		nob_log(NOB_ERROR, "Invalid subcommand provided");
		return 1;
	} 
	
	return 0;
}
