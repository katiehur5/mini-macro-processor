#include "proj1.h"

// add more states here! maybe ERROR?
enum state {PLAINTEXT, ESCAPE, MACRO, COMMENT, COMMENT_NEWLINE, DEF_NAME,
            DEF_VALUE, USER_ARG, UNDEF_NAME, IF_COND};

int main(int argc, char **argv)
{
    string_t *input = string_malloc(INITIAL_OUTPUT_SIZE);

    if (argc > 1) {
        // remove comments from file(s) and concatenate into input string_t
        delete_comments(argc, argv, input);
    }
    else {
        int ch;
        while ((ch = fgetc(stdin)) != EOF) {
            string_putchar(input, ch);
        }
        string_t *new_input = string_delete_comments(input);
        string_free(input);
        input = new_input;
    }

    // a buffer for fully processed output
    string_t *output = string_malloc(INITIAL_OUTPUT_SIZE);

    // a dictionary for macros
    macro_dict *md = create_macro_dict();
    expand(md, input, output);

    printf("%s",output->data);
    string_free(output);
    string_free(input);
    free_dict(md);
    
}

string_t *string_malloc(size_t capacity) {
    if (capacity < 1) {
        DIE("Error: %s", "Invalid argument to string_malloc function.");
    }
    string_t *result = malloc(sizeof(string_t));
    if (result == NULL) {
        DIE("Error: %s", "Failed to allocate memory for string_t.");
    }
    else {
        result->data = malloc(sizeof(char) * capacity);
        result->size = 0;
        result->capacity = capacity;
        if (result->data == NULL) {
            DIE("Error: %s", "Failed to allocate memory for string_t data.");
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
void string_putstring(string_t *str1, string_t *str2, int free) {
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
        if (free == 1) {
            string_free(str2);
        }
    }
}
void string_putsubstring(string_t *str1, string_t *str2, int index, int free) {
    if (str1 != NULL && str2 != NULL && index < str2->size) {
        size_t substring2_size = str2->size - index;
        if (str1->size + substring2_size >= str1->capacity) {
            size_t new_capacity = (str1->capacity + substring2_size) * 2;
            string_grow(str1, new_capacity);
        }
        for (int i = 0; i < substring2_size; i++) {
            size_t ind = str1->size + i;
            str1->data[ind] = str2->data[index + i];
        }

        str1->size = str1->size + substring2_size;
        str1->data[str1->size] = '\0';
        if (free == 1) {
            string_free(str2);
        }
    }
}
void string_grow(string_t *str, size_t new_capacity) {
    if (str != NULL && str->data != NULL) {
        str->data = realloc(str->data, new_capacity);
        str->capacity = new_capacity;
        if (str == NULL || str->data == NULL) {
            DIE("Error: %s", "Failed to re-allocate memory for string_t.");
        }
    }
}
void string_free(string_t *str) {
    if (str != NULL) {
        if (str->data != NULL) {
            free(str->data);
        }
        free(str);
        str = NULL;
    }
}
void string_clear(string_t *str) {
    str->data[0] = '\0';
    str->size = 0;
}
string_t *string_copy(string_t *str) {
    if (str == NULL) {
        DIE("Error: %s", "Invalid argument to string_copy function.");
    }
    string_t *new = string_malloc(str->size + 1);
    strcpy(new->data, str->data);
    new->size = str->size;
    new->capacity = str->size + 1;
    return new;
}

macro_dict *create_macro_dict() {
    macro_dict *md = malloc(sizeof(macro_dict));
    if (md == NULL) {
        DIE("Error: %s", "Failed to allocate memory for Macro dictionary.");
    }
    md->capacity = INITIAL_MACRO_DICT_CAPACITY;
    md->size = 0;
    md->table = calloc(md->capacity, sizeof(macro *));
    if (md->table == NULL) {
        DIE("Error: %s", "Failed to allocate memory for Macro dictionary hash table.");
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
    if (md == NULL || name == NULL) {
        DIE("Error: %s", "Invalid arguments to contains_macro function.");
    }
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
    if (md == NULL || name == NULL || value == NULL) {
        DIE("Error: %s", "Invalid arguments to add_macro function.");
    }
    size_t index = hash(name->data, md->capacity);
    
    macro *curr = md->table[index];

    macro *m = malloc(sizeof(macro));
    if (m == NULL) {
        DIE("Error: %s", "Failed to allocate memory for Macro.");
    }

    m->next = curr;
    md->table[index] = m;

    m->name = string_copy(name);
    m->value = string_copy(value);

    md->size++;
}
void delete_macro(macro_dict *md, string_t *name) {
    if (md == NULL || name == NULL) {
        DIE("Error: %s", "Invalid arguments to delete_macro function.");
    }
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
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}
void free_dict(macro_dict *md) {
    if (md != NULL) {
        for (int i = 0; i < md->capacity; i++) {
            macro *curr = md->table[i];
            while (curr != NULL) {
                macro *temp = curr;
                curr = curr->next;
                string_free(temp->name);
                string_free(temp->value);
                free(temp);
            }
        }
        free(md->table);
        free(md);
        md = NULL;
    }
}
string_t *get_macro_value(macro_dict *md, string_t *name) {
    if (md == NULL || name == NULL) {
        DIE("Error: %s", "Invalid arguments to get_macro_value function.");
    }
    size_t index = hash(name->data, md->capacity);
    macro *curr = md->table[index];
    
    while (curr != NULL) {
        if (strcmp(name->data, curr->name->data) == 0) {
            return curr->value;
        }
        curr = curr->next;
    }
    DIE("Error: %s", "Macro not in macro dictionary");
}
void print_macros(macro_dict *md){
    if (md == NULL) {
        DIE("Error: %s", "Invalid argument to print_macros function.");
    }
    macro *curr = NULL;
    for (int i = 0; i < md->capacity; i++) {
        curr = md->table[i];
        while (curr != NULL) {
            printf("Macro: %s, Value: %s\n", curr->name->data, curr->value->data);
            curr = curr->next;
        }
    }
}

string_t *string_delete_comments(string_t *str) {
    if (str == NULL) {
        DIE("Error: %s", "Invalid argumetn to string_delete_comments function.");
    }
    string_t *result = string_malloc(INITIAL_OUTPUT_SIZE);
    enum state curr = PLAINTEXT;
    int i = 0;
    while (i < str->size) {
        char ch = str->data[i];
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
                if (ch == '\n' || !isspace(ch)) {
                    curr = PLAINTEXT;
                    string_putchar(result, ch);
                }
                break;
            
            case ESCAPE:
                curr = PLAINTEXT;
                string_putchar(result, ch);
                break;

            default: 
                break;
        }
        i++;
    }
    return result;
}


void delete_comments(int argc, char **argv, string_t *result) {
    if (argv == NULL || result == NULL || argc < 2) {
        DIE("Error: %s", "Invalid arguments to delete_comments function.");
    }

    for (int i = 1; i < argc; i++) {
        FILE *in = fopen(argv[i], "r");
        if (in == NULL) {
            DIE("Error: Could not open file %s", argv[i]);
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
                    if (ch == '\n' || !isspace(ch)) {
                        curr = PLAINTEXT;
                        string_putchar(result, ch);
                    }
                    break;
                
                case ESCAPE:
                    curr = PLAINTEXT;
                    string_putchar(result, ch);
                    break;

                default: 
                    break;
            }
        }
        fclose(in);
    }
}

void expand(macro_dict *md, string_t *input, string_t *output) {
    // a buffer for macronames
    string_t *macroname = string_malloc(INITIAL_BUFFER_SIZE);
    string_t *argument1 = string_malloc(INITIAL_BUFFER_SIZE);
    string_t *argument2 = string_malloc(INITIAL_BUFFER_SIZE);

    int bracket_counter = 0;

    enum state curr = PLAINTEXT;
    bool escaped = false;

    int i = 0;

    // creates a pointer starting at the first character of the input
    while (i < input->size) {
        char *ch = &(input->data[i]);
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
                    // \\, \{, \}, \#, \%
                    if (*ch != '\\' && *ch != '{' && *ch != '}' && *ch != '#' && *ch != '%') {
                        string_putchar(output, '\\');
                    }
                    string_putchar(output, *ch);
                    curr = PLAINTEXT;
                }
                break;
            
            case MACRO:
                if (isalnum(*ch)) {
                    string_putchar(macroname, *ch);
                }
                else if (*ch == '{') {
                    // check if macro is def
                    if (strcmp(macroname->data, "def") == 0) {
                        string_clear(argument1);
                        curr = DEF_NAME;
                    }
                    // check if macro is undef
                    else if (strcmp(macroname->data, "undef") == 0) {
                        string_clear(argument1);
                        curr = UNDEF_NAME;
                    }
                    // check if macro is if
                    else if (strcmp(macroname->data, "if") == 0) {
                        int new_index = record_then_else(input, macroname, argument1, argument2, i+1);

                        // if non-empty --> then
                        if (*(ch + 1) != '}') {
                            string_t *new_input = string_malloc((input->size - i) + (argument1->size * 2));
                            string_putstring(new_input, argument1, 0);
                            string_putsubstring(new_input, input, new_index + 1, 0);
                            free(input->data);
                            *input = *new_input;
                            free(new_input);
                            // accounts for the fact that i will be incremented at very end of this iteration
                            i = -1;
                            curr = PLAINTEXT;
                        }

                        // if empty --> else
                        else {
                            string_t *new_input = string_malloc((input->size - i) + (argument2->size * 2));
                            string_putstring(new_input, argument2, 0);
                            string_putsubstring(new_input, input, new_index + 1, 0);
                            free(input->data);
                            *input = *new_input;
                            free(new_input);
                            // accounts for the fact that i will be incremented at very end of this iteration
                            i = -1;
                            curr = PLAINTEXT;
                        }
                    }

                    // check if macro is ifdef
                    else if (strcmp(macroname->data, "ifdef") == 0) {
                        int new_index = record_then_else(input, macroname, argument1, argument2, i+1);

                        // if macro is defined --> then
                        if (contains_macro(md, macroname)) {
                            string_t *new_input = string_malloc((input->size - i) + (argument1->size * 2));
                            string_putstring(new_input, argument1, 0);
                            string_putsubstring(new_input, input, new_index + 1, 0);
                            free(input->data);
                            *input = *new_input;
                            free(new_input);
                            // accounts for the fact that i will be incremented at very end of this iteration
                            i = -1;
                            curr = PLAINTEXT;
                        }
                        
                        // if macro not defined --> else
                        else {
                            string_t *new_input = string_malloc((input->size - i) + (argument2->size * 2));
                            string_putstring(new_input, argument2, 0);
                            string_putsubstring(new_input, input, new_index + 1, 0);
                            free(input->data);
                            *input = *new_input;
                            free(new_input);
                            // accounts for the fact that i will be incremented at very end of this iteration
                            i = -1;
                            curr = PLAINTEXT;
                        }
                    }

                    // check if macro is include
                    else if (strcmp(macroname->data, "include") == 0) {
                        // call include function
                        int curr_index = i + 1;
                        string_t *file_contents = include_file_contents(input, argument1, i+1, &curr_index);
                        string_t *new_input = string_malloc((input->size - i) + ((file_contents->size) * 2));
                        string_putstring(new_input, file_contents, 1);
                        string_putsubstring(new_input, input, curr_index + 1, 0);
                        free(input->data);
                        *input = *new_input;
                        free(new_input);
                        i = -1;
                        curr = PLAINTEXT;
                    }

                    // check if macro is expandafter
                    else if (strcmp(macroname->data, "expandafter") == 0) {
                        // call expandafter function
                        int start_index = i + 1;
                        expand_after(input, argument1, argument2, &start_index);
                        
                        string_t *after = string_copy(argument2);
                        string_t *after_expanded = string_malloc(INITIAL_BUFFER_SIZE);

                        expand(md, after, after_expanded);

                        string_free(after);

                        string_t *new_input = string_malloc((input->size - i) + ((argument1->size + argument2->size) * 2));
                        // string_putstring(new_input, argument2, 0);
                        string_putstring(new_input, argument1, 0);
                        string_putstring(new_input, after_expanded, 1);
                        string_putsubstring(new_input, input, start_index, 0);
                        free(input->data);
                        *input = *new_input;
                        free(new_input);
                        // accounts for the fact that i will be incremented at very end of this iteration
                        i = -1;
                        curr = PLAINTEXT;
                    }

                    // check if macro is user-defined
                    else if (contains_macro(md, macroname)) {
                        string_clear(argument2);
                        bracket_counter++;
                        curr = USER_ARG;
                    }
                    // check if macro is invalid
                    else {
                        DIE("Error: %s not defined", macroname->data);
                    }
                }
                else {
                    DIE("Error: %s", "Invalid character in Macroname.");
                }
                break;

            case DEF_NAME:
                if (isalnum(*ch)) {
                    string_putchar(argument1, *ch);
                }
                else if (*ch == '}') {
                    if (argument1->size == 0) {
                        DIE("Error: %s", "Invalid character in Macroname.");
                    }
                    if (contains_macro(md, argument1)) {
                        DIE("Error: %s", "Cannot redefine macro.");
                    }
                    else {
                        if (*(ch+1) != '{') {
                            DIE("Error: %s", "Expected {.");
                        }

                        string_clear(argument2);
                        curr = DEF_VALUE;
                        // skip the following {
                        i++;
                        bracket_counter++;
                    }
                }
                else {
                    DIE("Error: %s", "Character in macroname is not alphanumeric.");
                }
                break;

            case UNDEF_NAME:
                if (isalnum(*ch)) {
                    string_putchar(argument1, *ch);
                }
                else if (*ch == '}') {
                    if (argument1->size == 0) {
                        DIE("Error: %s", "Empty argument to undef.");
                    }
                    if (contains_macro(md, argument1)) {
                        delete_macro(md, argument1);
                        curr = PLAINTEXT;
                    }
                    else {
                        DIE("Error: %s", "Cannot undefine macro.");
                    }
                }
                else {
                    DIE("Error: %s", "Character in macroname is not alphanumeric.");
                }
                
                break;
            
            case DEF_VALUE:
                if (*ch == '\\') {
                    if (escaped == true) {
                        escaped = false;
                    }
                    else {
                        escaped = true;
                    }
                    string_putchar(argument2, *ch);
                }
                else if (*ch == '}') {
                    if (escaped == true) {
                        escaped = false;
                        string_putchar(argument2, *ch);
                    }
                    else {
                        bracket_counter--;
                        if (bracket_counter == 0) {
                            add_macro(md, argument1, argument2);
                            curr = PLAINTEXT;
                        }
                        else {
                            string_putchar(argument2, *ch);
                        }
                    }
                }
                else if (*ch == '{') {
                    if (escaped == true) {
                        escaped = false;
                    }
                    else {
                        bracket_counter++;
                    }
                    string_putchar(argument2, *ch);
                }
                else {
                    if (escaped == true) {
                        escaped = false;
                    }
                    string_putchar(argument2, *ch);
                }
                break;
            
            case USER_ARG:
                if (*ch == '\\') {
                    if (escaped == true) {
                        escaped = false;
                    }
                    else {
                        escaped = true;
                    }
                    string_putchar(argument2, *ch);
                }
                else if (*ch == '}') {
                    if (escaped == true) {
                        escaped = false;
                        string_putchar(argument2, *ch);
                    }
                    else {
                        bracket_counter--;
                        if (bracket_counter == 0) {
                            // argument is finished and time to replace #
                            // string_t *expanded = get_macro_value(md, macroname);

                            // replace #s here
                            string_t *expansion = replace_placeholders(get_macro_value(md, macroname), argument2);

                            string_t *new_input = string_malloc((input->size - i) + (expansion->size * 2));
                            string_putstring(new_input, expansion, 1);
                            string_putsubstring(new_input, input, i + 1, 0);
                            free(input->data);
                            *input = *new_input;
                            free(new_input);
                            // accounts for the fact that i will be incremented at very end of this iteration
                            i = -1;
                            curr = PLAINTEXT;
                        }
                        else {
                            string_putchar(argument2, *ch);
                        }
                    }
                }
                else if (*ch == '{') {
                    if (escaped == true) {
                        escaped = false;
                    }
                    else {
                        bracket_counter++;
                    }
                    string_putchar(argument2, *ch);
                }
                else {
                    if (escaped == true) {
                        escaped = false;
                    }
                    string_putchar(argument2, *ch);
                }
                break;
            
            default: 
                break;
        }
        i++;
    }
    if (curr == MACRO || curr == DEF_NAME || curr == DEF_VALUE || curr == USER_ARG || curr == UNDEF_NAME
        || curr == IF_COND) {
        DIE("Error: %s", "Unexpectedly found EOS.");
    }

    string_free(macroname);
    string_free(argument1);
    string_free(argument2);
}

