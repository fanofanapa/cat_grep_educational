#ifndef MY_GREP_FUNC
#define MY_GREP_FUNC

#include <ctype.h>
#include <getopt.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POSSIBLE_FLAGS "e:ivclnhsf:oE"
#define BUFFER_LENGTH 4096

typedef struct Flags {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int E;
} Flags;

typedef struct Helpers {
  int line_counter;
  int matching_lines_counter;
  int stop_for_l_flag;
  int is_filename_printed;
  int is_new_string;
  int end;
  int matched;
} Helpers;

int check_file(char *filename, Flags *flags, regex_t *regexes,
               int patterns_counter, int is_many_files);

char *my_strdup(char *str);
void patterns_free(char **patterns, int patterns_counter);
void regexes_free(regex_t *regexes, int patterns_counter);
int patterns_grow_cap(char ***patterns_pointer, int *cap, int need);
int save_pattern(char *pattern, char ***patterns_pointer, int *cap,
                 int patterns_counter);

void process_printing_filename(Flags *flags, Helpers *helpers,
                               int is_many_files, char *filename);

void process_flag_o(Flags *flags, Helpers *helpers, const char *filename,
                    char **str_start, regmatch_t *match);

void search_match_in_str(int patterns_counter, Helpers *helpers,
                         regex_t *regexes, char *str_buff);

void process_flag_parsing(Flags *flags, int flag, char ***patterns,
                          int *patterns_cap, int *patterns_counter,
                          int *is_error);

void search_match_o_flag(Helpers *helpers, char *str_start,
                         int patterns_counter, regex_t *regexes, Flags *flags,
                         int is_many_files, char *filename);
#endif