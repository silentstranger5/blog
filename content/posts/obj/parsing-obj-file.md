+++
date = '2025-01-26T20:32:35+03:00'
draft = false
title = 'Parsing Obj File'
+++

### OBJ files

In this series of posts, I am going to write a parser for `.obj` files. Those files are used to store data about 3d objects like vertex positions, normal vectors, texture coordinates, and material properties. This parser will be written iteratively, one step at a time. Why bother writing such a parser? You will learn:

- How to read text files
- How to process strings
- How to manage allocation and storage of data

And maybe something on top of this. This parser is not very complicated but requires a bit of code and a careful approach. An interleaved array that such a parser will produce can be used in projects that expect such data, like `OpenGL`.

Before we go further, I would like to illustrate the approach that I will be using. Here is a problem. Let's say that we have a file called `file.obj` that contains data like this:

```txt
v 0.5773502691896258 0.5773502691896258 0.5773502691896258
v -0.5773502691896258 -0.5773502691896258 0.5773502691896258
v 0.5773502691896258 -0.5773502691896258 -0.5773502691896258
v -0.5773502691896258 0.5773502691896258 -0.5773502691896258
v 0.5773502691896258 3.5773502691896257 0.5773502691896258
v 0.5773502691896258 3.5773502691896257 -0.5773502691896258
v -0.5773502691896258 3.5773502691896257 -0.5773502691896258
v -0.5773502691896258 3.5773502691896257 0.5773502691896258
v 0.5773502691896258 2.4226497308103743 0.5773502691896258
v 0.5773502691896258 2.4226497308103743 -0.5773502691896258
v -0.5773502691896258 2.4226497308103743 0.5773502691896258
v -0.5773502691896258 2.4226497308103743 -0.5773502691896258
...
```

Let's say that `v` is a key that represents a vertex. Each vertex is guaranteed to have three coordinates. We need to read this file and return an array of vertices that it contains. How are we going to do that?

Since we don't know the number of vertices in advance, there are two approaches that we can use:

1. Use a dynamic array to store vertices. The array will be automatically resized to store more vertices.
2. Read the file two times: the first time to count several vertices and the second time to parse them.

The first approach is quite cumbersome for several reasons:

