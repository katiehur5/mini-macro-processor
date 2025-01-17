#include "proj1.h"

// add more states here! maybe ERROR?
enum state {PLAINTEXT, ESCAPE, MACRO, COMMENT, COMMENT_NEWLINE, CHECK_MACRO};

int main(int argc, char **argv)
{
    
    // invalid command line
    if (argc < 2)
    {
        DIE("USAGE: %s filename\n", argv[0]);
    }
    
    string_t *output = string_malloc(INITIAL_BUFFER_SIZE);

    delete_comments(argc, argv, output);

    //FILE *in = fopen(argv[1], "r");
    //if (in != NULL) {

        /*
        enum state curr = PLAINTEXT;
        int ch;
        while ((ch = fgetc(stdin)) != EOF) {
            switch (curr)
            {
                case PLAINTEXT:
                // sees comment
                    if (ch == '%') 
                    {
                        curr = COMMENT;
                    }
                    else if (ch == '\\')
                    {
                        curr = ESCAPE;
                    }
                    else
                    {

                    }
                    break;
                case ESCAPE:
                    if (isalnum(ch)) {
                        curr = MACRO;
                    }
                    else {
                        curr = PLAINTEXT;
                    }
                    break;
            }
        }
        */

        // display data from output buffer
        printf(output->data);
        string_free(output);
    //}
}

string_t *string_malloc(size_t capacity) {
    if (capacity < 1) {
        DIE("Error: %s\n", "Invalid argument to string_malloc function.");
    }
    string_t *result = malloc(sizeof(string_t));
    if (result == NULL) {
        DIE("Error: %s\n", "Failed to allocate memory for string_t.");
    }
    else {
        result->data = malloc(sizeof(char) * capacity);
        result->size = 0;
        result->capacity = capacity;
        if (result->data == NULL) {
            DIE("Error: %s\n", "Failed to allocate memory for string_t data.");
        }
        result->data[0] = '\0';
        return result;
    }
}
void string_putchar(string_t *str, char c) {
    if (str != NULL) {
        if (str->size + 1 >= str->capacity) {
            size_t new_capacity = str->capacity * 2;
            string_grow(str, new_capacity);
        }
        str->data[str->size] = c;
        str->size++;
        str->data[str->size] = '\0';
    }
} 
void string_putstring(string_t *str1, string_t *str2) {
    if (str1 != NULL && str2 != NULL) {
        if (str1->size + str2->size >= str1->capacity) {
            size_t new_capacity = (str1->capacity + str2->size) * 2;
            string_grow(str1, new_capacity);
        }
        for (int i = 0; i < str2->size; i++) {
            size_t index = str1->size + i;
            str1->data[index] = str2->data[i];
        }

        str1->size = str1->size + str2->size;
        str1->data[str1->size] = '\0';
        string_free(str2);
    }
}
void string_grow(string_t *str, size_t new_capacity) {
    if (str != NULL && str->data != NULL) {
        str->data = realloc(str->data, new_capacity);
        str->capacity = new_capacity;
        if (str == NULL || str->data == NULL) {
            DIE("Error: %s\n", "Failed to re-allocate memory for string_t.");
        }
    }
}
void string_free(string_t *str) {
    if (str != NULL) {
        if (str->data != NULL) {
            free(str->data);
        }
        free(str);
    }
}

macro_dict *create_macro_dict() {
    macro_dict *md = malloc(sizeof(macro_dict));
    if (md == NULL) {
        DIE("Error: %s\n", "Failed to allocate memory for Macro dictionary.");
    }
    md->macros = malloc(sizeof(string_t *) * INITIAL_MACRO_DICT_CAPACITY);
    md->size = 0;
    md->capacity = INITIAL_MACRO_DICT_CAPACITY;
    md->max_length = 0;
    if (md->macros != NULL) {
        for (int i = 0; i < md->capacity; i++) {
            md->macros[i] = NULL;
        }
    }
    return md;
}

void delete_comments(int argc, char **argv, string_t *result) {
    if (argv == NULL || result == NULL || argc < 2) {
        DIE("Error: %s\n", "Invalid arguments to delete_comments function.");
    }

    for (int i = 1; i < argc; i++) {
        FILE *in = fopen(argv[i], "r");
        if (in == NULL) {
            DIE("Error: Could not open file %s\n", argv[i]);
        }
        enum state curr = PLAINTEXT;
        int ch;
        while ((ch = fgetc(in)) != EOF) {
            switch (curr)
            {
                case PLAINTEXT:
                    if (ch == '%') {
                        curr = COMMENT;
                    }
                    else if (ch == '\\') {
                        curr = ESCAPE;
                        string_putchar(result, ch);
                    }
                    else {
                        string_putchar(result, ch);
                    }
                    break;

                case COMMENT:
                    if (ch == '\n') {
                        curr = COMMENT_NEWLINE;
                    }
                    break;
                
                case COMMENT_NEWLINE:
                    if (!isspace(ch)) {
                        curr = PLAINTEXT;
                        string_putchar(result, ch);
                    }
                    break;
                
                case ESCAPE:
                    curr = PLAINTEXT;
                    string_putchar(result, ch);
                    break;
            }
        }
    }
}
