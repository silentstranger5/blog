+++
date = '2025-01-27T13:05:06+03:00'
draft = false
title = 'Parsing Obj File Part 2'
+++

In this part of the series on parsing `.obj` files, we are going to implement a part of the parser that will process keywords from the file. We will not cover faces and materials parsing yet, and we will cover it in later parts.

In our previous post, I showed you a simple parser that can parse vertices. Now we will extend and modify it to handle other keys like `o`, `vn`, and `vt`. To do this we need to change our approach to searching keys.

In a previous post, I used a simple function called `strcmp` to perform a string comparison. However, since we are going to search for one of multiple possible keys, there is a possible optimization here: a *binary search*.

C provides almost no implementation to any algorithm whatsoever. Two tasks, however, are so prevalent in program algorithms, that they made an exception for them and put their implementation in the standard library. Those tasks are *searching* and *sorting*. To be more precise, the standard library provides an implementation for *binary search* and *quick sort*. Since C is statically typed, standard library implementation is abstracting away from types using a mechanism called *void pointers*. Essentially, no assumptions are made about the data that is being processed, and all computations are handled using addresses to unknown objects. The only information being provided to the function other than the pointer to data is the size of an element and the number of those elements. It is the user's responsibility to perform casting pointers to corresponding types and perform a comparison between elements.

As always, create a directory and create a source file to follow along. Before we proceed, let's include headers that we need:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
```

To perform binary searching, we need a sorted array of keys to search from. `.obj` files define a fairly small number of keys so that you can sort them in your head. Let's define them in a global array:

```c
const char *objkeys[] = {
    "f", "mtllib", "o", "usemtl", "v", "vn", "vt"
};
```

After we are done searching, we are going to use a *position* of our key to check what kind of key we have. Let's define an enumeration with key indices:

```c
enum objkeysenum { F, MTLLIB, O, USEMTL, V, VN, VT };
```

For now, we won't touch faces and materials since they will require some additional code. That leaves us with meshes, vertices, normals, and texture coordinates. Let's define a structure called `objctx` (not "Object X", but ".obj context") that will contain all fields that we need:

```c
typedef struct {
    int nmeshes;
    int nvertices;
    int nnormals;
    int ntexcoords;
    float *vertices;
    float *normals;
    float *texcoords;
} objctx;
```

The first thing that we're going to do is to implement a binary search for our keys. The standard library function for that is called [`bsearch`](https://cplusplus.com/reference/cstdlib/bsearch/). The first three arguments are fairly straightforward, but the last one is a bit tricky. Here is a quote from reference:

```
Pointer to a function that compares two elements.
This function is called repeatedly by bsearch to compare the key against individual elements in the base. It shall follow the following prototype:

int compar (const void* pkey, const void* pelem);

Taking two pointers as arguments: the first is always key, and the second points to an element of the array (both type-casted to const void*). The function shall return (in a stable and transitive manner):

return value    meaning
 < 0            The element pointed to by pkey goes before the element pointed to by pelem
 0              The element pointed to by pkey is equivalent to the element pointed to by pelem
 > 0            The element pointed to by pkey goes after the element pointed to by pelem
