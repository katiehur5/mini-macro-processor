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

#define INITIAL_MACRO_DICT_CAPACITY 8
#define INITIAL_BUFFER_SIZE 16
#define INITIAL_OUTPUT_SIZE 128

// string struct
typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} string_t; 

typedef struct macro {
    string_t *name;
    string_t *value;
    struct macro *next;
} macro;

typedef struct {
    macro **table;
    size_t size;
    size_t capacity;
} macro_dict; 

void string_putchar(string_t *str, char c);
/**
 * @param str1 a pointer to the string_t that will be appended to
 * @param str2 a pointer to the strint_t that will be copied over 
 * and appended to str1
 * @param free if free is 1, str2 must be freed
 */
void string_putstring(string_t *str1, string_t *str2, int free);
/**
 * @param str1 a pointer to the string_t that will be appended to
 * @param str2 a pointer to the strint_t that will be copied over 
 * and appended to str1
 * @param index str2 will be copied over to str1 from index to the end
 * @param free if free is 1, str2 must be freed 
 */
void string_putsubstring(string_t *str1, string_t *str2, int index, int free);
void string_grow(string_t *str, size_t new_capacity);
string_t *string_malloc(size_t capacity);
void string_free(string_t *str);
void string_clear(string_t *str);
string_t *string_copy(string_t *str);

/**
 * Creates empty macro dictionary. The entries should be
 * initialized to NULL.
 * It is the caller's duty to free the memory allocated by
 * the function.
 * @return the pointer to the newly created macro_dict.
*/
macro_dict *create_macro_dict();

// hash table implementation functions
size_t hash(char *key, size_t capacity);
/**
 * Return true if macroname is contained in 
 * macro dictionary. Should be used when adding (defining)
 * or deleting (undefining) a macro.
 */
bool contains_macro(macro_dict *md, string_t *name);
/**
 * Creates a macro with given name and value and
 * adds it to the macro_dictionary by hashing it,
 * adding it to the linked list at the hashkey
 * index.
 * Dynamically allocated. Caller's duty to free
 * space when done with macro or deleting a macro.
 * @param name a pointer to macroname, must a copy of what
 * was put into the argument buffer.
 * @param value a pointer to macro's value, must make a copy of 
 * what was put into the argument buffer.
 */
void add_macro(macro_dict *md, string_t *name, string_t *value);
void delete_macro(macro_dict *md, string_t *name);
void free_dict(macro_dict *md);
string_t *get_macro_value(macro_dict *md, string_t *name);
void print_macros(macro_dict *md);

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
string_t *string_delete_comments(string_t *str);

void expand(macro_dict *md, string_t *input, string_t *output);

/**
 * Caller's duty to free the returned string.
 */
string_t *replace_placeholders(string_t *expanded, string_t *arg);

int record_then_else(string_t *input, string_t *macroname, string_t *arg1, string_t *arg2, size_t index);

string_t *include_file_contents(string_t *input, string_t *arg1, int index, int *end_index);
void expand_after(string_t *input, string_t *arg1, string_t *arg2, int *index);