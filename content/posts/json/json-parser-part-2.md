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

Before we go further I'd like to tell you a curious detail about data structures. If you visit [JSON page](https://json.org), you easily find syntactical specifications of JSON format. The curious detail that I've noticed is the fact that some data type definitions are circular. For example:

```
1. Array is a collection of values
2. Value can have one of several types, including arrays
```

Therefore, `array` would refer to the `value`, which in turn would refer to `array`. It is a chicken and an egg problem of sorts. So when I was trying to define the data types I was confused: how can I define a data structure definition that is circular? It turns out that in C you can declare not only variables but data types as well. Take a look at this:

```c
struct Person {
    char *name;
    int age;
    struct Person *friends;
}
```

This recursive definition should be familiar to you, especially if you implemented linked lists before. Note that the last field is a *pointer* to a struct. You can't define it as a value, since then the compiler won't be able to evaluate the data structure:

```
struct Person which consists of three fields, one of which is a
struct Person which consists of three fields, one of which is a
...
```

You can see that this loop just won't terminate, since the compiler has to evaluate all of the value fields inside of the structure. Compare that to

```
struct Person which consists of three fields, one of which is a
pointer to struct Person.
```

Note that pointers are only evaluated when the value is dereferenced, therefore compiler does not have to evaluate the pointer field when the structure is defined.

This is nice and all, but how does that relate to our problem? Let's say that we have this:

```c
struct Person {
    char *name;
    int age;
    struct People *friends;
};

struct People {
    int capacity;
    int size;
    struct Person *people;
};
```

Notice the circular definition: `Person -> People -> Person -> ...`. This is quite a similar situation. This also works since we use pointers that can be evaluated at dereferencing. What if we use `typedef`s?

```c
typedef struct Person {
    char *name;
    int age;
    struct People *friends;
} Person;

typedef struct People {
    int capacity;
    int size;
    struct Person *people;
} People;
```

That still works. But what if we use unnamed structures?

```c
typedef struct {
    char *name;
    int age
    People *friends;
} Person;

typedef struct {
    int capacity;
    int size;
    Person *people;
} People;
```

I use anonymous structure definitions quite often since they save me from excessive typing. However, since `People` is not yet defined at the point where the `friends` field of `Person` is defined, the compiler will give you an error. How can we fix it? Well, we can just *declare* type definitions to a yet-undefined structure like this:

```c
typedef struct Person Person;
typedef struct People People;

struct Person {
    char *name;
    int age;
    People *friends;
};

struct People {
    int capacity;
    int size;
    Person *people;
};
```

Surprisingly, it works! Yes, you can just *declare* a symbol `Person` to be defined as a `struct Person`, whatever it is, even if it's not defined yet.

This took a bit more time to explain than I expected. It is not very important to what we'll be doing today, but I still thought that it might be an interesting corner of syntax.

With syntax out of the way, let's think about how we'll define our data. JSON specification from the website above will come in handy for this:

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

// check the current token without advancing further
token parser_peek(parser *parser) {
    return parser->tokens[parser->current];
}

// check if we are done
int parser_is_at_end(parser *parser) {
    return parser_peek(parser).type == TOKEN_EOF;
}

// check if the type of the current token is equal to the type
int check(parser *parser, int type) {
    if (parser_is_at_end(parser)) {
        return 0;
    }
    return parser_peek(parser).type == type;
}

// return the previous token
token previous(parser *parser) {
    return parser->tokens[parser->current - 1];
}

// advance the parser to the next token
token parser_advance(parser *parser) {
    if (!parser_is_at_end(parser)) {
        parser->current++;
    }
    return previous(parser);
}

// consume the current token if matches the type and report an error otherwise 
token consume(parser *parser, int type, char *msg) {
    if (check(parser, type)) {
        return parser_advance(parser);
    }
    parser_error(parser_peek(parser), msg);
}

// report the parser error
void parser_error(token token, char *msg) {
    fprintf(stderr, "[line %d] at '%s': %s\n", token.line, token.lexeme, msg);
    exit(1);
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
            array->elements, array->capacity * sizeof(value)
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
        object->members, object->capacity * sizeof(member)
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
    // preallocate array storage
    array->capacity = 4;
    array->elements = malloc(4 * sizeof(value));
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
2. Preallocate some space for the array
3. Parse the value and append it to the array
4. While the current token is a comma:
    - Advance the parser
    - Parse the new value
    - Append it to the array

This pattern is typical for parsing the comma-delimited values. Now let's handle the parsing of a member (an entry of an object):

```c
// parse member
void parse_member(parser *parser, member *member) {
    // create the new member and allocate memory for the value field
    member->value = malloc(sizeof(value));
    // consume the member key string and report an error if it's not present
    token string = consume(parser, TOKEN_STRING, "expected string");
    // copy the string token lexeme
    member->string = strdup(string.lexeme);
    // consume the colon token
    consume(parser, COLON, "expected colon");
    // parse the value
    value value = {0};
    parse_value(parser, &value);
    // copy the value into the value field
    memcpy(member->value, &value, sizeof(value));
}
```

Again, this is very similar to this syntactical clause of JSON:

```
member
    string ':' element
```

Finally, let's wrap it inside a new function that will parse a list of comma-delimited members:

```c
void parse_members(parser *parser, object *object) {
    if (parser_peek(parser).type == RIGHT_BRACE) {
        return;
    }
    object->capacity = 4;
    object->members = malloc(4 * sizeof(member));
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
    // init object on the stack to zero
    object object = {0};
    // consume the left brace
    consume(parser, LEFT_BRACE, "expected left brace");
    // parse the member's list
    parse_members(parser, &object);
    // consume the right brace
    consume(parser, RIGHT_BRACE, "expected right brace");
    // set object field of value to objects
    value->object = object;
}
```

This code is not very complicated. We initialize the `object` on the stack, pass it into the `parse_members` function, and set the `value->object` field to `object`.

```c
// parse array
void parse_array(parser *parser, value *value) {
    value->type = ARRAY;
    array array = {0};
    consume(parser, LEFT_BRACKET, "expected left bracket");
    parse_elements(parser, &array);
    consume(parser, RIGHT_BRACKET, "expected right bracket");
    value->array = array;
}
```

This function is very similar to `parse_objects`.

Let's get to the interesting part. Here is how to parse the value:

```c
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
void parser_free(parser *parser) {
    // free lexeme string of each token
    for (int i = 0; i < parser->current; i++) {
        free(parser->tokens[i].lexeme);
    }
    // free the tokens array
    free(parser->tokens);
}
```

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
// append the message to the string
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

// convert an array to a string and append it to the string
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

// convert an object to a string and append it to the string
void object_string(object *object, string *string, int ind) {
    char keystr[64] = {0};
    string_cat(string, "{ ");
    if (object->size > 0) {
        string_cat(string, "\n");
    }
    for (int i = 0; i < object->size; i++) {
        member member = object->members[i];
        for (int i = 0; i < ind + 1; i++) {
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

// convert a value to a string and append it to the string
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
parser_free(&parser);
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