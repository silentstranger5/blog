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
    int nfaces;
    int nfaceverts;
    int *faces;
    int *meshoffsets;
    float *vertices;
    float *normals;
    float *texcoords;
    float *buffer;
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
    case F:
        int nverts = 0;
        ctx->nfaces++;
        while (strtok(NULL, " ")) {
            nverts++;
        }
        if (nverts > 3) {
            nverts = (nverts - 2) * 3;
        }
        ctx->nfaceverts += nverts;
        break;
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
    ctx->faces = malloc(3 * ctx->nfaceverts * sizeof(int));
    ctx->buffer = calloc(8 * ctx->nfaceverts, sizeof(float));
    ctx->meshoffsets = malloc((ctx->nmeshes + 1) * sizeof(int));
}

void objctx_reset(objctx *ctx) {
    ctx->nvertices = 0;
    ctx->nnormals = 0;
    ctx->ntexcoords = 0;
    ctx->nfaces = 0;
    ctx->nfaceverts = 0;
    ctx->nmeshes = 0;
}

void parse_face(char *s, int *fptr) {
    int i = 0;
    char *t = NULL;
    while (t = strchr(s, '/')) {
        fptr[i++] = atoi(s);
        s = t + 1;
    }
    fptr[i++] = atoi(s);
}

void parse_objline(objctx *ctx, char *s) {
    int key = find_objkey(s);       // check the type of a string key
    switch (key) {
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
    case O:
        ctx->meshoffsets[ctx->nmeshes++] = ctx->nfaceverts;
        break;
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

void build_buffer(objctx *ctx) {
    for (int i = 0; i < ctx->nfaceverts; i++) {
        float *bptr = ctx->buffer + 8 * i;
        int *fptr = ctx->faces + 3 * i;
        int vert = fptr[0];
        int texc = fptr[1];
        int norm = fptr[2];
        if (vert) {
            memcpy(bptr, ctx->vertices + 3 * (vert - 1), 3 * sizeof(float));
        }
        if (texc) {
            memcpy(bptr + 3, ctx->texcoords + 2 * (texc - 1), 2 * sizeof(float));
        }
        if (norm) {
            memcpy(bptr + 5, ctx->normals + 3 * (norm - 1), 3 * sizeof(float));
        }
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
}

void objctx_free(objctx *ctx) {
    free(ctx->vertices);
    free(ctx->normals);
    free(ctx->texcoords);
    free(ctx->faces);
    free(ctx->buffer);
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
    ctx.meshoffsets[ctx.nmeshes] = ctx.nfaceverts;
    build_buffer(&ctx);
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