```

Notice that both `pkey` and `pelem` are *pointers* to objects. This means that we need to do three things:

- Cast void pointer to pointer to an object of appropriate type
- Dereference this pointer to obtain a value that can be used for comparison
- Perform a comparison between two values

Since we are comparing strings (which are pointers to characters), then a pointer to a string is a pointer to a pointer to a character. We can use a standard library `strcmp` function mentioned in the previous post to perform a comparison between strings. Here is what our string comparison function looks like:

```c
int strcomp(const void *key, const void *elem) {
    const char *skey = *(const char **) key;
    const char *selem = *(const char **) elem;
    return strcmp(skey, selem);
}
```

Notice an `o` after `c`, don't confuse it with `strcmp`. Also, note how we obtain an element from the pointer:

```c
(const char **) key;        // cast to pointer to string
*(const char **) key;       // dereference a pointer to string to get a string
const char *skey = *(const char **) key;    // assign string to `skey` variable
```

Now that we have a comparison function, we can implement a search algorithm itself. Let's say that we are given a string like so:

```
v 0.5773502691896258 0.5773502691896258 0.5773502691896258
```

We need to find what kind of key such string starts with. To obtain a key from this string, we can use the `strtok` function that I introduced in the previous post:

```c
char *key = strtok(s, " ");
```

Now we need to know the number of elements in the `objkeys` array. You can type it by hand, but there is a better method. In contrast to pointers that only represent an address and always have a fixed size, arrays represent an entire region of memory and their size is equal to the amount of space that their elements occupy. This enables us to calculate the number of elements in an array like so:

```c
size_t nkeys = sizeof(objkeys) / sizeof(char *);
```

The function `sizeof` returns the number of bytes that a given object occupies. Therefore, to calculate the number of elements in an array, we can divide the size of an array by the size of one element of an array.

We now have all the information required to perform a binary search:

```c
char **kptr = (char **) bsearch(&key, objkeys, nkeys, sizeof(char *), strcomp);
```

Notice that `bsearch` returns a *pointer* to the element that we are looking for. This means that this function will return a pointer to a string, which is a pointer to a pointer to `char`. We can now check if `kptr` is a valid address. If it's not, then `bsearch` could not find the element that we are looking for. In other words, `objkeys` does not contain the `key` element. In this case, we can return `-1`:

```c
if (!kptr) {
    return -1;
}
```

Finally, we can subtract the pointer that we got from the `objkeys` address to get an index of the `key` and return it:

```c
return kptr - objkeys;
```

Here is what a complete function looks like:

```c
int find_objkey(char *s) {
    char *key = strtok(s, " ");
    size_t nkeys = sizeof(objkeys) / sizeof(char *);
    char **kptr = (char **) bsearch(&key, objkeys, nkeys, sizeof(char *), strcomp);
    if (!kptr) {
        return -1;
    }
    return kptr - objkeys;
}
```

Now let's say that we are given a pointer to `objctx` that will store our context information along with a string from the file. We need to implement a function that will update information in the `objctx` instance depending on a key from the string. More precisely, we are going to count how many meshes, vertices, normals, and texture coordinates we have in our `.obj` file:

```c
void count_key(objctx *ctx, char *s) {
    int key = find_objkey(s);   // check the type of a string key
    switch (key) {
    case O:                 // new mesh found
        ctx->nmeshes++;     // increase the number of meshes
        break;
    case V:                 // new vertex found
        ctx->nvertices++;   // increase the number of vertices
        break;
    case VN:                // new normal found
        ctx->nnormals++;    // increase the number of normals
        break;
    case VT:                // new texture coordinate found
        ctx->ntexcoords++;  // increase number of tex. coordinates
        break;
    default:                // ignore unknown keys (like comments or empty lines)
        break;
    }
}
```

After we are done parsing our file line by line and counting all types of keys, we are ready to allocate memory for those objects:

```c
void objctx_malloc(objctx *ctx) {
    ctx->vertices = malloc(3 * ctx->nvertices * sizeof(float));
    ctx->normals = malloc(3 * ctx->nnormals * sizeof(float));
    ctx->texcoords = malloc(2 * ctx->ntexcoords * sizeof(float));
}
```

Notice that vertices and normals contain three coordinates each (since they denote a vector in 3d space) and texture coordinates contain two coordinates (since they denote the position of an image which is a plane).

Remember that we need to reset all counters to zero to use them to track a current position in an array:

```c
void objctx_reset(objctx *ctx) {
    ctx->nvertices = 0;
    ctx->nnormals = 0;
    ctx->ntexcoords = 0;
}
```

Now comes the interesting part. We need to implement a parser that will scan a string from a file and write values from this string to an appropriate array of our context depending on the key:

```c
void parse_objline(objctx *ctx, char *s) {
    int key = find_objkey(s);       // check the type of a string key
    switch (key) {
    case V:
        float *vptr = ctx->vertices + 3 * ctx->nvertices++;
        for (int i = 0; i < 3; i++) {
            char *verts = strtok(NULL, " ");
            float vert = atof(verts);
            vptr[i] = vert;
        }
        break;
    case VN:
        float *nptr = ctx->normals + 3 * ctx->nnormals++;
        for (int i = 0; i < 3; i++) {
            char *norms = strtok(NULL, " ");
            float norm = atof(norms);
            nptr[i] = norm;
        }
        break;
    case VT:
        float *tptr = ctx->texcoords + 2 * ctx->ntexcoords++;
        for (int i = 0; i < 2; i++) {
            char *texs = strtok(NULL, " ");
            float tex = atof(texs);
            tptr[i] = tex;
        }
        break;
    default:
        break;
    }
}
```

Each case is very similar to the others:

- Get a position of an array where we will put values from the current string
- For a certain number of times (depending on the number of coordinates in a key type):
    - Scan the next token from the string
    - Convert it to a `float` number
    - Write it to the context array

Now that we have all of the values from the context, we can finally print them neatly:

```c
void objctx_print(objctx *ctx) {
    printf("nmeshes: %d\n", ctx->nmeshes);
    printf("nvertices: %d\n", ctx->nvertices);
    printf("nnormals: %d\n", ctx->nnormals);
    printf("ntexcoords: %d\n", ctx->ntexcoords);
    printf("vertices:\n");
    printf("%4d ", 0);
    for (int i = 0; i < ctx->nvertices; i++) {
        if (i > 0 && (i % 3) == 0) {
            printf("\n%4d ", i);
        }
        printf("[ ");
        for (int j = 0; j < 3; j++) {
            printf("%8.4f ", ctx->vertices[i * 3 + j]);
        }
        printf("] ");
    }
    putchar('\n');
    printf("normals:\n");
    printf("%4d ", 0);
    for (int i = 0; i < ctx->nnormals; i++) {
        if (i > 0 && (i % 3) == 0) {
            printf("\n%4d ", i);
        }
        printf("[ ");
        for (int j = 0; j < 3; j++) {
            printf("%8.4f ", ctx->normals[i * 3 + j]);
        }
        printf("] ");
    }
    putchar('\n');
    printf("texcoords:\n");
    printf("%4d ", 0);
    for (int i = 0; i < ctx->ntexcoords; i++) {
        if (i > 0 && (i % 3) == 0) {
            printf("\n%4d ", i);
        }
        printf("[ ");
        for (int j = 0; j < 2; j++) {
            printf("%8.4f ", ctx->texcoords[i * 2 + j]);
        }
        printf("] ");
    }
    putchar('\n');
}
```

Now that we are done with our context, don't forget to free all data:

```c
void objctx_free(objctx *ctx) {
    free(ctx->vertices);
    free(ctx->normals);
    free(ctx->texcoords);
}
```

Now let's outline the whole program:

```c
void parse_obj(const char *filename) {
    FILE *f = fopen(filename, "r"); // open the file
    if (!f) {                       // check if file exists
        fprintf(stderr, "failed to open file %s\n", filename);
        exit(1);
    }
    char s[512];                    // char buffer to store lines from the file
    objctx ctx = {0};               // context instance initialized on stack to zero
    while (fgets(s, 512, f)) {      // read file line by line
        count_key(&ctx, s);         // count keys in our file
    }
    objctx_malloc(&ctx);            // allocate memory to hold values from file
    objctx_reset(&ctx);             // reset all counters to zero
    rewind(f);                      // rewind file to the start
    while (fgets(s, 512, f)) {      // read file line by line
        parse_objline(&ctx, s);     // update context values
    }
    objctx_print(&ctx);             // print values from context
    objctx_free(&ctx);              // free context data
    fclose(f);                      // close the file
}
```

The `main` function is very simple:

```c
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s filename.obj\n", argv[0]);
        return 1;
    }
    parse_obj(argv[1]);
    return 0;
}
```

Check out the [source code](../obj1.c). Try to compile the program and run it with [this file](../model.obj):

```bash
./parse model.obj
nmeshes: 5
nvertices: 50
nnormals: 80
ntexcoords: 0
vertices:
   0 [   0.5774   0.5774   0.5774 ] [  -0.5774  -0.5774   0.5774 ] [   0.5774  -0.5774  -0.5774 ]
   3 [  -0.5774   0.5774  -0.5774 ] [   0.5774   3.5774   0.5774 ] [   0.5774   3.5774  -0.5774 ]
   6 [  -0.5774   3.5774  -0.5774 ] [  -0.5774   3.5774   0.5774 ] [   0.5774   2.4226   0.5774 ]
   9 [   0.5774   2.4226  -0.5774 ] [  -0.5774   2.4226   0.5774 ] [  -0.5774   2.4226  -0.5774 ]
  12 [   4.0000   0.0000   0.0000 ] [   3.0000   1.0000   0.0000 ] [   3.0000   0.0000   1.0000 ]
  15 [   3.0000  -1.0000   0.0000 ] [   3.0000   0.0000  -1.0000 ] [   2.0000   0.0000   0.0000 ]
  18 [   3.5774   3.5774   0.5774 ] [   3.0000   3.3568   0.9342 ] [   3.0000   2.6432   0.9342 ]
  21 [   3.5774   2.4226   0.5774 ] [   3.9342   3.0000   0.3568 ] [   3.9342   3.0000  -0.3568 ]
  24 [   3.5774   3.5774  -0.5774 ] [   3.3568   3.9342   0.0000 ] [   2.6432   3.9342   0.0000 ]
  27 [   2.4226   3.5774   0.5774 ] [   3.0000   3.3568  -0.9342 ] [   2.4226   3.5774  -0.5774 ]
  30 [   3.5774   2.4226  -0.5774 ] [   3.0000   2.6432  -0.9342 ] [   2.4226   2.4226   0.5774 ]
  33 [   2.6432   2.0658   0.0000 ] [   3.3568   2.0658   0.0000 ] [   2.0658   3.0000  -0.3568 ]
  36 [   2.0658   3.0000   0.3568 ] [   2.4226   2.4226  -0.5774 ] [   6.0000   2.0257   0.8507 ]
  39 [   6.0000   0.9743   0.8507 ] [   6.8507   1.5000   0.5257 ] [   6.5257   2.3507   0.0000 ]
  42 [   5.4743   2.3507   0.0000 ] [   5.1493   1.5000   0.5257 ] [   6.0000   2.0257  -0.8507 ]
  45 [   6.0000   0.9743  -0.8507 ] [   5.1493   1.5000  -0.5257 ] [   6.8507   1.5000  -0.5257 ]
  48 [   6.5257   0.6493   0.0000 ] [   5.4743   0.6493   0.0000 ]
