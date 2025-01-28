+++
date = '2025-01-28T20:02:37+03:00'
draft = false
title = 'Parsing Obj File Part 3'
+++

In the previous post, we created a core of the `.obj` parser. It can handle keywords now, and store the data from a file. Today, I am going to handle *faces*.

Faces represent a combination of vertices that comprise one facet of an object. Each face consists of *face vertices* which are a triplet of indices that denote a position of the vertex, texture coordinate, and normal, in that order. 

Before we go further, let's clarify one detail. There is a distinction between a *vertex* and *face vertex*. Consider a vertex from a cube. If you look at it, you might notice that any vertex of a cube is a part of three cube faces. When I simply say "vertex", I mean a vertex of an object, like a cube. When I say "face vertex", however, I mean a more specific case of the vertex that belongs to a face of an object. Sometimes I might omit "face" from "face vertex". In such case, it should hopefully be clear from the context that I mean a "face vertex".

Let's consider a simple example:

```
f 1//1 2//1 3//1
```

This means that the object has a face that consists of three vertices. Let's take a look at the first face vertex. Indices that comprise the face vertex are separated by the `/` (slash) character. This means that the first face vertex consists of:

- first vertex
- first normal

Note that there is nothing between the two slashes. This means that this vertex (just like any other in this face) does not have a texture coordinate.

Let's say that the first vertex looks like this:

```
v 3.577350 3.577350 0.577350
```

And first normal looks like this:

```
vn 0.5257 -0.0000 0.8507
```

This means that in the form of an interleaved `buffer` the first face vertex shall look like this (assuming that all absent data points are zero-initialized):

```
[ 3.577350 3.577350 0.577350 ] [ 0.000000 0.000000 ] [ 0.5257 -0.0000 0.8507 ]
```

We are also going to cover `meshes`. Right now, our parser is just combining all data, back-to-back. This is also going to be the case for faces and buffers. We need a way to distinguish between different meshes. To do that, we are going to introduce *mesh offsets*. It will be an array that will consist of face vertex offsets for each mesh. The last element is going to be the total number of face vertices. This way, for any mesh with index `i < meshes`, the size of the mesh (i.e. amount of face vertices per each mesh) can be calculated like so:

```
meshsize[i] = meshoffsets[i + 1] - meshoffsets[i]
```

Conversely, the `meshoffsets` array will be filled like this:

```
[ meshoffset[0], meshoffset[1], ..., meshoffset[nmeshes - 1], nfacevert ]
```

Without further ado, let's introduce additional fields into the `objects` structure:

```c
// add this to objectx
int nfaces;             // number of faces
int nfaceverts;         // number of face vertices
int *faces;             // faces
int *meshoffsets;       // mesh offsets
float *buffer;          // interleaved buffer
```

Before we can allocate `faces` or `buffer` we need to calculate the amount of face vertices in our file. There is a subtle nuance, however. We are going to *triangulate* all faces, i. e. we are going to turn all n-gons with n > 3 into triangles. We will cover the triangulation algorithm later, but for now, it is sufficient to note that amount of vertices in the triangulated face is calculated like this:

```
nfaceverts_new = (nfaceverts_old - 2) * 3;
```

This formula works for any `nfaceverts_old >= 3`. Note that each face is guaranteed to have at least three vertices, and a face with two or fewer vertices is illegal. With this knowledge, we can now update the `count_key` function:

```c
// add this inside switch statement in `count_key` function:
case F:
    ctx->nfaces++;                  // new face detected
    int nverts = 0;                 // number of face vertices on this line
    while (strtok(NULL, " ")) {     // while there are face vertices on the line
        nverts++;                   // increase `nverts`
    }
    nverts = (nverts - 2) * 3;      // apply triangulation formula
    ctx->nfaceverts += nverts;      // add number of face vertices on current line
                                    // to total number of vertices
    break;
```

Now that we know how many faces and face vertices we have, we can allocate memory to hold data from the file:

```c
// add this to objctx_malloc
ctx->faces = calloc(3 * ctx->nfaceverts, sizeof(int));
ctx->buffer = calloc(8 * ctx->nfaceverts, sizeof(float));
ctx->meshoffsets = malloc((ctx->nmeshes + 1) * sizeof(int));
```

Note that we allocate 8 numbers per face vertex (3 coordinates per vertex + 2 coordinates for texture + 3 coordinates for normal). We also initialize `faces` and `buffer` to zero. We initialize `meshoffsets` with 1 extra number to hold a total number of face vertices according to our specifications.

Before we proceed with parsing, we need to reset all corresponding counters to zero:

```c
// add this to objctx_reset
ctx->nfaces = 0;
ctx->nfaceverts = 0;
ctx->nmeshes = 0;
```

