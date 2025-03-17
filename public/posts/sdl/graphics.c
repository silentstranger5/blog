#include "graphics.h"

void my_line(render_t *renderer, float x0, float y0, float x1, float y1) {
    if (x0 > x1) {
        swap(float, x0, y0);
        swap(float, x1, y1);
    }
    float k = (y1 - y0) / (x1 - x0);
    for (float x = x0; x < x1; x++) {
        float y = y0 + k * (x - x0);
        point(renderer, x, y);
    }
}

void my_rectangle(render_t *r, int m, float x0, float y0, float w, float h) {
    switch (m) {
    case LINE:
        line(r, x0, y0, x0 + w, y0);
        line(r, x0 + w, y0, x0 + w, y0 + h);
        line(r, x0 + w, y0 + h, x0, y0 + h);
        line(r, x0, y0 + h, x0, y0);
        break;
    case FILL:
        for (float y = y0; y < y0 + h; y++) {
            line(r, x0, y, x0 + w, y);
        }
        break;
    }
}

void ellipse(render_t *r, int m, float a, float b, float h, float k) {
    switch (m) {
    case LINE:
        for (float x = -a; x < a; x += 0.1f) {
            float y = b * (float) sqrt(1 - (x * x) / (a * a));
            point(r, x + h, -y + k);
            point(r, x + h,  y + k);
        }
        break;
    case FILL:
        for (float y = -b; y < b; y++) {
            float x = a * (float) sqrt(1 - (y * y) / (b * b));
            line(r, -x + h, y + k, x + h, y + k);
        }
        break;
    }
}

void circle(render_t *re, int m, float r, float h, float k) {
    ellipse(re, m, r, r, h, k);
}

int cmpy(const void *x, const void *y) {
    return (int) (((float *) x)[1] - ((float *) y)[1]);
}

void triangle(render_t *r, int m, float *v) {
    switch(m) {
        case LINE:
            for (int i = 0; i < 3; i++) {
                line(r, v[2 * i + 0], v[2 * i + 1],
                    v[2 * ((i+1)%3) + 0], v[2 * ((i+1)%3) + 1]);
            }
            break;
        case FILL:
            qsort(v, 3, 2 * sizeof(float), cmpy);
            for (float y = v[1]; y < v[5]; y++) {
                float x0 = (y < v[3]) ?
                    v[0] + (v[2] - v[0]) * (y - v[1]) / (v[3] - v[1]) :
                    v[2] + (v[4] - v[2]) * (y - v[3]) / (v[5] - v[3]) ;
                float x1 = v[0] + (v[4] - v[0]) * (y - v[1]) / (v[5] - v[1]);
                line(r, x0, y, x1, y);
            }
            break;
    }
}

void polygon(render_t *r, int m, float *v, int n) {
    for (int i = 0; i < n - 2; i++) {
        triangle(r, m, (float []) {
            v[0], v[1],
            v[2 * (i+1) + 0], v[2 * (i+1) + 1],
            v[2 * (i+2) + 0], v[2 * (i+2) + 1],
        });
    }
}