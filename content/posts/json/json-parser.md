+++
date = '2025-02-03T00:00:08+03:00'
draft = false
title = 'Json Parser'
+++

I am fairly familiar with JSON file format. It is used extensively for data exchange. If you have some level of experience with development, you are probably familiar with it as well. Among the benefits of JSON format:

- It is easy to read and write
- It is easy to parse and construct

I often hear that JSON is a quite simple format, and parsing it is not hard. In the recent series of posts, I already tackled parsing OBJ files. So, I became genuinely curious whether it's possible to write a JSON parser from scratch.

Before I started to do it, I thought that it was quite a daunting task. I couldn't even think about where to start. You may ask me: "What is so difficult about it? You already implemented a parser for another file format." The problem is that JSON syntax is much less rigorous compared to OBJ. All lines in OBJ file **must** have this format per specification:

```
key value1 value2 ... valuen
```

There is no space between the start of the line and the key. All values are delimited by exactly one space. More than that, many types of data have rigorous requirements as well (for example, vertex position must be a vector with *exactly* three coordinates).

In contrast, JSON syntax specification is much less rigorous. Between two tokens, you can have one space, or two, or tab, or entire newline, or no spaces at all. This means that we can't just parse stuff in place, we now need a *scanner* - a piece of software that discards all whitespaces and leaves only *tokens* - keywords and symbols.

Data that is stored in JSON has much less rigorous specifications as well: there are no pre-defined keys that would tell you a type of data. You have to infer a data type from the context, and it can be much more flexible as well. A value field can store:

- Array
- Object
- Number
- String
- True or False
- Null

I think that you probably now have an idea about why parsing JSON seemed like a daunting task to me. Despite me being upset, I recalled that some time ago I read a wonderful book. It turned out to be of huge help.

This book is [Crafting Interpreters](https://craftinginterpreters.com) by Robert Nystrom. Former game developer at Electronic Arts, Robert now works on the [Dart Language](https://dart.dev), so I probably would take it as a sign that he knows what he's doing. This book is easy to read, it has a lot of good code with nice explanations, and most important of all, this book is free! Don't just trust my words, try to read it and follow along. The original interpreter is implemented in Java, but there are [many other implementations](https://github.com/munificent/craftinginterpreters/wiki/Lox-implementations) as well, so you can follow along in any language. I would *recommend* that you do it unless are a huge fan of Java, of course; this will allow you to *think* about what you are doing instead of blindly typing the code. You can also gain some experience of reading a source code as well.

The book describes an implementation of the full-fledged programming language. However, it's much more useful than that. In fact, the entire scanner with the basic framework of the parser is borrowed from this book.

Today we are going to implement a scanner. As I mentioned earlier, the scanner is responsible for breaking an input into *tokens* - keywords and special characters. Here is a simple example. Consider that we have a text like this:

```
{ "key": [ 3, 14, 15 ], "object": { "type": "nested" } }
```

If we process this string with a scanner, here is what we get:

```
['{', "key", ':', '[', 3, ',', 14, ',', ... ]
```

It does not matter how many spaces (if any) are between tokens. The scanner will discard all whitespace characters and only leave tokens.

As always, create a new directory and a new source file inside it. Include all necessary headers:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
```

Before we describe what a token looks like let's define token types with an enum:

```c
enum token_type {
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, COLON,
    TOKEN_STRING, TOKEN_NUMBER,
    TOKEN_FALSE, TOKEN_TRUE,
    TOKEN_NULL, TOKEN_EOF
};
```

We will represent a token in a very simple way:

```c
typedef struct {
    int type;       // token type from enum above
    int line;       // line where token is located
    char *lexeme;   // token lexeme
} token;
```

Here, `lexeme` is a string representation of a token. For example, a token that represents an integer `42` will have lexeme `"42"`.

We need another data structure, called `scanner`, to represent a state machine that is responsible for scanning the source code:

```c
typedef struct {
    int start;      // start of the current token
    int current;    // end of the current token
    int line;       // current line
    int capacity;   // capacity of tokens array
    int size;       // size of tokens array
    char *source;   // json source string
    token *tokens;  // tokens
} scanner;
```

Here, `start` and `current` are the borders of the current token being processed. Here is an example:

```
["Hello", "world"]
```

In this example, the borders of the token `"world"` are `9` and `15`.

Fields `capacity` and `size` are here because the `tokens` field is a dynamic array. This does waste a bit of memory and reallocation is also a costly operation. However, we will need to access tokens by indices a lot when we do the parsing, so this is an acceptable tradeoff.

Before we go further, define an array of keywords. We'll need it later:

```c
const char *keywords[] = { "false", "true", "null" };
```

In our program, we will read a JSON file and scan it. Let's define functions to read a text from a file:

```c
char *file_read(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("failed to open file");
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        perror("failed to allocate memory");
        return NULL;
    }
    rewind(f);
    size = fread(buffer, sizeof(char), size, f);
    if (!size) {
        perror("failed to read file");
        return NULL;
    }
    buffer[size] = 0;
    fclose(f);
    return buffer;
}
```

If you read my other posts, the function `file_read` is probably familiar to you. It calculates the size of the file using a combination of `fseek` and `ftell`, first to set the internal pointer to the end of the file, and second to get the offset from the start. `rewind` sets the internal pointer back to the start. Then we allocate enough space with `malloc`, read a file with `fread`, and close it with `fclose`. Notice one line above `return`. `strrchr` returns a pointer to the rightmost occurrence of the specified character. We use it to terminate all symbols after the last right brace since `ftell` returns a slightly larger number than needed on Windows (because `\r\n` is replaced with `\n` but `ftell` does not consider it).

Now let's consider some utility functions:

```c
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
```

Almost all of those functions are simple enough to read, so I don't think that any comments are necessary.

There is one function, however, that deserves some attention. `substring` accepts three arguments: a string, a start position, and an end position. It returns a *copy* of `s[start:end]`. We will use `substring` primarily to write token lexemes. Since our tokens must be independent of the source string (we will use them later in the parser), we obtain copies of substrings that represent tokens. `size` can be calculated from the position arguments. We add one to the size since we need space to store the terminating null byte. Then we use `strncpy` to copy `size` bytes from `s + start` and set the last byte of our string to zero to terminate it.

Here are some more utility functions that we'll need later:

```c
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

