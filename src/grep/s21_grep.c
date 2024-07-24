#define _GNU_SOURCE  // for getline
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int reg_flag;
  bool invert;
  bool count;
  bool file_match;
  bool number_line;
  bool no_error;
  bool no_filename;
  char *file_with_patterns;
} all_flags_t;

all_flags_t grep_read_flags(int argc, char *argv[]) {
  all_flags_t flags = {0, 0, 0, 0, 0, 0, 0, NULL};
  int curr_flag;
  while ((curr_flag = getopt(argc, argv, "eivclnhsfo:")) != -1) {
    switch (curr_flag) {
      case 'e':
        flags.reg_flag |= REG_EXTENDED;
        break;
      case 'i':
        flags.reg_flag |= REG_ICASE;
        break;
      case 'v':
        flags.invert = true;
        break;
      case 'c':
        flags.count = true;
        break;
      case 'l':
        flags.file_match = true;
        break;
      case 'n':
        flags.number_line = true;
        break;
      case 's':
        flags.no_error = true;
        break;
      case 'h':
        flags.no_filename = true;
        break;
      case 'f':
        flags.file_with_patterns = optarg;
        break;
      case 'o':

      default:
        break;
    }
  }
  return flags;
}

void add_newline_if_needed(char *line) {
  size_t len = strlen(line);
  if (len > 0 && line[len - 1] != '\n') {
    printf("%s\n", line);
  } else {
    printf("%s", line);
  }
}

void grep_count(FILE *file, const char *filename, all_flags_t flags,
                regex_t *preg, int count_file) {
  char *line = NULL;
  size_t length = 0;
  regmatch_t match;
  int count = 0;
  while (getline(&line, &length, file) > 0) {
    int match_found = !regexec(preg, line, 1, &match, 0);
    if (flags.invert) {
      match_found = !match_found;
    }
    if (match_found) {
      ++count;
    }
  }
  if (!flags.file_match) {
    if (count_file == 1) {
      printf("%d\n", count);
    } else if (!flags.no_filename) {
      printf("%s:%d\n", filename, count);
    } else {
      printf("%d\n", count);
    }
  }
  free(line);
}

void grep_file(FILE *file, all_flags_t flags, regex_t *preg,
               const char *filename, int count_file) {
  char *line = NULL;
  size_t length = 0;
  regmatch_t match;
  int line_num = 0;
  while (getline(&line, &length, file) > 0) {
    ++line_num;
    int match_found = !regexec(preg, line, 1, &match, 0);
    if (flags.invert) {
      match_found = !match_found;
    }
    if (match_found) {
      if (flags.file_match) {
        printf("%s\n", filename);
        free(line);
        return;
      }
      if (count_file == 1) {
        if (!flags.no_filename) {
          if (flags.number_line) {
            printf("%d:", line_num);
          }
        }
        add_newline_if_needed(line);
      } else {
        if (!flags.no_filename) {
          if (flags.number_line) {
            printf("%s:%d:", filename, line_num);
          } else {
            printf("%s:", filename);
          }
        } else if (flags.number_line) {
          printf("%d:", line_num);
        }
        add_newline_if_needed(line);
      }
    }
  }
  free(line);
}

void grep(int argc, char *argv[], all_flags_t flags) {
  char **pattern = &argv[optind];
  char **end = &argv[argc];
  regex_t preg_storage;
  regex_t *preg = &preg_storage;
  int count = 0;

  if (flags.file_with_patterns) {
    FILE *pattern_file = fopen(flags.file_with_patterns, "r");
    if (!pattern_file) {
      if (!flags.no_error) {
        fprintf(stderr, "%s: ", argv[0]);
        perror(flags.file_with_patterns);
      }
      exit(EXIT_FAILURE);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, pattern_file);
    if (read == -1) {
      if (!flags.no_error) {
        fprintf(stderr, "No pattern provided in file\n");
      }
      fclose(pattern_file);
      exit(EXIT_FAILURE);
    }
    line[read - 1] = '\0';
    pattern = &line;
    fclose(pattern_file);
  }

  if (pattern == end) {
    fprintf(stderr, "No pattern provided\n");
    exit(EXIT_FAILURE);
  }

  if (regcomp(preg, *pattern, flags.reg_flag)) {
    fprintf(stderr, "Failed to compile regex\n");
    exit(EXIT_FAILURE);
  }

  for (char **filename = pattern + 1; filename != end; ++filename) {
    if (**filename == '-') {
      continue;
    }
    ++count;
    if (count >= 2) {
      break;
    }
  }

  for (char **filename = pattern + 1; filename != end; ++filename) {
    if (**filename == '-') {
      continue;
    }
    FILE *file = fopen(*filename, "rb");
    if (!file) {
      if (!flags.no_error) {
        fprintf(stderr, "%s: ", argv[0]);
        perror(*filename);
      }
      continue;
    }
    if (flags.count && !flags.file_match) {
      grep_count(file, *filename, flags, preg, count);
    } else if (!(flags.count && flags.file_match)) {
      grep_file(file, flags, preg, *filename, count);
    }
    fclose(file);
  }
  regfree(preg);
}

int main(int argc, char *argv[]) {
  all_flags_t flags = grep_read_flags(argc, argv);
  grep(argc, argv, flags);
  return 0;
}
