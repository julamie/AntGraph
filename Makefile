all: loesung.c compile run
	gcc -g -o loesung -O3 -std=c11 -Wall -Werror -Wextra -Wpedantic -DNDEBUG loesung.c
	cat example_testcases/example_long_ID.stdin | ./loesung

compile: loesung.c
	gcc -g -o loesung -O3 -std=c11 -Wall -Werror -Wextra -Wpedantic -DNDEBUG loesung.c

run:
	cat example_testcases/example_long_ID.stdin | ./loesung

check: loesung.c compile
	gcc -g -o loesung -O3 -std=c11 -Wall -Werror -Wextra -Wpedantic -DNDEBUG loesung.c
	cat example_testcases/example_long_ID.stdin | valgrind --leak-check=yes ./loesung > /dev/null