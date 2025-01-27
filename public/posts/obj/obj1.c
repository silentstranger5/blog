#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *objkeys[] = {
    "f", "mtllib", "o", "usemtl", "v", "vn", "vt"
};

enum objkeysenum { F, MTLLIB, O, USEMTL, V, VN, VT };

typedef struct {
    int nmeshes;
    int nvertices;
    int nnormals;
    int ntexcoords;
    float *vertices;
    float *normals;
    float *texcoords;
} objctx;

int strcomp(const void *key, const void *elem) {
    const char *skey = *(const char **) key;
    const char *selem = *(const char **) elem;
    return strcmp(skey, selem);
}

int find_objkey(char *s) {
    char *key = strtok(s, " ");
    size_t nkeys = sizeof(objkeys) / sizeof(char *);
    char **kptr = (char **) bsearch(&key, objkeys, nkeys, sizeof(char *), strcomp);
    if (!kptr) {
        return -1;
    }
    return kptr - objkeys;
}

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

void objctx_malloc(objctx *ctx) {
    ctx->vertices = malloc(3 * ctx->nvertices * sizeof(float));
    ctx->normals = malloc(3 * ctx->nnormals * sizeof(float));
    ctx->texcoords = malloc(2 * ctx->ntexcoords * sizeof(float));
}

void objctx_reset(objctx *ctx) {
    ctx->nvertices = 0;
    ctx->nnormals = 0;
    ctx->ntexcoords = 0;
}

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

void objctx_free(objctx *ctx) {
    free(ctx->vertices);
    free(ctx->normals);
    free(ctx->texcoords);
}

void parse_obj(const char *filename) {
    FILE *f = fopen(filename, "r");         // open the file
    if (!f) {                               // check if file exists
        fprintf(stderr, "failed to open file %s\n", filename);
        exit(1);
    }
    char s[512];                            // char buffer to store lines from the file
    objctx ctx = {0};                       // context instance initialized on stack to zero
    while (fgets(s, 512, f)) {              // read file line by line
        count_key(&ctx, s);                 // count keys in our file
    }
    objctx_malloc(&ctx);                    // allocate memory to hold values from file
    objctx_reset(&ctx);                     // reset all counters to zero
    rewind(f);                              // rewind file to the start
    while (fgets(s, 512, f)) {              // read file line by line
        parse_objline(&ctx, s);             // update context values
    }
    objctx_print(&ctx);                     // print values from context
    objctx_free(&ctx);                      // free context data
    fclose(f);                              // close the file
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s filename.obj\n", argv[0]);
        return 1;
    }
    parse_obj(argv[1]);
    return 0;
}