// peek at one character after the current one
char scanner_peeknext(scanner *scanner) {
    if (scanner->current + 1 >= strlen(scanner->source)) {
        return '\0';
    }
    return scanner->source[scanner->current + 1];
}
```

Again, most of those functions are simple enough to read. Their purpose and how they work is fairly obvious from the source code.

Here comes the interesting part. Let's create a new function called `add_token`:

```c
// add the current token from the source string into scanner tokens
void add_token(scanner *scanner, int type) {
    // copy the lexeme substring
    char *lexeme = substring(
        scanner->source, scanner->start, scanner->current
    );
    // create a new token
    token token = {
        .type = type,
        .lexeme = lexeme,
        .line = scanner->line,
    };
    // if size exceeds capacity
    if (scanner->size >= scanner->capacity) {
        // double the capacity
        scanner->capacity *= 2;
        // reallocate the memory
        scanner->tokens = realloc(
            scanner->tokens, scanner->capacity * sizeof(token)
        );
    }
    // append the token to the tokens array
    scanner->tokens[scanner->size++] = token;
}
```

The first line of the body of this function uses the `substring` function defined above. We use the `start` and `current` fields of the parser to denote the position of the current token being processed.

Then we create a new token using the `lexeme` substring, the `token_type` argument, and the current line of the scanner.

Take a look at the conditional statement. This is a typical dynamic array reallocation code. Here is what it's doing:

```
If the current array size >= the array capacity:
    Double the capacity of the array
    Reallocate the array with the new capacity
```

Note the `realloc` function. It allocates a new block of memory with the provided size and then copies the contents of the provided memory block here. 

The last line copies the `token` in the last cell of the `tokens` array and increases the array size. This is a typical function that implements appending an element to a dynamic array.

Create a new function called `scanner_string`:

```c
// scan a string token from the source string
void scanner_string(scanner *scanner) {
    // while the current character is not a quote and we are not done yet
    while (scanner_peek(scanner) != '"' && !scanner_is_at_end(scanner)) {
        // increase the scanner line if the newline character encountered
        if (scanner_peek(scanner) == '\n') {
            scanner->line++;
        }
        // advance the scanner
        scanner_advance(scanner);
    }
    // if we are done but have not met the terminating quote character
    if (scanner_is_at_end(scanner)) {
        // there is an error
        scanner_error(scanner->line, "unterminated string");
    }
    // consume the terminating quote character
    scanner_advance(scanner);
    // trim quotes so that lexeme only contains the string
    scanner->start++;
    scanner->current--;
    // add the new token
    add_token(scanner, TOKEN_STRING);
    // restore scanner positions
    scanner->start--;
    scanner->current++;
}
```

We assume that the opening quote is already consumed. Try to walk through an example, like "hello, world". Consider the edge case when the string is not terminated. Remember that `advance` increases the scanner starting position while `peek` only peeks at the current character.

Now let's consider scanning numbers:

```c
// scan a number token from the source string
void scanner_number(scanner *scanner) {
    // while the current character is a digit
    while (isdigit(scanner_peek(scanner))) {
        // advance the scanner
        scanner_advance(scanner);
    }
    // if the current character is a dot and
    // one character after it is a digit
    if (scanner_peek(scanner) == '.' && isdigit(scanner_peeknext(scanner))) {
        // consume the dot
        scanner_advance(scanner);
        // while the current character is a digit
        while (isdigit(scanner_peek(scanner))) {
            // advance the scanner
            scanner_advance(scanner);
        }
    }
    // add the new token
    add_token(scanner, TOKEN_NUMBER);
}
```

If there is a sign character, we assume that it is already consumed. Again, try to walk through an example like `3.141572`. Consider an edge case like `517.a`.

Let's define a new function. It will accept a string, and will return the keyword type of that string:

```c
// first keyword is FALSE and all other keywords
// follow after that
#define keyoffset TOKEN_FALSE