string_t *replace_placeholders(string_t *expanded, string_t *arg) {
    if (expanded == NULL || arg == NULL) {
        DIE("Error: %s", "Invalid arguments to replace_placeholders function.");
    }

    string_t *new_result = string_malloc(INITIAL_BUFFER_SIZE);

    bool escaped = false;
    int i = 0;
    while (i < expanded->size) {
        char *ch = &(expanded->data[i]);
        
        if (*ch == '\\') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                escaped = true;
            }
            string_putchar(new_result, *ch);
        }
        else if (*ch == '#' && escaped == false) {
            string_putstring(new_result, arg, 0);
        }
        else {
            if (escaped == true) {
                escaped = false;
            }
            string_putchar(new_result, *ch);
        }
        i++;
    }

    return new_result;
}

int record_then_else(string_t *input, string_t *macroname, string_t *arg1, string_t *arg2, size_t index) {
    if (input == NULL || arg1 == NULL || arg2 == NULL || index < 0 || index >= input->size) {
        DIE("Error: %s", "Invalid arguments to record_then_else function.");
    }
    string_clear(macroname);
    string_clear(arg1);
    string_clear(arg2);
    
    bool escaped = false;
    int bracket_counter = 1;
    while (index < input->size) {
        char *ch = &(input->data[index]);
        if (*ch == '\\') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                escaped = true;
            }
            string_putchar(macroname, *ch);
        }
        else if (*ch == '}') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                bracket_counter --;
                if (bracket_counter == 0) {
                    if (*(ch + 1) != '{') {
                        DIE("Error: %s", "Expected {.");
                    }
                    index = index + 2;
                    break;
                }
            }
            string_putchar(macroname, *ch);
        }
        else if (*ch == '{') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                bracket_counter++;
            }
            string_putchar(macroname, *ch);
        }
        else {
            if (escaped == true) {
                escaped = false;
            }
            string_putchar(macroname, *ch);
        }
        index++;
    }
    if (index >= input->size) {
        DIE("Error: %s", "Found EOS.");
    }

    // record then statement in arg1
    escaped = false;
    bracket_counter = 1;
    while (index < input->size) {
        char *ch = &(input->data[index]);
        if (*ch == '\\') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                escaped = true;
            }
            string_putchar(arg1, *ch);
        }
        else if (*ch == '}') {
            if (escaped == true) {
                escaped = false;
                string_putchar(arg1, *ch);
            }
            else {
                bracket_counter--;
                if (bracket_counter == 0) {
                    if (*(ch + 1) != '{') {
                        DIE("Error: %s", "Expected {.");
                    }
                    index = index + 2;
                    break;
                }
                else {
                    string_putchar(arg1, *ch);
                }
            }
        }
        else if (*ch == '{') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                bracket_counter++;
            }
            string_putchar(arg1, *ch);
        }
        else {
            if (escaped == true) {
                escaped = false;
            }
            string_putchar(arg1, *ch);
        }
        index++;
    }
    if (index >= input->size) {
        DIE("Error: %s", "Found EOS.");
    }

    // record else statement in arg2
    bool finished = false;
    escaped = false;
    bracket_counter = 1;
    while (index < input->size) {
        char *ch = &(input->data[index]);
        if (*ch == '\\') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                escaped = true;
            }
            string_putchar(arg2, *ch);
        }
        else if (*ch == '}') {
            if (escaped == true) {
                escaped = false;
                string_putchar(arg2, *ch);
            }
            else {
                bracket_counter--;
                if (bracket_counter == 0) {
                    finished = true;
                    break;
                }
                else {
                    string_putchar(arg2, *ch);
                }
            }
        }
        else if (*ch == '{') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                bracket_counter++;
            }
            string_putchar(arg2, *ch);
        }
        else {
            if (escaped == true) {
                escaped = false;
            }
            string_putchar(arg2, *ch);
        }
        index++;
    }
    if (!finished) {
        DIE("Error: %s", "Expected }");
    }
    return index;
}

