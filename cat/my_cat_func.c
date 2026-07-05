#include "my_cat_func.h"

void print_file(char *filename, Flags *flags, int *string_counter,
                int *is_newline) {
  FILE *fptr = fopen(filename, "r");
  int ch;
  int prev_blanks_count = *is_newline ? 1 : 0;

  if (flags->e || flags->t) flags->v = 1;

  // управляющий цикл для печати посимвольно
  while ((ch = fgetc(fptr)) != EOF) {
    // обработка -s сжатие пустых строк
    if (flags->s) {
      if (ch == '\n') {
        prev_blanks_count++;
      } else {
        prev_blanks_count = 0;
      }
      if (prev_blanks_count > 2) {
        continue;
      }
    }

    // обработка флага -b нумерация непустых строк
    if (flags->b && *is_newline && ch != '\n') {
      printf("%*d\t", WIDTH, (*string_counter)++);
      *is_newline = 0;
    }

    // обработка флага -n нумерация строк, -b переопределяет -n
    if (flags->n && (flags->b == 0) && *is_newline) {
      printf("%*d\t", WIDTH, (*string_counter)++);
      *is_newline = 0;
    }

    // обработка флага -e конец строки как $
    if (flags->e || flags->E) {
      if (ch == '\n') putchar('$');
    }

    int is_tab_printed = 0;
    // обработка флага -t табы как ^I
    if (flags->t || flags->T) {
      if (ch == '\t') {
        printf("^I");
        is_tab_printed = 1;
      }
    }
    if (flags->v) {
      if (process_v(ch) != 1 && is_tab_printed == 0) {
        putchar(ch);
      }
    } else if (is_tab_printed == 0) {
      putchar(ch);
    }
    if (ch == '\n') {
      *is_newline = 1;
    } else {
      *is_newline = 0;
    }
  }
  fclose(fptr);
}

// отображает управляющие и непечатаемые символы
int process_v(int ch) {
  int is_char_printed = 0;

  if ((ch != '\n' && ch != '\t') && ch < 32) {
    printf("^%c", ch + 64);
    is_char_printed = 1;
  } else if (ch == 127) {
    is_char_printed = 1;
    printf("^?");
  }

  return is_char_printed;
}

int is_file_exists(char *filename) {
  int is_exists = 0;
  FILE *fptr = fopen(filename, "r");

  if (fptr != NULL) {
    is_exists = 1;
    fclose(fptr);
  } else {
    is_exists = 0;
  }
  return is_exists;
}
