+++
date = '2025-01-29T19:37:05+03:00'
draft = false
title = 'Parsing Obj File Part 4'
+++

In the previous post, we added face parsing to our parser. In this (hopefully) last part of the series, I'm going to add the last feature: parsing materials.

Materials define various optical characteristics of the material that an object has. Materials are usually stored in a separate file. We need to define the keys of material files and corresponding values since the parsing algorithm will be pretty similar to that for `.obj` files. This list is a bit longer, but hopefully, it's still sortable by hand. Let's define keys and place values:

```c
// add this after obj keys enum
const char *mtlkeys[] = {
    "Ka", "Kd", "Ke", "Ks", 
    "Ni", "Ns", "d", "illum", 
    "map_Ka", "map_Kd",
    "map_Ks", "map_Ns", 
    "map_bump", "map_d", 
    "newmtl"
};

enum mtlkeysenum {
    KA, KD, KE, KS,
    NI, NS, D, ILLUM,
    MAP_KA, MAP_KD,
    MAP_KS, MAP_NS,
    MAP_BUMP, MAP_D,
    NEWMTL
};
```

Now we need to define a new data structure that will store data about materials:

```c
// add this before objctx structure
typedef struct {
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float emissive[3];
    float shininess;
    float refraction;
    float transparency;
    int   illum;
    char  *name;
    char  *map_ambient;
    char  *map_diffuse;
    char  *map_specular;
    char  *map_highlight;
    char  *map_alpha;
    char  *map_bump;
} mtl;
```

We also need to somehow store additional context related to materials. Let's define a new structure for that:

```c
// add this after `mat` structure

typedef struct {
    int  nmaterials;
    mat  *materials;
    char *filename;
} mtlctx;
```

We are going to store material data inside the object context. Let's embed materials context along with some other fields inside the `objctx` structure:

```c
// add this to objctx
int *mtlindices;
char  *filename;
mtlctx materials;
```

We will take care of the `mtlindices` field later. Everything else should be fairly simple to understand.

Before we can process a material file, we need to obtain a path to the file. Our program is based on the assumption that the `.obj` and `.mtl` files reside in the same directory. We now have two cases to consider:

1. The `.obj` file path is in the same directory and specified without slashes (like `file.obj`)
2. The `.obj` file path is in some other directory relative to the current directory (like `path/to/file.obj`)

In the first case, we can just use the provided `.mtl` file path without any modifications whatsoever. In the second case, however, we need to obtain a directory of our `.obj` file and then concatenate it with the `.mtl` filename.

Let's create a new function before the `count_key` function:

```c
// add this before `count_key` function
void mtl_filename(objctx *ctx, char *filename) {
    // code below goes here
}
```

Here, `ctx` is a context with the data, and `filename` is the material filename as specified in the `.obj` file. We assume that at this point `filename` field of `ctx` already stores the filename of the `.obj` file. Let's consider the first case mentioned above:

```c
if (!strchr(ctx->filename, '/')) {
    ctx->materials.filename = strdup(filename);
    return;
}
```

The function `strchr` returns a pointer to a character in a string. Here, the `!strchr(ctx->filename, '/')` expression is true if the `ctx->filename` field does not contain any slashes. The function `strdup` returns a copy string of its first argument.

Now we will implement the second case. First, let's create a copy of the `ctx->filename` field to obtain a relative path to the `.obj` file. After that, let's replace all backslashes with forward slashes:

```c
char *directory = strdup(ctx->filename);
for (char *c = directory; *c; c++) {
    if (*c == '\\') {
        *c = '/';
    }
}
```

Here we use `c` to represent a pointer in the `directory` string. We iterate through the string until we encounter a null byte (which is succinctly written as `*c`). Inside the loop, we check whether the current symbol is a backslash, and if so, we change it to a forward slash.

After that, let's cut the directory from the `.obj` file path:

```c
*(strrchr(directory, '/') + 1) = 0;
```

Again, `strrchr` returns a pointer to *the last occurrence* of a character in a string. Here, we add one to this pointer, dereference it, and set it to zero. In other words, we set a character after the last occurrence of a forward slash in the `directory` to a null byte, thus terminating the string. For example, consider the string `path/to/file.obj`. Here, the last occurrence of slash is after `to`. One character after that is `f`. C uses null bytes to terminate strings. Thus, if we set `f` to a null byte, we obtain the string `path/to/`. This is exactly what we need.

