#include "my_cat_func.h"

int main(int argc, char **argv) {
  if (argc <= 1) {
    printf("usage: my_cat <optional: flags> <filenames> ...");
    fprintf(stderr, "usage error");
    return 0;
  }

  struct option my_long_options[] = {{"number-nonblank", no_argument, 0, 'b'},
                                     {"number", no_argument, 0, 'n'},
                                     {"squeeze-blank", no_argument, 0, 's'},
                                     {0, 0, 0, 0}};

  Flags flags = {0};
  int flag;
  int option_index = 0;
  while ((flag = getopt_long(argc, argv, POSSIBLE_FLAGS, my_long_options,
                             &option_index)) != -1) {
    if (flag == 'b') {
      flags.b = 1;
    } else if (flag == 'e') {
      flags.e = 1;
    } else if (flag == 'n') {
      flags.n = 1;
    } else if (flag == 's') {
      flags.s = 1;
    } else if (flag == 't') {
      flags.t = 1;
    } else if (flag == 'v') {
      flags.v = 1;
    } else if (flag == 'T') {
      flags.T = 1;
    } else if (flag == 'E') {
      flags.E = 1;
    }
  }

  int is_newline = 1;
  int counter_for_lines = 1;
  for (int i = optind; i < argc; i++) {
    if (is_file_exists(argv[i])) {
      print_file(argv[i], &flags, &counter_for_lines, &is_newline);
    } else {
      printf("my_cat: %s No such file or directory\n", argv[i]);
      fprintf(stderr, "no such file error");
      return 0;
    }
  }

  return 0;
}