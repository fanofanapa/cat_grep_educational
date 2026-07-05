#include "my_grep_func.h"

char *my_strdup(char *str) {
  size_t len = strlen(str) + 1;
  char *strptr = malloc(len);
  if (strptr != NULL) memcpy(strptr, str, len);

  return strptr;
}

void patterns_free(char **patterns, int patterns_counter) {
  if (patterns != NULL) {
    for (int i = 0; i < (patterns_counter); i++) {
      if (patterns[i] != NULL) free(patterns[i]);
    }
    free(patterns);
  }
}

int patterns_grow_cap(char ***patterns_pointer, int *cap, int need) {
  if (need <= (*cap)) return 0;

  int is_error = 0;
  int new_cap = ((*cap) == 0) ? 4 : *cap;
  while ((new_cap < need) && is_error == 0) {
    if (new_cap > INT_MAX / 2) {
      is_error = 1;
    } else {
      new_cap *= 2;
    }
  }

  char **tmp = realloc(*patterns_pointer, (size_t)new_cap * sizeof(char *));
  if (tmp == NULL) {
    is_error = 1;
  } else {
    *patterns_pointer = tmp;
    *cap = new_cap;
  }

  return is_error;
}

void regexes_free(regex_t *regexes, int patterns_counter) {
  if (regexes != NULL) {
    for (int i = 0; i < patterns_counter; i++) {
      regfree(&regexes[i]);
    }
    free(regexes);
  }
}

int save_pattern(char *pattern, char ***patterns_pointer, int *cap,
                 int patterns_counter) {
  int is_error = 0;

  if (patterns_grow_cap(patterns_pointer, cap, patterns_counter + 1) != 0) {
    is_error = 1;
  } else {
    (*patterns_pointer)[patterns_counter] = my_strdup(pattern);

    if ((*patterns_pointer)[patterns_counter] == NULL) {
      patterns_free((*patterns_pointer), patterns_counter);
      (*patterns_pointer) = NULL;
      is_error = 1;
    }
  }

  return is_error;
}

void process_printing_filename(Flags *flags, Helpers *helpers,
                               int is_many_files, char *filename) {
  if (is_many_files && helpers->matched && helpers->is_new_string &&
      !helpers->is_filename_printed) {
    if (!flags->h) {
      printf("%s:", filename);
      helpers->is_filename_printed = 1;
    }
  }
}

void process_flag_o(Flags *flags, Helpers *helpers, const char *filename,
                    char **str_start, regmatch_t *match) {
  if (flags->l && helpers->matched && !helpers->is_filename_printed) {
    printf("%s\n", filename);
    helpers->stop_for_l_flag = 1;
    helpers->is_filename_printed = 1;
    helpers->end = 1;
  } else if (!flags->c) {
    // номер строки
    if (flags->n) printf("%i:", helpers->line_counter);

    // только совпадающая часть для -o
    int len = match->rm_eo - match->rm_so;
    printf("%.*s\n", len, (*str_start) + match->rm_so);
    helpers->is_new_string = 1;
    helpers->is_filename_printed = 0;
  } else {
    if (helpers->is_new_string) {
      helpers->matching_lines_counter++;
      helpers->is_new_string = 0;
    }
  }
  // увеличиваем адрес, как минимум на 1, чтобы не падать в беск цикл
  (*str_start) += (match->rm_eo == 0) ? 1 : match->rm_eo;
  if ((**str_start) == '\0') helpers->end = 1;
}

void search_match_in_str(int patterns_counter, Helpers *helpers,
                         regex_t *regexes, char *str_buff) {
  for (int i = 0; i < patterns_counter && helpers->matched == 0; i++) {
    if (regexec(&regexes[i], str_buff, 0, NULL, 0) == 0) {
      helpers->matched = 1;
    }
  }
}

