#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int nvertices;
    float *vertices;
} vertices;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s file.obj\n", argv[0]);
        exit(1);
    }
    char s[512];
    vertices vert = {0};
    FILE *f = fopen(argv[1], "r");
    while (fgets(s, 512, f)) {
        char *key = strtok(s, " ");
        if (!strcmp(key, "v")) {
            vert.nvertices++;
        }
    }
    vert.vertices = malloc(3 * vert.nvertices * sizeof(float));
    vert.nvertices = 0;
    rewind(f);
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
    fclose(f);
    return 0;
}