#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} all_flags_t;

void print_with_flags(FILE *file, all_flags_t *flags, int *count);
int parse_flags(int argc, char *argv[], all_flags_t *flags);

int main(int argc, char *argv[]) {
  all_flags_t flags = {0, 0, 0, 0, 0, 0};
  int count = 1;

  if (parse_flags(argc, argv, &flags) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  optind = 1;

  while (optind < argc && argv[optind][0] == '-') {
    if (strcmp(argv[optind], "--") == 0) {
      optind++;
      break;
    }
    optind++;
  }

  if (parse_flags(argc, argv, &flags) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  if (optind == argc) {
    print_with_flags(stdin, &flags, &count);
  } else {
    for (int i = optind; i < argc; i++) {
      FILE *file = fopen(argv[i], "r");
      if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
      }
      print_with_flags(file, &flags, &count);
      fclose(file);
    }
  }

  return EXIT_SUCCESS;
}

void print_with_flags(FILE *file, all_flags_t *flags, int *count) {
  int line;
  int prev = '\n';
  int temp = 0;

  while ((line = fgetc(file)) != EOF) {
    if (flags->s && prev == '\n') {
      if (line == '\n') {
        temp++;
        if (temp >= 2) {
          continue;
        }
      } else {
        temp = 0;
      }
    }

    if (line == '\n') {
      if ((flags->n || flags->b) && prev == '\n' && !flags->b) {
        printf("%6d\t", (*count)++);
      }
      if (flags->e) {
        printf("$");
      }
      printf("%c", line);
    } else {
      if (prev == '\n' && (flags->n || (flags->b && line != '\n'))) {
        printf("%6d\t", (*count)++);
      }
      if (line == '\t' && flags->t) {
        printf("^I");
      } else if (flags->v &&
                 (line < 32 && line != '\t' && line != '\n' && line != '\r')) {
        printf("^%c", line + 64);
      } else if (flags->v && line == 127) {
        printf("^?");
      } else if (flags->v && line == '\r') {
        printf("^M");
      } else {
        printf("%c", line);
      }
    }
    prev = line;
  }
}

int parse_flags(int argc, char *argv[], all_flags_t *flags) {
  int option;
  struct option long_options[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                  {"number", no_argument, NULL, 'n'},
                                  {"squeeze-blank", no_argument, NULL, 's'},
                                  {NULL, 0, NULL, 0}};
  int long_index = 0;

  while ((option = getopt_long(argc, argv, "benstvET", long_options,
                               &long_index)) != -1) {
    switch (option) {
      case 'b':
        flags->b = 1;
        break;
      case 'e':
        flags->e = 1;
        flags->v = 1;
        break;
      case 'n':
        flags->n = 1;
        break;
      case 's':
        flags->s = 1;
        break;
      case 't':
        flags->t = 1;
        flags->v = 1;
        break;
      case 'v':
        flags->v = 1;
        break;
      case 'E':
        flags->e = 1;
        break;
      case 'T':
        flags->t = 1;
        break;
      default:
        fprintf(stderr, "Usage: %s [-benstvET] [file...]\n", argv[0]);
        return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
