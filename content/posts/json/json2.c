#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define keyoffset 8

enum token_type {
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, COLON,
    TOKEN_STRING, TOKEN_NUMBER,
    TOKEN_FALSE, TOKEN_TRUE,
    TOKEN_NULL, TOKEN_EOF
};

enum value_type {
    OBJECT, ARRAY,
    VALUE_STRING, VALUE_NUMBER,
    VALUE_TRUE, VALUE_FALSE,
    VALUE_NULL
};

typedef struct {
    int type;
    int line;
    char *lexeme;
} token;

typedef struct {
    int start;
    int current;
    int line;
    int capacity;
    int size;
    char *source;
    token *tokens;
} scanner;

typedef struct {
    int current;
    token *tokens;
} parser;

typedef struct value value;

typedef struct {
    char *string;
    value *value;
} member;

typedef struct {
    int capacity;
    int size;
    member *members;
} object;

typedef struct {
    int capacity;
    int size;
    value *elements;
} array;

struct value {
    int type;
    union {
        char *string;
        float number;
        object object;
        array array;
    };
};

typedef struct {
    int capacity;
    int size;
    char *string;
} string;

const char *keywords[] = { "false", "true", "null" };

// read file
int file_read(const char *filename, char **buf) {
    int ret = 0;
    FILE *f = fopen(filename, "r");
    if (!f) {
        return ret;
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    *buf = malloc((size + 1) * sizeof(char));
    if (!*buf) {
        return ret;
    }
    rewind(f);
    ret = fread(*buf, sizeof(char), size, f);
    fclose(f);
    (*buf)[size] = 0;
    return ret;
}

// go to the next character and return the current one
char scanner_advance(scanner *scanner) {
    return scanner->source[scanner->current++];
}

// check if we are done parsing the source
int scanner_is_at_end(scanner *scanner) {
    return scanner->current >= strlen(scanner->source);
}

// peek at the current character without advancing further
char scanner_peek(scanner *scanner) {
    if (scanner_is_at_end(scanner)) {
        return '\0';
    }
    return scanner->source[scanner->current];
}

// return a copy of s[start:end]
char *substring(char *s, int start, int end) {
    int size = end - start;
    char *t = malloc(size + 1);
    strncpy(t, s + start, size);
    t[size] = 0;
    return t;
}

// report a scanner error
void scanner_error(int line, char *msg) {
    fprintf(stderr, "[line %d]: %s\n", line, msg);
    exit(1);
}

// check if c is a digit
int isdigit(char c) {
    return c >= '0' && c <= '9';
}

// check if c is a letter or an underscore
int isalpha(char c) {
    return  (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c == '_');
}

// check if c is a digit or a letter
int isalnum(char c) {
    return isdigit(c) || isalpha(c);
}

// peek at one character after the next one
char scanner_peeknext(scanner *scanner) {
    if (scanner->current + 1 >= strlen(scanner->source)) {
        return '\0';
    }
    return scanner->source[scanner->current + 1];
}

// add the current token from the source string into scanner tokens
void add_token(scanner *scanner, int type) {
    char *text = substring(
        scanner->source, scanner->start, scanner->current
    );
    token token = {
        .type = type,
        .lexeme = text,
        .line = scanner->line,
    };
    if (scanner->size >= scanner->capacity) {
        scanner->capacity *= 2;
        scanner->tokens = realloc(
            scanner->tokens, scanner->capacity * sizeof(token)
        );
    }
    scanner->tokens[scanner->size++] = token;
}

// scan a string token from the source string
void scanner_string(scanner *scanner) {
    while (scanner_peek(scanner) != '"' && !scanner_is_at_end(scanner)) {
        if (scanner_peek(scanner) == '\n') {
            scanner->line++;
        }
        scanner_advance(scanner);
    }
    if (scanner_is_at_end(scanner)) {
        scanner_error(scanner->line, "unterminated string");
    }
    scanner_advance(scanner);
    scanner->start++;
    scanner->current--;
    add_token(scanner, TOKEN_STRING);
    scanner->start--;
    scanner->current++;
}

// scan a number token from the source string
void scanner_number(scanner *scanner) {
    while (isdigit(scanner_peek(scanner))) {
        scanner_advance(scanner);
    }
    if (scanner_peek(scanner) == '.' && isdigit(scanner_peeknext(scanner))) {
        scanner_advance(scanner);
        while (isdigit(scanner_peek(scanner))) {
            scanner_advance(scanner);
        }
    }
    add_token(scanner, TOKEN_NUMBER);
}

// return the type of the keyword
int key_type(char *key) {
    int nkeys = sizeof(keywords) / sizeof(char *);
    for (int i = 0; i < nkeys; i++) {
        if (!strcmp(key, keywords[i])) {
            return keyoffset + i;
        }
    }
    return -1;
}

// scan identifier
void identifier(scanner *scanner) {
    while (isalnum(scanner_peek(scanner))) {
        scanner_advance(scanner);
    }
    char *text = substring(
        scanner->source, scanner->start, scanner->current
    );
    int type = key_type(text);
    if (type == -1) {
        char line[128];
        sprintf(line, "unexpected identifier: %s", text);
        scanner_error(scanner->line, line);
    }
    add_token(scanner, type);
    free(text);
}

// scan token
void scan_token(scanner *scanner) {
    char c = scanner_advance(scanner);
    switch(c) {
    case '{':
        add_token(scanner, LEFT_BRACE);
        break;
    case '}':
        add_token(scanner, RIGHT_BRACE);
        break;
    case '[':
        add_token(scanner, LEFT_BRACKET);
        break;
    case ']':
        add_token(scanner, RIGHT_BRACKET);
        break;
    case ',':
        add_token(scanner, COMMA);
        break;
    case ':':
        add_token(scanner, COLON);
        break;
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        scanner->line++;
        break;
    case '"':
        scanner_string(scanner);
        break;
    case '-':
    case '+':
        if (isdigit(scanner_peeknext(scanner))) {
            scanner_advance(scanner);
            scanner_number(scanner);
        }
    default:
        if (isdigit(c)) {
            scanner_number(scanner);
        } else if (isalpha(c)) {
            identifier(scanner);
        } else {
            char msg[128];
            sprintf(msg, "unexpected character: %c", c);
            scanner_error(scanner->line, msg);
        }
        break;
    }
}

// scan tokens
void scan_tokens(scanner *scanner) {
    while (!scanner_is_at_end(scanner)) {
        scanner->start = scanner->current;
        scan_token(scanner);
    }
    scanner->start = scanner->current;
    add_token(scanner, TOKEN_EOF);
}

// print tokens
void print_tokens(scanner *scanner) {
    for (int i = 0; i < scanner->size; i++) {
        token token = scanner->tokens[i];
        printf("type: %d\n", token.type);
        printf("lexeme: %s\n", token.lexeme);
        printf("line: %d\n", token.line);
    }
}

// peek at the current token
token parser_peek(parser *parser) {
    return parser->tokens[parser->current];
}

// check if we are done
int parser_is_at_end(parser *parser) {
    return parser_peek(parser).type == TOKEN_EOF;
}

// check if the current token has appropriate type
int check(parser *parser, int type) {
    if (parser_is_at_end(parser)) {
        return 0;
    }
    return parser_peek(parser).type == type;
}

// get previous token
token previous(parser *parser) {
    return parser->tokens[parser->current - 1];
}

// go to the next token
token parser_advance(parser *parser) {
    if (!parser_is_at_end(parser)) {
        parser->current++;
    }
    return previous(parser);
}

// report a parser error
void parser_error(token token, char *msg) {
    fprintf(stderr, "[line %d] at '%s': %s\n", token.line, token.lexeme, msg);
    exit(1);
}

// consume the current token
token consume(parser *parser, int type, char *msg) {
    if (check(parser, type)) {
        return parser_advance(parser);
    }
    parser_error(parser_peek(parser), msg);
    return (token){0};
}

// add a value to an array
void array_add_value(array *array, value *value) {
    if (array->size >= array->capacity) {
        array->capacity *= 2;
        array->elements = realloc(
            array->elements, array->capacity * sizeof(*value)
        );
    }
    array->elements[array->size++] = *value;
}

// add a member to an object
void object_add_member(object *object, member *member) {
    if (object->size >= object->capacity) {
        object->capacity *= 2;
        object->members = realloc(
            object->members, object->capacity * sizeof(*member)
        );
    }
    object->members[object->size++] = *member;
}

void parse_value(parser *, value *);

// parse elements
void parse_elements(parser *parser, array *array) {
    if (parser_peek(parser).type == RIGHT_BRACKET) {
        return;
    }
    value value = {0};
    parse_value(parser, &value);
    array_add_value(array, &value);
    while (parser_peek(parser).type == COMMA) {
        parser_advance(parser);
        parse_value(parser, &value);
        array_add_value(array, &value);
    }
}

// parse member
void parse_member(parser *parser, member *member) {
    token string = consume(parser, TOKEN_STRING, "expected string");
    member->string = strdup(string.lexeme);
    consume(parser, COLON, "expected colon");
    member->value = calloc(1, sizeof(value));
    parse_value(parser, member->value);
}

// parse members
void parse_members(parser *parser, object *object) {
    if (parser_peek(parser).type == RIGHT_BRACE) {
        return;
    }
    member member = {0};
    parse_member(parser, &member);
    object_add_member(object, &member);
    while (parser_peek(parser).type == COMMA) {
        parser_advance(parser);
        parse_member(parser, &member);
        object_add_member(object, &member);
    }
}

// parse object
void parse_object(parser *parser, value *value) {
    value->type = OBJECT;
    value->object.capacity = 4;
    value->object.members = malloc(4 * sizeof(member));
    consume(parser, LEFT_BRACE, "expected left brace");
    parse_members(parser, &value->object);
    consume(parser, RIGHT_BRACE, "expected right brace");
}

// parse array
void parse_array(parser *parser, value *value) {
    value->type = ARRAY;
    value->array.capacity = 4;
    value->array.elements = malloc(4 * sizeof(*value));
    consume(parser, LEFT_BRACKET, "expected left bracket");
    parse_elements(parser, &value->array);
    consume(parser, RIGHT_BRACKET, "expected right bracket");
}

// parse value
void parse_value(parser *parser, value *value) {
    token token = parser_peek(parser);
    enum token_type type = token.type;
    switch (type) {
    case LEFT_BRACE:
        parse_object(parser, value);
        break;
    case LEFT_BRACKET:
        parse_array(parser, value);
        break;
    case TOKEN_STRING:
        parser_advance(parser);
        value->type = VALUE_STRING;
        value->string = strdup(token.lexeme);
        break;
    case TOKEN_NUMBER:
        parser_advance(parser);
        value->type = VALUE_NUMBER;
        value->number = atof(token.lexeme);
        break;
    case TOKEN_TRUE:
        parser_advance(parser);
        value->type = VALUE_TRUE;
        break;
    case TOKEN_FALSE:
        parser_advance(parser);
        value->type = VALUE_FALSE;
        break;
    case TOKEN_NULL:
        value->type = VALUE_NULL;
        break;
    default:
        printf("unexpected token: %s\n", token.lexeme);
        exit(1);
    }
}

// free parser data
void free_parser(parser *parser) {
    for (int i = 0; i < parser->current + 1; i++) {
        free(parser->tokens[i].lexeme);
    }
    free(parser->tokens);
}

// concatenate to the string
void string_cat(string *string, char *msg) {
    int size = strlen(msg);
    if (string->size + size >= string->capacity) {
        string->capacity += size;
        string->capacity *= 2;
        string->string = realloc(
            string->string, string->capacity
        );
    }
    string->string[string->size] = 0;
    string->size += size;
    strcat(string->string, msg);
}

// print string to the standard output
void string_print(string *string) {
    puts(string->string);
}

// free data from string
void free_string(string *string) {
    free(string->string);
}

void value_string(value *value, string *string, int ind);

// concatenate array to the string
void array_string(array *array, string *string, int ind) {
    string_cat(string, "[ ");
    for (int i = 0; i < array->size; i++) {
        value value = array->elements[i];
        value_string(&value, string, ind);
        if (i < array->size - 1) {
            string_cat(string, ", ");
        }
    }
    string_cat(string, " ]");
}

// concatenate object to the string
void object_string(object *object, string *string, int ind) {
    char keystr[64] = {0};
    string_cat(string, "{ ");
    if (object->size > 0) {
        string_cat(string, "\n");
    }
    for (int i = 0; i < object->size; i++) {
        member member = object->members[i];
        for (int j = 0; j < ind + 1; j++) {
            string_cat(string, "    ");
        }
        sprintf(keystr, "%s: ", member.string);
        string_cat(string, keystr);
        value_string(member.value, string, ind + 1);
        if (i < object->size - 1) {
            string_cat(string, ",");
        }
        string_cat(string, "\n");
    }
    for (int i = 0; i < ind; i++) {
        string_cat(string, "    ");
    }
    string_cat(string, "}");
}

// concatenate value to the string
void value_string(value *value, string *string, int ind) {
    char valstr[64] = {0};
    switch (value->type) {
    case ARRAY:
        array_string(&value->array, string, ind);
        break;
    case OBJECT:
        object_string(&value->object, string, ind);
        break;
    case VALUE_NUMBER:
        sprintf(valstr, "%f", value->number);
        string_cat(string, valstr);
        break;
    case VALUE_STRING:
        sprintf(valstr, "\"%s\"", value->string);
        string_cat(string, valstr);
        break;
    case VALUE_FALSE:
        string_cat(string, "false");
        break;
    case VALUE_TRUE:
        string_cat(string, "true");
        break;
    case VALUE_NULL:
        string_cat(string, "null");
        break;
    }
}

// free data from value
void free_value(value *value) {
    switch(value->type) {
    case ARRAY:
        for (int i = 0; i < value->array.size; i++) {
            free_value(&value->array.elements[i]);
        }
        free(value->array.elements);
        break;
    case OBJECT:
        for (int i = 0; i < value->object.size; i++) {
            free(value->object.members[i].string);
            free_value(value->object.members[i].value);
            free(value->object.members[i].value);
        }
        free(value->object.members);
        break;
    case VALUE_STRING:
        free(value->string);
        break;
    }
}

// parse json string
void parse_json(const char *buffer, value *value) {
    scanner scanner = {
        .line = 1,
        .source = (char *) buffer,
        .capacity = 4,
        .tokens = malloc(4 * sizeof(token))
    };
    scan_tokens(&scanner);
    parser parser = {.tokens = scanner.tokens};
    parse_value(&parser, value);
    free_parser(&parser);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [file.json]\n", argv[0]);
        return 1;
    }
    char *source = NULL;
    int fsize = file_read(argv[1], &source);
    if (!fsize) {
        fprintf(stderr, "failed to read file %s\n", argv[1]);
        return 1;
    }
    value value = {0};
    parse_json(source, &value);
    string string = {
        .capacity = 64,
        .string = malloc(64)
    };
    value_string(&value, &string, 0);
    string_print(&string);
    free_value(&value);
    free_string(&string);
    free(source);
    return 0;
}