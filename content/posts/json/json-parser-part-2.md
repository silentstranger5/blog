+++
date = '2025-02-03T20:17:37+03:00'
draft = false
title = 'Json Parser Part 2'
+++

In the previous post, we wrote a *scanner* for our JSON parser. In this post, we will finally *parse* our tokens - produce actual data from them that we can use.

Firstly, let's define enum with possible types of our values:

```c
// add this after token_type enum
enum value_type {
    OBJECT, ARRAY,
    VALUE_STRING, VALUE_NUMBER,
    VALUE_TRUE, VALUE_FALSE,
    VALUE_NULL
};
```

Similar to our `scanner` structure that represents a state machine while scanning the source string, let's define a new structure called `parser`. It's going to be really simple:

```c
// add this after the scanner
typedef struct {
    int current;    // index of the current token
    token *tokens;  // tokens
} parser;
```

Let's think about how we'll define our data. JSON specification from the website above will come in handy for this:

```
JSON
    element

value
    object
    array
    string
    number
    "true"
    "false"
    "null"

object
    '{' members '}'

members
    member
    member ',' members

member
    string ':' element

array
    '[' elements ']'

elements
    value
    value ',' value
```

Note that I've simplified it and discarded all whitespaces since all tokens are already scanned.

Since `value` is a fundamental building block for other compound types, let's declare it:

```c
typedef struct value value;
```

Now we can define all other structures:

```c
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
```

Note that `array` and `object` are similar. They are both implemented as dynamic arrays. It's perfect for `array`, but suboptimal for `object` since it's intended to be a dictionary structure. Perhaps hash maps are better for this, but I'm trying to keep this implementation small and simple. With some additional effort, you can implement it as a hashmap in the future.

Now that compound types are defined, we can finally implement the `value`. Do you remember the `value_type` enum mentioned earlier? It will come in handy here. Since a value can take on any of some possible data types, there is one feature that is perfect for its implementation. It's called a *union*. Union stores all types of fields in one place and interprets the data according to the field. It takes as much space as the largest field, which allows to save a lot of space. We can then wrap it in the structure with one additional field that will store a type of our structure. Here is how our `value` structure looks like:

```c
struct value {
    int type;
    union {
        char *string;
        float number;
        object object;
        array array;
    };
};
```

Note that the union here is anonymous and embedded directly into the `value` structure. This allows one to access union fields directly like this: `value->string`.

Now let's define utility functions for our parser:

```c
// add all code below after the print_tokens function

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
```

Those functions should be simple to read and understand. I think that no more comments are necessary.

Now that we're done with utility functions, let's handle adding new values to `array` and `object:

```c
// add the value to the array
void array_add_value(array *array, value *value) {
    // if the size exceeds the capacity
    if (array->size >= array->cpacity) {
        // double the capacity
        array->capacity *= 2;
        // reallocate the memory
        array->elements = realloc(
            array->elements, array->capacity * sizeof(*value)
        );
    }
    // append the value to the array
    array->elements[array->size++] = *value;
}

// add the member to the object
void object_add_member(object *object, member *member) {
    if (object->size >= object->capacity) {
        object->capacity *= 2;
        object->members = realloc(
        object->members, object->capacity * sizeof(*member)
    );
    }
    object->members[object->size++] = *member;
}
```

Recall that the conditional statement contains the reallocation code. The last line appends the object to the dynamic array.

Now let's parse `elements` and `members`:

```c
void parse_value(parser *, value *);

// parse elements
void parse_elements(parser *parser, array *array) {
    // if the current token is the right bracket, we are done
    if (parser_peek(parser).type == RIGHT_BRACKET) {
        return;
    }
    // parse the new value
    value value = {0};
    parse_value(parser, &value);
    // append the value to the array
    array_add_value(array, &value);
    // while there is a comma, there are more values to parse
    while (parser_peek(parser).type == COMMA) {
        // consume the comma token
        parser_advance(parser);
        // parse the new value
        parse_value(parser, &value);
        // append the value to the array
        array_add_value(array, &value);
    }
}
```

This type of parsing is almost a direct translation of this syntactical clause:

```
array
    '[ ]'
    '[' elements ']'

elements
    value
    value ',' value
```

The array consists of brackets with optional elements inside. Elements can be either a single value or a list of values delimited by commas. Here is how we parse it:

1. Check whether there are any elements to parse. If not, we are done
2. Parse the value and append it to the array
3. While the current token is a comma:
    - Advance the parser
    - Parse the new value
    - Append it to the array

This pattern is typical for parsing the comma-delimited values. Now let's handle the parsing of a member (an entry of an object):

```c
// parse member
void parse_member(parser *parser, member *member) {
    // consume the member key string and report an error if it's not present
    token string = consume(parser, TOKEN_STRING, "expected string");
    // copy the string token lexeme
    member->string = strdup(string.lexeme);
    // consume the colon token
    consume(parser, COLON, "expected colon");
    // allocate memory for member's value
    member->value = calloc(1, sizeof(value));
    // parse the value
    parse_value(parser, &value);
}
```

Note that we allocate memory for member's value field. In the beginning of this post, we resolved circular data type definition by forward declaration of the `value` type. Because of that, we have to use a pointer for the `value` field in the `member` structure and allocate it on the heap. This is unfortunate, but still better that the other way around. Had we declared `array` and `object` structures, amount of heap allocations would grow significantly. The bottom line here is simple: allocate on the stack everything that you can (unless you need more storage).

This code is very similar to this syntactical clause of JSON:

```
member
    string ':' element
