#include "my_grep_func.h"

int main(int argc, char **argv) {
  int is_error = 0;
  if (argc <= 1) {
    printf("usage: my_grep <options> template file_path <file_path2> ...");
    fprintf(stderr, "usage error\n");
    is_error = 1;
  }

  Flags flags = {0};

  int flag;
  char **patterns = NULL;
  int patterns_counter = 0;
  int patterns_cap = 0;

  while (is_error == 0 && ((flag = getopt(argc, argv, POSSIBLE_FLAGS)) != -1)) {
    process_flag_parsing(&flags, flag, &patterns, &patterns_cap,
                         &patterns_counter, &is_error);
  }

  int files_start = 0;
  if (is_error == 0) {
    if (patterns_counter == 0 && optind < argc) {
      int save_res = save_pattern(argv[optind], &patterns, &patterns_cap,
                                  patterns_counter);
      if (save_res != 0) {
        is_error = 1;
        patterns_free(patterns, patterns_counter);
        patterns = NULL;
      } else {
        patterns_counter++;
        optind++;
      }
    }

    files_start = optind;

    if (!argv[files_start]) {
      printf("my_grep: cant locate a file or read template\n");
      printf("usage: my_grep <options> template file_path <file_path2> ...");
      fprintf(stderr, "finding file error\n");
      is_error = 1;
    }
  }

  regex_t *regexes = malloc(patterns_counter * sizeof(regex_t));
  if (regexes == NULL) {
    is_error = 1;
    patterns_free(patterns, patterns_counter);
    patterns = NULL;
  }
  int cflags = 0;
  cflags |= REG_NEWLINE;

  if (is_error == 0) {
    if (flags.i) cflags |= REG_ICASE;
    if (flags.E) cflags |= REG_EXTENDED;

    for (int i = 0; i < patterns_counter && is_error == 0; i++) {
      if (regcomp(&regexes[i], patterns[i], cflags) != 0) {
        regexes_free(regexes, i);
        regexes = NULL;

        patterns_free(patterns, patterns_counter);
        patterns = NULL;

        is_error = 1;
        printf("\nERROR WHILE COMPILING REGULAR EXPRESSION");
        fprintf(stderr, "regcomp error\n");
      }
    }
  }

  if (is_error == 0) {
    int is_many_files = (files_start + 1 == argc) ? 0 : 1;
    for (int i = files_start; i < argc; i++) {
      if (check_file(argv[i], &flags, regexes, patterns_counter,
                     is_many_files) != 0) {
        if (!flags.s)
          printf("my_grep: %s No such file or directory\n", argv[i]);
      }
    }
  }

  if (patterns != NULL) {
    patterns_free(patterns, patterns_counter);
    patterns = NULL;
  }
  if (regexes != NULL) {
    regexes_free(regexes, patterns_counter);
    regexes = NULL;
  }
  return 0;
}