#define NOB_IMPLEMENTATION
#include "src/nob.h"

const char * input_paths[] = {
	"src/main.c",
	"src/types.c",
	"src/linter.c",
	"src/compiler.c",
	"src/interpreter.c"
};

int main(int argc, char** argv)
{
	NOB_GO_REBUILD_URSELF(argc, argv);

	const char * program = nob_shift_args(&argc, &argv);
	(void) program;
	
	if (nob_needs_rebuild("minos", input_paths, NOB_ARRAY_LEN(input_paths))) {
		Nob_Cmd cmd = {0};
		nob_cmd_append(&cmd, "cc");
		nob_cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb", "-pedantic");
		nob_cmd_append(&cmd, "-o", "minos");
		nob_da_append_many(&cmd, input_paths, NOB_ARRAY_LEN(input_paths));
		if (!nob_cmd_run_sync(cmd)) return 1;	
	} else {
		nob_log(NOB_INFO, "Executable is up-to-date");
	}

	if (argc > 0) {
		const char * subcmd = nob_shift_args(&argc, &argv);

		if (strcmp(subcmd, "-r") == 0) {
			Nob_Cmd cmd = {0};
			nob_cmd_append(&cmd, "./minos");
			nob_da_append_many(&cmd, argv, argc);
			if (!nob_cmd_run_sync(cmd)) return 1;	
		} else {
			nob_log(NOB_ERROR, "Unknown subcommand %s", subcmd);
		}
	}
	
	return 0;
}
