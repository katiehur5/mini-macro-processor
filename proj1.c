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
    
    string_t *input = string_malloc(INITIAL_OUTPUT_SIZE);

    // remove comments from file(s) and concatenate into input string_t
    delete_comments(argc, argv, input);

    // a buffer for fully processed output
    string_t *output = string_malloc(INITIAL_OUTPUT_SIZE);
    // a buffer for macronames
    string_t *macroname = string_malloc(INITIAL_BUFFER_SIZE);
    // a dictionary for macros
    macro_dict *md = create_macro_dict();

    enum state curr = PLAINTEXT;

    // creates a pointer starting at the first character of the input
    for (int i = 0; i < input->size; i++) {
        char *ch = input->data[i];
        switch (curr) 
        {
            case PLAINTEXT:
                if (*ch == '\\') {
                    curr = ESCAPE;
                }
                else {
                    string_putchar(output, *ch);
                }
                break;

            case ESCAPE:
                if (isalnum(*ch)) {
                    curr = MACRO;
                    string_clear(macroname);
                    // start a character buffer that keeps track of the macro name
                    string_putchar(macroname, *ch);
                }
                else {
                    string_putchar(output, *ch);
                    curr = PLAINTEXT;
                }
                break;
            
            case MACRO:
                if (isalnum(*ch)) {
                    string_putchar(macroname, *ch);
                }
                else if (*ch == '{') {
                    // check if valid macro here!
                    curr = CHECK_MACRO;
                }
                else {
                    DIE("Error: %s\n", "Invalid character in Macroname.");
                }
                break;
            
            case CHECK_MACRO:
                // check if it's built-in
                if (strcmp(macroname->data, "def")==0) {

                }

                // check if it's user-defined
                else if(contains_macro(md, macroname)) {

                }
                
                // throw error if invalid macroname
                else {
                    DIE("Error: %s\n", "Macro not defined.");
                }
                break;
        }
    }

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
void string_clear(string_t *str) {
    str->data[0] = '\0';
    str->size = 0;
}

macro_dict *create_macro_dict() {
    macro_dict *md = malloc(sizeof(macro_dict));
    if (md == NULL) {
        DIE("Error: %s\n", "Failed to allocate memory for Macro dictionary.");
    }
    md->capacity = INITIAL_MACRO_DICT_CAPACITY;
    md->size = 0;
    md->table = calloc(md->capacity, sizeof(macro *));
    if (md->table == NULL) {
        DIE("Error: %s\n", "Failed to allocate memory for Macro dictionary hash table.");
    }
    return md;
}

size_t hash(char *key, size_t capacity) {
    size_t hash_value = 0;
    while (*key) {
        hash_value = (hash_value * 31 + *key) % capacity;
        key++;
    }
    return hash_value;
}
bool contains_macro(macro_dict *md, string_t *name) {
    size_t index = hash(name->data, md->capacity);
    macro *curr = md->table[index];

    while (curr != NULL) {
        if (strcmp(name->data, curr->name->data) == 0) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}
void add_macro(macro_dict *md, string_t *name, string_t *value) {
    size_t index = hash(name->data, md->capacity);
    
    macro *curr = md->table[index];

    macro *m = malloc(sizeof(macro));
    if (m == NULL) {
        DIE("Error: %s\n", "Failed to allocate memory for Macro.");
    }

    m->next = curr;
    md->table[index] = m;

    // copy name's data to new string_t
    m->name->data = string_malloc(name->size + 2);
    strcpy(m->name->data, name->data);
    m->name->capacity = name->size + 2;
    m->name->size = name->size;

    // copy value's data to new string_t
    m->value->data = string_malloc(value->size + 2);
    strcpy(m->value->data, value->data);
    m->value->capacity = value->size + 2;
    m->value->size = value->size;

    md->size++;
}
void delete_macro(macro_dict *md, string_t *name) {
    size_t index = hash(name->data, md->capacity);
    macro *curr = md->table[index];
    macro *prev =  NULL;
    while (curr != NULL) {
        if (strcmp(name->data, curr->name->data) == 0) {
            if (prev == NULL) {
                md->table[index] = curr->next;
            }
            else {
                prev->next = curr->next;
            }
            string_free(curr->name);
            string_free(curr->value);
            free(curr);
            md->size--;
        }
        prev = curr;
        curr = curr->next;
    }
}
void free_dict(macro_dict *md) {
    for (int i = 0; i < md->capacity; i++) {
        macro *curr = md->table[i];
        while (curr != NULL) {
            macro *temp = curr;
            curr = curr->next;
            string_free(temp->name);
            string_free(temp->value);
            free(temp);
        }
        free(md->table);

    }
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