```

Finally, let's wrap it inside a new function that will parse a list of comma-delimited members:

```c
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
```

This code is very similar to `parse_elements`, so I don't think there is much to comment on.

Now that we can parse an optional comma-delimited list of members and elements, we can finally define functions to parse `objects` and `arrays`:

```c
// parse object
void parse_object(parser *parser, value *value) {
    // set the value type to OBJECT
    value->type = OBJECT;
    // allocate members on object
    value->object.capacity = 4;
    value->object.members = malloc(4 * sizeof(member));
    // consume the left brace
    consume(parser, LEFT_BRACE, "expected left brace");
    // parse the members list
    parse_members(parser, &value->object);
    // consume the right brace
    consume(parser, RIGHT_BRACE, "expected right brace");
}
```

This code is not very complicated. We allocate some memory for object members on value, pass it into the `parse_members` function, and set the `value->object` field to `object`.

```c
// parse array
void parse_array(parser *parser, value *value) {
    value->type = ARRAY;
    value->array.capacity = 4;
    value->array.elements = malloc(4 * sizeof(*value));
    consume(parser, LEFT_BRACKET, "expected left bracket");
    parse_elements(parser, &value->array);
    consume(parser, RIGHT_BRACKET, "expected right bracket");
}
```

This function is very similar to `parse_objects`.

Let's get to the interesting part. Here is how to parse the value:

```c
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
```

This function is fairly simple. We peek at the current token. If it is the left brace, there is an object ahead and we call the `parse_object` function. If it is left bracket, we call the `parse_array` function. If it is a number, we use the `atof` function to convert the token's lexeme to a number and put it inside the `number` field. If it is a string, we just copy the token's lexeme into the `string` field of the value. Otherwise, we just set up the appropriate type value. It is sufficient to identify the value and no further information is needed.

At this point, we are done with parsing, so let's free the parser data:

```c
// free parser data
void free_parser(parser *parser) {
    // free lexeme string of each token
    for (int i = 0; i < parser->current + 1; i++) {
        free(parser->tokens[i].lexeme);
    }
    // free the tokens array
    free(parser->tokens);
}
```

Note that we add 1 to the `current` field since we also need to free the final `EOF` token.

Ironically enough, we are going to print the JSON value as a JSON string. This will allow you to convert the value back to the JSON string. We are going to use yet another dynamic array to represent strings:

```c
// add this after the value structure
typedef struct {
    int capacity;
    int size;
    char *string;
} string;
```

Here are functions related to our string:

```c
// concatenate to the string
void string_cat(string *string, char *msg) {
    int size = strlen(msg);
    // reallocate the string
    if (string->size + size >= string->capacity) {
        string->capacity += size;
        string->capacity *= 2;
        string->string = realloc(
            string->string, string->capacity
        );
    }
    // terminate the string with the nul byte
    string->string[string->size] = 0;
    // increase the string size
    string->size += size;
    // concatenate msg to the string
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
```

Now let's handle the conversion to string from the value and compound types:

```c
// function declaration
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
            string_cat(string, "    ");
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
        string_cat(string, "    ");
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
```

There is a lot of code related to pretty printing. You can read it if you'd like. I'm not going to comment on this code much.

After we are done working with our value, don't forget to free all data related to it:

```c
// free data from value
void free_value(value *value) {
    switch(value->type) {
    case ARRAY:
        // free all values from the array
        for (int i = 0; i < value->array.size; i++) {
            free_value(&value->array.elements[i]);
        }
        // free the elements array
        free(value->array.elements);
        break;
    case OBJECT:
        // free key and value from each member of the object
        for (int i = 0; i < value->object.size; i++) {
            free(value->object.members[i].string);
            free_value(value->object.members[i].value);
            free(value->object.members[i].value);
        }
        // free the member's array
        free(value->object.members);
        break;
    case VALUE_STRING:
        // free the string value
        free(value->string);
        break;
    }
}
```

Let's change the `parse_json` function:

```c
// change parse_json signature to this
void parse_json(const char *buffer, value *value)
// remove this from parse_json
print_tokens(&scanner);
// add this to parse_json after scan_tokens
parser parser = {.tokens = scanner.tokens};
parse_value(&parser, value);
free_parser(&parser);
```

Finally, let's update the `main` function:

```c
// change parse_json line in main to this
value value = {0};
parse_json(source, &value);
// add this to main after parse_json
string string = {
 .capacity = 64,
 .string = malloc(64)
};
value_string(&value, &string, 0);
string_print(&string);
free_value(&value);
free_string(&string);
free(source);
```

Try to compile the program. Start it with [this file](../sample.json). Check out the [source code](../json2.c).

Here is a simple exercise for you: try to implement two functions, `array_get` and `object_get`, that given a value and identifier (integer index for array and string key for object) would return either a pointer to appropriate value or NULL if nothing is found.