string_t *include_file_contents(string_t *input, string_t *arg1, int index, int *end_index) {
    if (input == NULL || arg1 == NULL || index >= input->size) {
        DIE("Error: %s", "Invalid arguments to include_file_contents function.");
    }
    string_clear(arg1);
    string_t *new_result = string_malloc(INITIAL_BUFFER_SIZE);

    bool escaped = false;
    bool finished = false;
    int bracket_counter = 1;
    while (index < input->size) {
        char *ch = &(input->data[index]);
        if (*ch == '\\') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                escaped = true;
            }
            string_putchar(arg1, *ch);
        }
        else if (*ch == '}') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                bracket_counter --;
                if (bracket_counter == 0) {
                    *end_index = index;
                    finished = true;
                    break;
                }
            }
            string_putchar(arg1, *ch);
        }
        else if (*ch == '{') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                bracket_counter++;
            }
            string_putchar(arg1, *ch);
        }
        else {
            if (escaped == true) {
                escaped = false;
            }
            string_putchar(arg1, *ch);
        }
        index ++;
    }
    if (!finished) {
        DIE("Error: %s", "Expected }");
    }

    FILE *in = fopen(arg1->data, "r");
    if (in == NULL) {
        DIE("Error: Could not open file %s", arg1->data);
    }

    int ch;
    while ((ch = fgetc(in)) != EOF) {
        string_putchar(new_result, ch);
    }
    fclose(in);
    string_t *final_result = string_delete_comments(new_result);
    string_free(new_result);
    return final_result;
}

