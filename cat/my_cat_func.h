#ifndef MY_CAT_FUNC
#define MY_CAT_FUNC

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define POSSIBLE_FLAGS "benstvET"
#define WIDTH 6

typedef struct Flags {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
  int E;
  int T;
} Flags;

int process_v(int ch);
void print_file(char *filename, Flags *flags, int *string_counter,
                int *is_newline);
int is_file_exists(char *filename);
#endif