normals:
   0 [   0.5774  -0.5774   0.5774 ] [  -0.5774   0.5774   0.5774 ] [   0.5774   0.5774  -0.5774 ]
   3 [  -0.5774  -0.5774  -0.5774 ] [   0.0000   1.0000   0.0000 ] [   0.0000   1.0000   0.0000 ]
   6 [   1.0000   0.0000   0.0000 ] [   1.0000   0.0000   0.0000 ] [   0.0000   0.0000   1.0000 ]
   9 [   0.0000   0.0000   1.0000 ] [   0.0000   0.0000  -1.0000 ] [   0.0000   0.0000  -1.0000 ]
  12 [   0.0000  -1.0000   0.0000 ] [   0.0000  -1.0000   0.0000 ] [  -1.0000   0.0000   0.0000 ]
  15 [  -1.0000   0.0000   0.0000 ] [   0.5774   0.5774   0.5774 ] [   0.5774  -0.5774  -0.5774 ]
  18 [   0.5774  -0.5774   0.5774 ] [   0.5774   0.5774  -0.5774 ] [  -0.5774   0.5774  -0.5774 ]
  21 [  -0.5774  -0.5774   0.5774 ] [  -0.5774   0.5774   0.5774 ] [  -0.5774  -0.5774  -0.5774 ]
  24 [   0.5257   0.0000   0.8507 ] [   0.5257   0.0000   0.8507 ] [   0.5257   0.0000   0.8507 ]
  27 [   0.8507   0.5257   0.0000 ] [   0.8507   0.5257   0.0000 ] [   0.8507   0.5257   0.0000 ]
  30 [   0.0000   0.8507   0.5257 ] [   0.0000   0.8507   0.5257 ] [   0.0000   0.8507   0.5257 ]
  33 [   0.0000   0.8507  -0.5257 ] [   0.0000   0.8507  -0.5257 ] [   0.0000   0.8507  -0.5257 ]
  36 [   0.5257   0.0000  -0.8507 ] [   0.5257   0.0000  -0.8507 ] [   0.5257   0.0000  -0.8507 ]
  39 [   0.0000  -0.8507   0.5257 ] [   0.0000  -0.8507   0.5257 ] [   0.0000  -0.8507   0.5257 ]
  42 [   0.8507  -0.5257   0.0000 ] [   0.8507  -0.5257   0.0000 ] [   0.8507  -0.5257   0.0000 ]
  45 [  -0.8507   0.5257   0.0000 ] [  -0.8507   0.5257   0.0000 ] [  -0.8507   0.5257   0.0000 ]
  48 [  -0.5257   0.0000   0.8507 ] [  -0.5257   0.0000   0.8507 ] [  -0.5257   0.0000   0.8507 ]
  51 [   0.0000  -0.8507  -0.5257 ] [   0.0000  -0.8507  -0.5257 ] [   0.0000  -0.8507  -0.5257 ]
  54 [  -0.5257   0.0000  -0.8507 ] [  -0.5257   0.0000  -0.8507 ] [  -0.5257   0.0000  -0.8507 ]
  57 [  -0.8507  -0.5257   0.0000 ] [  -0.8507  -0.5257   0.0000 ] [  -0.8507  -0.5257   0.0000 ]
  60 [   0.3568   0.0000   0.9342 ] [   0.0000   0.9342   0.3568 ] [  -0.5774   0.5774   0.5774 ]
  63 [   0.5774   0.5774   0.5774 ] [  -0.3568   0.0000   0.9342 ] [  -0.3568   0.0000  -0.9342 ]
  66 [   0.5774   0.5774  -0.5774 ] [   0.0000   0.9342  -0.3568 ] [   0.3568   0.0000  -0.9342 ]
  69 [  -0.5774   0.5774  -0.5774 ] [   0.5774  -0.5774   0.5774 ] [   0.0000  -0.9342   0.3568 ]
  72 [  -0.5774  -0.5774   0.5774 ] [   0.0000  -0.9342  -0.3568 ] [  -0.5774  -0.5774  -0.5774 ]
  75 [   0.5774  -0.5774  -0.5774 ] [   0.9342   0.3568   0.0000 ] [   0.9342  -0.3568   0.0000 ]
  78 [  -0.9342   0.3568   0.0000 ] [  -0.9342  -0.3568   0.0000 ]
texcoords:
   0
```