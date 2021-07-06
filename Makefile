comp = gcc -g -o loesung -O3 -std=c11 -Wall -Werror -Wextra -Wpedantic -DNDEBUG loesung.c
run = cat example_testcases/example_long_ID.stdin | ./loesung
val = valgrind --leak-check=yes --show-leak-kinds=all ./loesung > /dev/null

all: loesung.c compile run
	$(comp)
	$(run)

compile: loesung.c
	$(comp)

run:
	$(run)

check: loesung.c compile
	$(comp)
	cat example_testcases/example_long_ID.stdin | $(val)