# Variables
#
# variables for compiling rules
SHELL=/bin/bash
CC=gcc
CFLAGS=-static -g -Wall -pedantic -std=c11

# paths for testing/submitting
HW1=/c/cs323/proj1
BIN=/c/cs323/proj1


# Compiling
#
# compile proj1 target, acts as default target to "make"
all: proj1

# compile proj1 executable, called by default with all
proj1: proj1.o
	${CC} ${CCFLAGS} -o $@ $^ -lm

# compile object files
proj1.o: proj1.h

# delete the current executable and object files (run to force make to recompile)
clean:
	rm -f proj1 *.o

# Usage: make compare-file file=<input_file>
compare-file: proj1
	valgrind -s --track-origins=yes --leak-check=full --show-leak-kinds=all ./proj1 $(file) > my_output.txt
	/c/cs323/proj1/proj1 $(file) > expected_output.txt
	diff my_output.txt expected_output.txt

# Usage: make compare-stdin input="<input here>"
compare-stdin: proj1
	echo "$(input)" | valgrind -s --track-origins=yes --leak-check=full --show-leak-kinds=all ./proj1 > std_my_output.txt
	echo "$(input)" | /c/cs323/proj1/proj1 > std_expected_output.txt
	diff std_my_output.txt std_expected_output.txt

# PWD Testing
#
# run the entire test suite within your current directory
test:
	${HW1}/run_tests.pl

# run a single test using the command, e.g. "make singletest TEST=t001"
singletest:
	${HW1}/run_tests.pl $(TEST)


# Submission and Testing
#
# submit your source code/documentation files (change if you created more files)
submit:
	${BIN}/submit 1 Makefile proj1.h proj1.c

# check which files have been submitted and when
check:
	${BIN}/check 1
