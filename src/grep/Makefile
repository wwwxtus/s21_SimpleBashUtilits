all: s21_grep

s21_grep: s21_grep.o
	gcc s21_grep.o -o s21_grep

s21_grep.o: s21_grep.c
	gcc -Wall -Wextra -Werror -std=c11 -c -o s21_grep.o s21_grep.c

rebuild: clean all

clean:
	rm -rf *.o s21_grep