Finally, let's concatenate `directory` with `filename` and free `directory` since we won't need it:

```c
char **path = &(ctx->materials.filename);
*path = calloc(strlen(directory) + strlen(filename) + 1, sizeof(char));
strcat(*path, directory);
strcat(*path, filename);
free(directory);
```

Notice that `path` is a pointer to a string, which is a pointer to a pointer to `char`. Also, note that we allocate one character more than the combined length of the `directory` and `filename` strings since we need to store a terminating null byte.

The complete function looks like this:

```c
// add this before `count_key` function
void mtl_filename(objctx *ctx, char *filename) {
    if (!strchr(ctx->filename, '/')) {
        ctx->materials.filename = strdup(filename);
        return;
    }
    char *directory = strdup(ctx->filename);
    for (char *c = directory; *c; c++) {
        if (*c == '\\') {
            *c = '/';
        }
    }
    *(strrchr(directory, '/') + 1) = 0;
    char **path = &(ctx->materials.filename);
    *path = calloc(strlen(directory) + strlen(filename) + 1, sizeof(char));
    strcat(*path, directory);
    strcat(*path, filename);
    free(directory);
}
```

Note that we use the `ctx->materials.filename` field to store the `.mtl` filename. We will need it in the future.

Before we allocate enough space for materials, we need to count how many materials we have. Let's create a new function for that:

```c
void count_materials(objctx *ctx, char *filename) {
    mtl_filename(ctx, filename);
    FILE *f = fopen(ctx->materials.filename, "r");
    if (!f) {
        fprintf(stderr, "failed to open %s\n", ctx->materials.filename);
        exit(1);
    }
    char s[512];
    while (fgets(s, 512, f)) {
        char *key = strtok(s, " ");
        if (!strcmp(key, "newmtl")) {
            ctx->materials.nmaterials++;
        }
    }
    fclose(f);
}
```

This function should be pretty simple:

- open a file
- check if it exists
- read it line by line
- whenever a `newmtl` key is encountered:
    - increase the number of materials
- close file

Let's plug this function inside the `count_key` function:

```c
case MTLLIB:
    s = strchr(s, '\0') + 1;
    *strchr(s, '\n') = 0;
    count_materials(ctx, s);
    break;
```

Notice that we set `s` to be one character after a null byte (which is now in place of the former space character, which is after the key). This way, we obtain a string that contains a `.mtl` filename. We also set a newline character to zero. After that, we call `count_materials` function.

Now that we are done with counting, we can allocate the memory:

```c
// add this to `objctx_malloc` function
ctx->mtlindices = malloc(ctx->nmeshes * sizeof(int));
ctx->materials.materials = calloc(ctx->materials.nmaterials, sizeof(mtl));
```

Note that all materials are initialized to zero.

Let's reset a new counter in the `objctx_reset` function:

```c
// add this to `objctx_reset` function
ctx->materials.nmaterials = 0;
```

Now we need to write a new function called `find_mtlkey` to find the position of the `.mtl` file key:

```c
// add this before `parse_face` function
int find_mtlkey(char *s) {
    size_t nkeys = sizeof(mtlkeys) / sizeof(char *);
    char *key = strtok(s, " ");
    char **kptr = (char **) bsearch(&key, mtlkeys, nkeys, sizeof(char *), strcomp);
    if (!kptr) {
        return -1;
    }
    return kptr - mtlkeys;
}
```

This function is very similar to `find_objkey`, so I don't think that it needs much explanation.

Now for the most interesting part. We need to write a function that, given a line from the `.mtl` line, can parse it and update the context. Here is the function:

```c
void parse_mtlline(objctx *ctx, char *s) {
    char *mapstr = NULL;
    int key = find_mtlkey(s);
    mtl *mtl = ctx->materials.materials + ctx->materials.nmaterials - 1;
    switch (key) {
    case KA:
        for (int i = 0; i < 3; i++) {
            char *astr = strtok(NULL, " ");
            mtl->ambient[i] = atof(astr);
        }
        break;
    case KD:
        for (int i = 0; i < 3; i++) {
            char *dstr = strtok(NULL, " ");
            mtl->diffuse[i] = atof(dstr);
        }
        break;
    case KE:
        for (int i = 0; i < 3; i++) {
            char *estr = strtok(NULL, " ");
            mtl->emissive[i] = atof(estr);
        }
        break;
    case KS:
        for (int i = 0; i < 3; i++) {
            char *sstr = strtok(NULL, " ");
            mtl->specular[i] = atof(sstr);
        }
        break;
    case NI:
        char *rstr = strtok(NULL, " ");
        mtl->refraction = atof(rstr);
        break;
    case NS:
        char *sstr = strtok(NULL, " ");
        mtl->shininess = atof(sstr);
        break;
    case D:
        char *tstr = strtok(NULL, " ");
        mtl->transparency = atof(tstr);
        break;
    case ILLUM:
        char *istr = strtok(NULL, " ");
        mtl->illum = atoi(istr);
        break;
    case MAP_KA:
        char *mapstr = strtok(NULL, " ");
        mtl->map_ambient = strdup(mapstr);
        break;
    case MAP_KD:
        mapstr = strtok(NULL, " ");
        mtl->map_diffuse = strdup(mapstr);
        break;
    case MAP_KS:
        mapstr = strtok(NULL, " ");
        mtl->map_specular = strdup(mapstr);
        break;
    case MAP_NS:
        mapstr = strtok(NULL, " ");
        mtl->map_highlight = strdup(mapstr);
        break;
    case MAP_BUMP:
        mapstr = strtok(NULL, " ");
        mtl->map_bump = strdup(mapstr);
    case MAP_D:
        mapstr = strtok(NULL, " ");
        mtl->map_alpha = strdup(mapstr);
        break;
    case NEWMTL:
        mtl++;
        ctx->materials.nmaterials++;
        char *name = strtok(NULL, " ");
        *strchr(name, '\n') = 0;
        mtl->name = strdup(name);
        break;
    default:
        break;
    }
}
```

There are three types of cases:

- If the key denotes a vector, parse it one number after another three times
- If the key denotes a number, parse it once
- If the key denotes a map string, copy it into the context

Most functions here should be familiar to you, and the code is pretty simple. Note that most cases use `mtl`, which in turn is a pointer to the current material whose index is denoted by `ctx->materials.nmaterials - 1`. We subtract one since materials have the following form:

```
newmtl Material
Ka ...  # ambient for Material
Kd ...  # diffuse for Material
...
```

Since we increase the number of materials each time we encounter the `newmtl` key, this means that the current material should be one position behind.

Let's introduce a new function, `parse_materials`, that, given a context, can parse the `.mtl` file and update all necessary data:

```c
void parse_materials(objctx *ctx) {
    FILE *f = fopen(ctx->materials.filename, "r");
    char s[512];
    while (fgets(s, 512, f)) {
        parse_mtlline(ctx, s);
    }
    fclose(f);
}
```

Note that we assume that `ctx->materials.filename` is already set to `.mtl` filename (by the `mtl_filename` function) at this point.

Finally, let's introduce *material indices*. At this point, all materials are parsed, and information about them is available. We now need a way to associate each mesh with a material. We are going to do that with material indices. The `matindices` field of the `objctx` structure shall be an array of indices of materials associated with each mesh. Let's say that the `objctx.materials.materials` field is an array of n materials:

```
[mat0, mat1, ..., mat(n-1)]
```

where `mat(i)` is a material with index `i`.

Then, `matindices` is an array of n integers:

```
[mesh0_matidx, mesh1_matidx, ..., mesh(i-1)_matidx ]
```

Where `mesh(i)_matidx` is an index from an array `materials.materials` of a material associated with mesh from `meshoffsets` array with an index `i`. This means, for example, that `mesh` from `meshoffsets` with an index `0` has a material from `materials.materials` with an index `mesh0_matidx`. In other words, the material of the first mesh is `materials.materials[mesh0_matidx]` or `materials.materials[matindices[0]]` which is equivalent.

To construct the `matindices` array we are going to use the `USEMTL` case of the `parse_objline` function in the following manner:

```c
case USEMTL:
    char *mtlname = strtok(NULL, " ");                              // obtain a material name
    *strchr(mtlname, '\n') = 0;                                     // terminate newline with nul byte
    for (int i = 0; i < ctx->materials.nmaterials; i++) {           // iterate through all materials
        if (!strcmp(mtlname, ctx->materials.materials[i].name)) {   // if materials.materials[i].name == mtlname
            ctx->matindices[ctx->nmeshes - 1] = i;                  // set material index for the current mesh to be i
        }
    }
    break;
```

Now that we have data about materials, let's print all that data. Let's create a new function called `mat_print` that will print each material from the context of the material:

```c
// create new function before `objctx_print`
void mtl_print(objctx *ctx) {
    printf("materials:\n");
    for (int i = 0; i < ctx->materials.nmaterials; i++) {
        mtl *mtl = ctx->materials.materials + i;
        printf("name:\t\t%s\n", mtl->name);
        printf("ambient:\t[ ");
        for (int j = 0; j < 3; j++) {
            printf("%8.4f ", mtl->ambient[j]);
        }
        printf("]\n");
        printf("diffuse:\t[ ");
        for (int j = 0; j < 3; j++) {
            printf("%8.4f ", mtl->diffuse[j]);
        }
        printf("]\n");
        printf("specular:\t[ ");
        for (int j = 0; j < 3; j++) {
            printf("%8.4f ", mtl->specular[j]);
        }
        printf("]\n");
        printf("emissive:\t[ ");
        for (int j = 0; j < 3; j++) {
            printf("%8.4f ", mtl->emissive[j]);
        }
        printf("]\n");
        printf("shininess:\t%10.4f\n", mtl->shininess);
        printf("refraction:\t%10.4f\n", mtl->refraction);
        printf("transparency:\t%10.4f\n", mtl->transparency);
        printf("illum:\t\t%5d\n", mtl->illum);
        if (mtl->map_ambient) {
            printf("ambient map: %s\n", mtl->map_ambient);
        }
        if (mtl->map_diffuse) {
            printf("diffuse map: %s\n", mtl->map_diffuse);
        }
        if (mtl->map_specular) {
            printf("specular map: %s\n", mtl->map_specular);
        }
        if (mtl->map_highlight) {
            printf("highlight map: %s\n", mtl->map_highlight);
        }
        if (mtl->map_alpha) {
            printf("alpha map: %s\n", mtl->map_alpha);
        }
        if (mtl->map_bump) {
            printf("bump map: %s\n", mtl->map_bump);
        }
    }
}
```

Now let's update the `objctx_print` function to handle materials:

```c
// add this to `objctx_print` function
printf("material indices:\n");
for (int i = 0; i < ctx->nmeshes; i++) {
    if (i > 0 && (i % 16 == 0)) {
        putchar('\n');
    }
    printf("%4d ", ctx->mtlindices[i]);
}
putchar('\n');
mtl_print(ctx);
```

At this point, we are done with our data. Let's free all data from materials with a new function called `matctx_free`:

```c
// create this function before `objctx_free` function
void mtlctx_free(mtlctx *ctx) {
    free(ctx->filename);
    for (int i = 0; i < ctx->nmaterials; i++) {
        mtl *mtl = ctx->materials + i;
        if (mtl->map_ambient) {
            free(mtl->map_ambient);
        }
        if (mtl->map_diffuse) {
            free(mtl->map_diffuse);
        }
        if (mtl->map_specular) {
            free(mtl->map_specular);
        }
        if (mtl->map_highlight) {
            free(mtl->map_highlight);
        }
        if (mtl->map_alpha) {
            free(mtl->map_alpha);
        }
        if (mtl->map_bump) {
            free(mtl->map_bump);
        }
        free(mtl->name);
    }
    free(ctx->materials);
}
```

Note that we only free map strings if they are not `NULL` (by default all of them are initialized to `NULL`).

Update the `objctx_free` function to handle materials as well:

```c
// add this to `objctx_free` function
free(ctx->mtlindices);
mtlctx_free(&ctx->materials);
```

Finally, update the `parse_obj` function:

```c
// add this to `parse_obj` function after `ctx` initialization
ctx.filename = (char *) filename;

// add this to `parse_obj` function after `objctx_reset`
parse_materials(&ctx);
```

Here is a [material file](../solids.mtl) that you can use to test the program. You can check out [source code](../obj3.c) as always. Try to compile the program. It should now print materials data.