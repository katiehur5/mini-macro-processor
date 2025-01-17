// proj1.h                                          Stan Eisenstat (09/17/15)
//
// System header files and macros for proj1

#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Write message to stderr using format FORMAT
#define WARN(format,...) fprintf (stderr, "proj1: " format "\n", __VA_ARGS__)

// Write message to stderr using format FORMAT and exit.
#define DIE(format,...)  WARN(format,__VA_ARGS__), exit (EXIT_FAILURE)

// Double the size of an allocated block PTR with NMEMB members and update
// NMEMB accordingly.  (NMEMB is only the size in bytes if PTR is a char *.)
#define DOUBLE(ptr,nmemb) realloc (ptr, (nmemb *= 2) * sizeof(*ptr))

#define INITIAL_MACRO_DICT_CAPACITY 4
#define INITIAL_BUFFER_SIZE 128

// string struct
typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} string_t;

typedef struct {
    string_t **macros;
    size_t size;
    size_t capacity;
    size_t max_length;
} macro_dict; 

void string_putchar(string_t *str, char c);
void string_putstring(string_t *str1, string_t *str2);
void string_grow(string_t *str, size_t new_capacity);
string_t *string_malloc(size_t capacity);
void string_free(string_t *str);

/**
 * Creates empty macro dictionary. The entries should be
 * initialized to NULL.
 * It is the caller's duty to free the memory allocated by
 * the function.
 * @return the pointer to the newly created macro_dict.
*/
macro_dict *create_macro_dict();

/**
 * Goes through each individual file passed in as arguments
 * and deletes comments.
 * @param argc the number of command arguments of type int.
 * @param argv a pointer to an array of the arguments inputted
 * on the command line. These should contain the filenames starting
 * from index 1.
 * @param result a pointer to a string_t output buffer, where
 * the file content will be put after comments are ommitted.
 */
void delete_comments(int argc, char **argv, string_t *result);