// return the type of the keyword
int key_type(char *key) {
    // number of elements in keywords array
    int nkeys = sizeof(keywords) / sizeof(char *);
    // compare the key against each known keyword
    for (int i = 0; i < nkeys; i++) {
        // if it matches
        if (!strcmp(key, keywords[i])) {
            // return its type
            return keyoffset + i;
        }
    }
    // keyword type is unknown
    return -1;
}
```

Let's say that we have some keyword and we want to determine its type. This function is fairly simple. It compares the provided `key` with each keyword from the `keyword` global array we defined earlier. If there is a match, it returns the type of the keyword, and otherwise it returns `-1`. Note the `keyoffset` symbol. The first keyword defined in the `token_type` enum is `TOKEN_FALSE`, and all other keywords follow after that. Note that the order of tokens matches with order of defined `keywords`. This allows us to use `keyoffset` to calculate the keyword position in the array as the token type.

Now let's consider identifiers. Perhaps, this is more appropriate to call them keywords since we don't support variables and other symbols:

```c
// scan identifier
void identifier(scanner *scanner) {
    // while the current character is a letter or a digit
    while (isalnum(scanner_peek(scanner))) {
        // advance the scanner
        scanner_advance(scanner);
    }
    // copy the current token
    char *text = substring(
        scanner->source, scanner->start, scanner->current
    );
    // determine its type
    int type = key_type(text);
    // if it is -1, we couldnt determine the type of the identifier
    if (type == -1) {
        // report a formatted error
        char line[64];
        sprintf(line, "unexpected identifier: %s", text);
        scanner_error(scanner->line, line);
    }
    // add the current token into the scanner
    add_token(scanner, type);
    // free text
    free(text);
}
```

Now for the skeleton of our scanner:

```c
// scan token
void scan_token(scanner *scanner) {
    // consider the next character in the source string
    char c = scanner_advance(scanner);
    switch(c) {
    // single characters
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
    // ignore whitespace characters
    case ' ':
    case '\r':
    case '\t':
        break;
    // increase the scanner line
    case '\n':
        scanner->line++;
        break;
    // parse the string
    case '"':
        scanner_string(scanner);
        break;
    // if we encounter the number sign character
    case '-':
    case '+':
        // and the next character is a digit
        if (isdigit(scanner_peeknext(scanner))) {
            // consume the sign character
            scanner_advance(scanner);
            // scan the number token
            scanner_number(scanner);
        }
    default:
        // if the character is a digit
        if (isdigit(c)) {
            // scan the number
            scanner_number(scanner);
        // if it is a letter
        } else if (isalpha(c)) {
            // scan the identifier
            identifier(scanner);
        // otherwise report an error
        } else {
            char msg[128];
            sprintf(msg, "unexpected character %c", c);
            scanner_error(scanner->line, msg);
        }
        break;
    }
}
```

Since most of the job is done inside of the functions, the parsing function is relatively simple.

Here is a wrapper of the function above that scans tokens in a loop:

```c
// scan tokens
void scan_tokens(scanner *scanner) {
    // while we are not done
    while (!scanner_is_at_end(scanner)) {
        // set starting position to the current position
        scanner->start = scanner->current;
        // scan the next token
        scan_token(scanner);
    }
    // create an empty string token
    scanner->start = scanner->current;
    // add an EOF terminating token
    add_token(scanner, TOKEN_EOF);
}
```

Let's define a function that will print all of the scanner's tokens:

```c
// print tokens
void print_tokens(scanner *scanner) {
    for (int i = 0; i < scanner->size; i++) {
        token token = scanner->tokens[i];
        printf("type: %d\n", token.type);
        printf("lexeme: %s\n", token.lexeme);
        printf("line: %d\n", token.line);
    }
}
```

Don't forget to clean up the scanner:

```c
// free scanner data
void free_scanner(scanner *scanner) {
    for (int i = 0; i < scanner->size; i++) {
        free(scanner->tokens[i].lexeme);
    }
    free(scanner->tokens);
}
```

Now let's define a function that will parse the JSON string:

```c
// parse json string
void parse_json(const char *source) {
    scanner scanner = {
        .line = 1,
        .source = source,
        .capacity = 4,
        .tokens = malloc(4 * sizeof(token))
    };
    scan_tokens(&scanner);
    print_tokens(&scanner);
    free_scanner(&scanner);
}
```

Finally, write the `main` function:

```c
int main(int argc, char **argv) {
    // check the usage
    if (argc != 2) {
        printf("usage: %s [file.json]\n", argv[0]);
        return 1;
    }
    // get the contents of the provided file
    char *source = file_read(argv[1]);
    if (!source) {
        return 1;
    }
    // parse json string
    parse_json(source);
    // free the source string
    free(source);
    return 0;
}
```

Phew, that was a lot of work. Check out the [source code with samples](https://github.com/silentstranger5/jsonparser)