1. In the future we will need to store not only vertices (or more generally 3-component arrays of floats) but all sorts of data. C is statically typed and therefore requires you to write an implementation for each type. There are some libraries that automate this process (like [stc](https://github.com/stclib/STC)), but this will add a new dependency to our project.
2. Reallocation is a costly operation: it will copy all elements of each reallocation
3. Memory locality is probably suboptimal considering that we will need to call reallocation for each type of dynamic array some number of times
4. Code can be arguably more complex (especially if you are writing dynamic arrays on your own)

In my humble opinion, the second approach is not only more simple but presumably more effective as well.

### Simple parser

I am going to write a simple example step by step so that you can understand the core principle of the parser. Before we start, let's include all necessary headers:

```c
#include <stdio.h>      // standard input/output
#include <stdlib.h>     // useful functions
#include <string.h>     // string manipulation
```

For data, we need a simple structure called `vertices` that will consist of two fields: number of vertices, and vertex data:

```c
typedef struct {
    int vertices;
    float *vertices;
} vertices;
```

Our `main` function will accept one argument which is the filename of our file. The first piece is pretty simple. If the number of arguments is not correct, print the usage message and exit the program.

```c
if (args != 2) { // check program usage
    printf("usage: %s file.obj\n", argv[0]);
    exit(1);
}
```

Next, we allocate all variables on the stack. `s` is the character buffer that will store lines from the file. `vert` is an instance of our `vertices` structure. Notice that it is allocated on the stack and initialized to zero.

```c
char s[512]; // buffer to store line from the file
vertices vert = {0}; // vertices instance
```

Then, we open the file and check if it is opened correctly.

```c
FILE *f = fopen(argv[1], "r"); // open file
if (!f) { // check if file exists
    printf("failed to open %s\n", argv[1]);
    exit(1);
}
```

Now, we read the file line by line and check the first word on the line. If it is `v`, then it means that we found a new vertex. In this case, we increase the number of vertices:

```c
while (fgets(s, 512, f)) { // read file line by line
    char *key = strtok(s, " "); // get first word
    if (!strcmp(key, "v")) { // check if key == "v"
        vert.nvertices++; // increase number of vertices
    }
}
```

The function `fgets` is typically used to read an input from some file. It reads the input until either `newline` or `EOF` (end of file) is encountered. Also, notice the `strtok` function. You can check the reference [here](https://cplusplus.com/reference/cstring/strtok/). We will use this function extensively to split lines by spaces. What it is doing is splitting the line into `tokens` (pieces of a line) by `delimiters`. For example this line:

```
v 0.5773502691896258 0.5773502691896258 0.5773502691896258
```

Tokens by delimiter ' ' (space) are:

```
"v", "0.5773502691896258, "0.5773502691896258", "0.5773502691896258"
```

The function `strcmp` compares two strings and returns `0` if the strings are equal to each other. We use it to check whether the first token of each line is equal to `v`, and if it is, this means that the current line denotes a vertex. In this case, we increase the `nvertices` field by one.

At this point, we know how many vertices our file contains. This means that we can allocate enough space to store all vertices (given that each vertex contains exactly three coordinates):

```c
vert.vertices = malloc(3 * vert.vertices * sizeof(float));
```

Now I am going to do something weird:

```c
vert.nvertices = 0;
```

I can probably guess the question you ask at this moment: "Hey, we spend so much effort to count how many vertices we have, so why do you set this field to zero? What was the point of counting vertices in the first place?"

Well, you see, we are about to read the whole file for the second time. It means that we need to somehow keep track of which vertex line we are currently parsing. Since the amount of vertices does not change between the first and second time we read the file, it will be the same after we are done parsing the vertices. In the meantime, we will use the field `nvertices` to keep track of the current vertex we are parsing.

Now, we need to reset the state of our file so that we can read it again from the start. The function `rewind` is doing just that:

```c
rewind(f);
```

Finally, here is the code to parse the vertices from the file:

```c
while (fgets(s, 512, f)) {
    char *key = strtok(s, " ");
    if (!strcmp(key, "v")) {
        float *vptr = vert.vertices + 3 * vert.nvertices++;
        for (int i = 0; i < 3; i++) {
            char *vstr = strtok(NULL, " ");
            vptr[i] = atof(vstr);
        }
    }
}
```

I suppose that functions `fgets`, `strtok`, and `strcmp` are already familiar to you from the code snippet earlier. `vptr` is a pointer to the place where we will put vertex coordinates from the current line. I am using the pointer arithmetic, but this expression is equivalent to `float *ptr = &vert.vertices[3 * vert.nvertices++];`. Recall that `vert.nvertices` is a current vertex that we are parsing, and since each vertex contains three coordinates, the address that we need is `3 * vert.nvertices`. We also increment the current vertex number after we copy the address.

`vstr` is the next token on the current line delimited by space. Notice that we use the `NULL` pointer to tell `strtok` function that we intend to continue splitting the current line into tokens. It is a common mistake to put the current line (`s`) as the first argument since in this case, parsing will start again from the beginning and you will get the first token again.

Finally, we use the `atof` function to convert a string to a `float` number and write it into a vertex.

We are done parsing all vertices. Now let's print them neatly:

```c
printf("vertices:\n");
printf("%4d ", 0);
for (int i = 0; i < vert.nvertices; i++) {
    if (i > 0 && (i % 3) == 0) {
        printf("\n%4d ", i);
    }
    printf("[ ");
    for (int j = 0; j < 3; j++) {
        printf("%8.4f ", vert.vertices[3 * i + j]);
    }
    printf("] ");
}
putchar('\n');
```

There is some stuff going on, but I'm not going to focus on formatting our output. All of this additional code is just here for our data to look pretty. The outer loop iterates through vertices, and the inner loop iterates through individual coordinates for each vertex. Such coordinate address is calculated as `3 * i + j` (again, since each vertex contains three coordinates).

Now all that is left is to close the file and return from our function:

```c
fclose(f);
return 0;
```

You can check the entire source code [here](../example.c). Compile the source code and try to launch it with the file I showed you earlier (`file.obj`):

```bash
./parse file.obj
vertices:
   0 [   0.5774   0.5774   0.5774 ] [  -0.5774  -0.5774   0.5774 ] [   0.5774  -0.5774  -0.5774 ]
   3 [  -0.5774   0.5774  -0.5774 ] [   0.5774   3.5774   0.5774 ] [   0.5774   3.5774  -0.5774 ]
   6 [  -0.5774   3.5774  -0.5774 ] [  -0.5774   3.5774   0.5774 ] [   0.5774   2.4226   0.5774 ]
   9 [   0.5774   2.4226  -0.5774 ] [  -0.5774   2.4226   0.5774 ] [  -0.5774   2.4226  -0.5774 ]
```