void expand_after(string_t *input, string_t *arg1, string_t *arg2, int *index) {
    if (input == NULL || arg1 == NULL || arg2 == NULL || *index >= input->size) {
        DIE("Error: %s", "Invalid arguments to expand_after function.");
    }

    string_clear(arg1);
    string_clear(arg2);
    // record BEFORE argument in arg1
    int i = *index;
    bool escaped = false;
    bool finished = false;
    int bracket_counter = 1;

    while (i < input->size) {
        char *ch = &(input->data[i]);
        if (*ch == '\\') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                escaped = true;
            }
            string_putchar(arg1, *ch);
        }
        else if (*ch == '}') {
            if (escaped == true) {
                escaped = false;
                string_putchar(arg1, *ch);
            }
            else {
                bracket_counter--;
                if (bracket_counter == 0) {
                    if (*(ch + 1) != '{') {
                        DIE("Error: %s", "Expected {");
                    }
                    finished = true;
                    i = i + 2;
                    break;
                }
                else {
                    string_putchar(arg1, *ch);
                }
            }
        }
        else if (*ch == '{') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                bracket_counter++;
            }
            string_putchar(arg1, *ch);
        }
        else {
            if (escaped == true) {
                escaped = false;
            }
            string_putchar(arg1, *ch);
        }
        i++;
    }
    if (!finished) {
        DIE("Error: %s", "Expected }");
    }

    // record AFTER argument in arg2
    escaped = false;
    finished = false;
    bracket_counter = 1;

    while (i < input->size) {
        char *ch = &(input->data[i]);
        if (*ch == '\\') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                escaped = true;
            }
            string_putchar(arg2, *ch);
        }
        else if (*ch == '}') {
            if (escaped == true) {
                escaped = false;
                string_putchar(arg2, *ch);
            }
            else {
                bracket_counter--;
                if (bracket_counter == 0) {
                    finished = true;
                    break;
                }
                else {
                    string_putchar(arg2, *ch);
                }
            }
        }
        else if (*ch == '{') {
            if (escaped == true) {
                escaped = false;
            }
            else {
                bracket_counter++;
            }
            string_putchar(arg2, *ch);
        }
        else {
            if (escaped == true) {
                escaped = false;
            }
            string_putchar(arg2, *ch);
        }
        i++;
    }
    if (!finished) {
        DIE("Error: %s", "Expected }");
    }
    *index = i + 1;
}