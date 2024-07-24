all: s21_cat

s21_cat: s21_cat.o
	gcc s21_cat.o -o s21_cat

s21_cat.o: s21_cat.c
	gcc -Wall -Wextra -Werror -std=c11 -c -o s21_cat.o s21_cat.c

rebuild: clean all

clean:
	rm -rf *.o s21_cat
