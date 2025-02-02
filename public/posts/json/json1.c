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

const char *keywords[] = { "false", "true", "null" };

// read file
int file_read(const char *filename, char **buffer) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    *buffer = malloc(size * sizeof(char));
    rewind(f);
    int ret = fread(*buffer, sizeof(char), size, f);
    fclose(f);
    *(strrchr(*buffer, '}') + 1) = 0;
    return ret;
}

// return file contents
char *file_content(const char *filename) {
    char *buffer = NULL;
    int size = file_read(filename, &buffer);
    if (!size) {
        return NULL;
    }
    return buffer;
}

// go to the next token and return the current one
char scanner_advance(scanner *scanner) {
    return scanner->source[scanner->current++];
}

// check if we are done parsing the source
int scanner_is_at_end(scanner *scanner) {
    return scanner->current >= strlen(scanner->source);
}

// peek at the current token without advancing further
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

// peek at one token after the next one
char scanner_peeknext(scanner *scanner) {
    if (scanner->current + 1 >= strlen(scanner->source)) {
        return '\0';
    }
    return scanner->source[scanner->current + 1];
}

// add the current token from the source string into scanner tokens
void add_token(scanner *scanner, int token_type) {
    char *text = substring(
        scanner->source, scanner->start, scanner->current
    );
    token token = {
        .type = token_type,
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
    char *numstr = substring(
        scanner->source, scanner->start, scanner->current
    );
    add_token(scanner, TOKEN_NUMBER);
}

// return the type of the keyword
int get_keytype(char *key) {
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
    int type = get_keytype(text);
    if (type == -1) {
        char line[128];
        sprintf(line, "unexpected identifier: %s", text);
        scanner_error(scanner->line, line);
    }
    add_token(scanner, type);
}

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
            sprintf(msg, "unexpected character %c", c);
            scanner_error(scanner->line, msg);
        }
        break;
    }
}

void scan_tokens(scanner *scanner) {
    while (!scanner_is_at_end(scanner)) {
        scanner->start = scanner->current;
        scan_token(scanner);
    }
    scanner->start = scanner->current;
    add_token(scanner, TOKEN_EOF);
}

void print_tokens(scanner *scanner) {
    for (int i = 0; i < scanner->size; i++) {
        token token = scanner->tokens[i];
        printf("type: %d\n", token.type);
        printf("lexeme: %s\n", token.lexeme);
        printf("line: %d\n", token.line);
    }
}

void parse_json(const char *source) {
    scanner scanner = {
        .line = 1,
        .source = source,
        .capacity = 8,
        .tokens = malloc(8 * sizeof(token))
    };
    scan_tokens(&scanner);
    print_tokens(&scanner);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [file.json]\n", argv[0]);
        return 1;
    }
    char *source = file_content(argv[1]);
    if (!source) {
        fprintf(stderr, "failed to read file %s\n", argv[1]);
        return 1;
    }
    parse_json(source);
    free(source);
    return 0;
}