void search_match_o_flag(Helpers *helpers, char *str_start,
                         int patterns_counter, regex_t *regexes, Flags *flags,
                         int is_many_files, char *filename) {
  while (!helpers->end && *str_start != '\0') {
    int best_pattern_index = -1;
    regmatch_t best_match;
    best_match.rm_so = -1;

    for (int i = 0; i < patterns_counter; i++) {
      regmatch_t current_match;
      if (regexec(&regexes[i], str_start, 1, &current_match, 0) == 0) {
        if (best_pattern_index == -1 ||
            current_match.rm_so < best_match.rm_so) {
          best_pattern_index = i;
          best_match = current_match;

          // выбираем более длинное совпадение
        } else if (current_match.rm_so == best_match.rm_so) {
          int current_len = current_match.rm_eo - current_match.rm_so;
          int best_len = best_match.rm_eo - best_match.rm_so;
          if (current_len > best_len) {
            best_pattern_index = i;
            best_match = current_match;
          }
        }
      }
    }

    // если нашелся паттерт вкл флаг совпадения
    if (best_pattern_index != -1) {
      helpers->matched = 1;

      if (!flags->l) {
        process_printing_filename(flags, helpers, is_many_files, filename);
      }

      process_flag_o(flags, helpers, filename, &str_start, &best_match);

      if (helpers->stop_for_l_flag) helpers->end = 1;
    } else {
      helpers->end = 1;
    }
  }
}

int check_file(char *filename, Flags *flags, regex_t *regexes,
               int patterns_counter, int is_many_files) {
  Helpers helpers = {1, 0, 0, 0, 0, 0, 0};
  char str_buff[BUFFER_LENGTH];
  FILE *fptr = fopen(filename, "r");
  if (fptr == NULL) return 1;

  while (fgets(str_buff, sizeof(str_buff), fptr) && !helpers.stop_for_l_flag) {
    helpers.is_new_string = 1;
    helpers.matched = 0;

    if (flags->o && !flags->v) {
      char *str_start = str_buff;
      helpers.end = 0;

      search_match_o_flag(&helpers, str_start, patterns_counter, regexes, flags,
                          is_many_files, filename);

    } else {  // обработка без -o
      search_match_in_str(patterns_counter, &helpers, regexes, str_buff);

      if (flags->v) helpers.matched = !helpers.matched;
      if (flags->l && helpers.matched) {
        printf("%s\n", filename);
        helpers.stop_for_l_flag = 1;
        helpers.is_filename_printed = 1;
      }

      process_printing_filename(flags, &helpers, is_many_files, filename);

      if (helpers.matched && !helpers.stop_for_l_flag) {
        if (!flags->c) {
          if (flags->n) printf("%i:", helpers.line_counter);
          printf("%s", str_buff);
          helpers.is_new_string = 1;
          helpers.is_filename_printed = 0;
        } else {
          helpers.matching_lines_counter++;
        }
      }
    }

    helpers.line_counter++;
  }
  if (flags->c && !flags->l) {
    if (!helpers.is_filename_printed && is_many_files && !flags->h)
      printf("%s:", filename);
    printf("%i\n", helpers.matching_lines_counter);
  }
  fclose(fptr);
  return 0;
}

void process_flag_parsing(Flags *flags, int flag, char ***patterns,
                          int *patterns_cap, int *patterns_counter,
                          int *is_error) {
  if (flag == 'e') {
    flags->e = 1;

    int save_res =
        save_pattern(optarg, patterns, patterns_cap, *patterns_counter);
    if (save_res != 0) {
      *is_error = 1;
      patterns_free(*patterns, *patterns_counter);
      *patterns = NULL;
    } else {
      (*patterns_counter)++;
    }
  } else if (flag == 'i') {
    flags->i = 1;
  } else if (flag == 'v') {
    flags->v = 1;
  } else if (flag == 'c') {
    flags->c = 1;
  } else if (flag == 'l') {
    flags->l = 1;
  } else if (flag == 'n') {
    flags->n = 1;
  } else if (flag == 'h') {
    flags->h = 1;
  } else if (flag == 's') {
    flags->s = 1;
  } else if (flag == 'f') {
    flags->f = 1;

    char str_buff[BUFFER_LENGTH];
    FILE *fptr = fopen(optarg, "r");
    if (fptr == NULL) {
      if (!flags->s) {
        printf("my_grep: %s: No such file or directory\n", optarg);
      }
      *is_error = 1;
    } else {
      while (fgets(str_buff, sizeof(str_buff), fptr)) {
        size_t len = strlen(str_buff);
        if (len > 0 && str_buff[len - 1] == '\n') {
          str_buff[len - 1] = '\0';
        }
        int save_res =
            save_pattern(str_buff, patterns, patterns_cap, *patterns_counter);
        if (save_res != 0) {
          *is_error = 1;
          patterns_free(*patterns, *patterns_counter);
          *patterns = NULL;

        } else {
          (*patterns_counter)++;
        }
      }
      fclose(fptr);
    }
  } else if (flag == 'o') {
    flags->o = 1;
  } else if (flag == 'E') {
    flags->E = 1;
  }
}