Now we are going to create a function that will parse a face vertex:

```c
// create new function before parse_objline
void parse_face(char *s, int *fptr) {
    int i = 0;
    char *t = NULL;
    while (t = strchr(s, '/')) {
        fptr[i++] = atoi(s);
        s = t + 1;
    }
    fptr[i++] = atoi(s);
}
```

Our function accepts two parameters. `s` is a string that contains a face vertex (like `1/2/3`). `fptr` is a pointer to the place where we need to put a face vertex. The algorithm is pretty simple:

```
while a current string contains character '/' (slash):
    - parse a float number from the string
    - put it inside fptr[i]
    - increment number i
    - set s to be t + 1 (set s to start one character after t)
- parse a float number from the string
- put it inside fptr[i]
```

Try to reason through this algorithm step by step to parse a simple face vertex (like `1/2/3`). Carefully check how it works. Also, consider the edge cases:

- What if a middle number is absent? (as in `1//2`)
- What if the last number is absent? (as in `1/2`)
- What if only the first number is present? (as in `1`)

When you are trying to answer those questions, consider this:

- If `atof` encounters a non-digit character, it returns `0`
- All three numbers from `fptr` are initialized to zero

Now that we prepared all that is needed, the interesting part begins. We are going to cover new cases for the `parse_objline` function. First, let's cover how to parse `o` key:

```c
// add this to switch statement inside parse_objline
case O:
    ctx->meshoffsets[ctx->nmeshes++] = ctx->nfaceverts;
    break;
```

This code is pretty simple: write a current number of face vertices in a current position of `meshoffsets` and increment the number of meshes.

Now let's consider how to parse faces.

```c
// add this to switch statement inside parse_objline
case F:
    // add code below here
```

The first thing that we need is to create all variables. `nvert` will store several face vertices on a current line. `fverts` will be a string that holds the current face vertex string. `fptr` will point to the *start* of where we are going to put face vertices from the current line:

```c
int nvert = 0;
char *fverts = NULL;
int *fptr = ctx->faces + 3 * ctx->nfaceverts;
```

We could use `fptr` as the first argument for the `parse_face` function, but this means that we have to increment it after each iteration. We can't do that, since we will need a pointer to the *start* of where we put our face vertices in a moment. Therefore, a new variable called `fvptr` will be an argument for the `parse_face` function:

```c
int *fvptr = fptr;
```

Here is a loop where face parsing happens:

```c
while(fverts = strtok(NULL, " ")) {
    parse_face(fverts, fvptr);
    fvptr += 3;
    nvert++;
}
```

Notice that we increment `fvptr` by three (since each face vertex contains three numbers), and `nvert` by one.

If we have only three vertices per face, this means that there is not much more work to be done and we can wrap it up. But do you remember when I mentioned face *triangulation* earlier? Now is the time to implement it.

```c
if (nvert > 3) {    // do not perform additional work if face is already a triangle
    // insert code below here
}
```

Let's consider a simple example where we have face vertices that only contain vertices and nothing else. You may neglect other data for now.

Let's say that we have a face like this:

```
f 1 2 3 4
```

We need to triangulate it to turn a quad into two triangles. How are we going to do it? Fortunately, all n-gons for `n > 3` have a useful property that is specified by `.obj` files. They are specified in the form of *triangle fans*, which means that there is a simple and deterministic way to triangulate any n-gon. Here is how to do it:

```
for i = 0; i < nvertices - 2; i++ do:
    add a new vertex face[0]
    add a new vertex face[i + 1]
    add a new vertex face[i + 2]
end
```

Here, `face` is an array of face vertices on a current line. Three specified vertices form a new triangle. If you look at this algorithm carefully, you can see where the triangulation formula mentioned above comes from.

After execution of this algorithm, a triangulated face should look like this:

```
f 1 2 3
f 1 3 4
```

It's a nice and simple algorithm, but we can't use it just yet. Before we do that, we need to copy all vertices from the current line into a new place, so that we can put triangulated face vertices back into the `faces` field. Let's do this:

```c
int *fcptr = malloc(3 * nvert * sizeof(int));
memcpy(fcptr, fptr, 3 * nvert * sizeof(int));
```

Here, `fcptr` is a place that will store a copy of the face vertices from the current line. The `memcpy` function performs a memory copy to the destination from a source.

Finally, let's perform triangulation and "unwrap" data from `fcptr` into `fptr`:

```c
for (int i = 0; i < nvert - 2; i++) {
    memcpy(fptr + 9 * i + 0, fcptr + 3 * 0, 3 * sizeof(int));
    memcpy(fptr + 9 * i + 3, fcptr + 3 * (i + 1), 3 * sizeof(int));
    memcpy(fptr + 9 * i + 6, fcptr + 3 * (i + 2), 3 * sizeof(int));
}
```

