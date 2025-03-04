#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "src/nob.h"

static const char *compiler = "cc";

static const char * input_paths[] = {
	"src/main.c",
	"src/types.c",
	"src/error.c",
	"src/linter.c",
	"src/compiler.c",
	"src/interpreter.c"
};

static const char *output = "minos";

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  const char *program = shift_args(&argc, &argv);
  (void)program;

  if (needs_rebuild(output, input_paths, ARRAY_LEN(input_paths))) {
    Cmd cmd = {0};
    cmd_append(&cmd, compiler);
    cmd_append(&cmd, "-Wall", "-Wextra", "-ggdb");
    cmd_append(&cmd, "-o", output);
    da_append_many(&cmd, input_paths, ARRAY_LEN(input_paths));
    if (!cmd_run_sync(cmd))
      return 1;
  } else {
    nob_log(INFO, "Executable is already up to date");
  }

  if (argc > 0) {
    const char *subcmd = shift_args(&argc, &argv);

    if (strcmp(subcmd, "-r") == 0) {
      Cmd cmd = {0};
      cmd_append(&cmd, temp_sprintf("./%s", output));
      temp_reset();
      da_append_many(&cmd, argv, argc);
      if (!cmd_run_sync(cmd))
        return 1;
    } else if (strcmp(subcmd, "-d") == 0) {
      Cmd cmd = {0};
      cmd_append(&cmd, "gdb");
      cmd_append(&cmd, temp_sprintf("./%s", output));
      temp_reset();
      if (!cmd_run_sync(cmd))
        return 1;
    } else {
      nob_log(ERROR, "Unknown subcommand %s", subcmd);
    }
  }

  return 0;
}