Now that we are done with triangulation, let's free data from `fcptr` since we won't need it and apply triangulation formula to get correct number of face vertices:

```c
free(fcptr);
nvert = (nvert - 2) * 3;
```

Now that we are done with triangulation, you can close an `if` statement. Don't forget to update the number of faces and face vertices. Close the case branch.

```c
ctx->nfaceverts += nvert;
ctx->nfaces++;
break;
```

Here is what the entire case looks like:

```c
case F:
    int nvert = 0;
    char *fverts = NULL;
    int *fptr = ctx->faces + 3 * ctx->nfaceverts;
    int *fvptr = fptr;
    while(fverts = strtok(NULL, " ")) {
        parse_face(fverts, fvptr);
        fvptr += 3;
        nvert++;
    }
    if (nvert > 3) {
        int *fcptr = malloc(3 * nvert * sizeof(int));
        memcpy(fcptr, fptr, 3 * nvert * sizeof(int));
        for (int i = 0; i < nvert - 2; i++) {
            memcpy(fptr + 9 * i + 0, fcptr + 3 * 0, 3 * sizeof(int));
            memcpy(fptr + 9 * i + 3, fcptr + 3 * (i + 1), 3 * sizeof(int));
            memcpy(fptr + 9 * i + 6, fcptr + 3 * (i + 2), 3 * sizeof(int));
        }
        free(fcptr);
        nvert = (nvert - 2) * 3;
    }
    ctx->nfaceverts += nvert;
    ctx->nfaces++;
    break;
```

Now that we have triangulated face vertex indices, it's time to build an interleaved buffer that will contain all data from faces. Create a new function.

```c
// create new function after parse_objline
void build_buffer(objctx *ctx) {
    for (int i = 0; i < ctx->nfaceverts; i++) {
        float *bptr = ctx->buffer + 8 * i;      // pointer to the place in the buffer where we'll put the data
        int *fptr = ctx->faces + 3 * i;         // pointer to the current face vertex
        int vert = fptr[0];                     // vertex is the first number
        int texc = fptr[1];                     // texture coordinate
        int norm = fptr[2];                     // normal vector
        if (vert) {                             // if vertex exists
            memcpy(bptr, ctx->vertices + 3 * (vert - 1), 3 * sizeof(float));        // copy the vertex
        }
        if (texc) {                             // if texture coordinate exists
            memcpy(bptr + 3, ctx->texcoords + 2 * (texc - 1), 2 * sizeof(float));   // copy the tex. coordinate
        }
        if (norm) {                             // if normal exists
            memcpy(bptr + 5, ctx->normals + 3 * (norm - 1), 3 * sizeof(float));     // copy the normal
        }
    }
}
```

Now that we have all the new data in the context, we can print it out neatly:

```c
// add this to objctx_print
printf("mesh offsets:\n");
for (int i = 0; i < ctx->nmeshes + 1; i++) {
    if (i > 0 && (i % 16) == 0) {
        putchar('\n');
    }
    printf("%4d ", ctx->meshoffsets[i]);
}
putchar('\n');
printf("faces:\n");
printf("%4d ", 0);
for (int i = 0; i < ctx->nfaceverts; i++) {
    if (i > 0 && (i % 3) == 0) {
        printf("\n%4d ", i);
    }
    printf("[ ");
    for (int j = 0; j < 3; j++) {
        printf("%4d ", ctx->faces[i * 3 + j]);
    }
    printf("] ");
}
putchar('\n');
printf("buffer:\n");
for (int i = 0; i < ctx->nfaceverts; i++) {
    printf("%4d ", i);
    int nd[] = {3, 2, 3};
    int offs[] = {0, 3, 5};
    for (int j = 0; j < 3; j++) {
        printf("[ ");
        for (int k = 0; k < nd[j]; k++) {
            printf("%8.4f ", ctx->buffer[8 * i + offs[j] + k]);
        }
        printf("] ");
    }
    putchar('\n');
}
```

Don't forget to free all new fields from the context:

```c
// add this to objctx_free
free(ctx->faces);
free(ctx->buffer);
free(ctx->meshoffsets);
```

All that you need to do in `main` is to insert two lines:

```c
// add this to main after end of the second while loop
ctx.meshoffsets[ctx.nmeshes] = ctx.nfaceverts;
build_buffer(&ctx);
```

That's it. Try to compile the program and check if it works with the file from the previous post. Now you should see more data being printed, like mesh offsets, face indices, and buffers.

You can also check out the [source code](../obj2.c).