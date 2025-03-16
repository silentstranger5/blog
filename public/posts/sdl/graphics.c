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

void my_rectangle(render_t *renderer, int m, float x0, float y0, float w, float h) {
    switch (m) {
    case LINE:
        line(renderer, x0, y0, x0 + w, y0);
        line(renderer, x0 + w, y0, x0 + w, y0 + h);
        line(renderer, x0 + w, y0 + h, x0, y0 + h);
        line(renderer, x0, y0 + h, x0, y0);
        break;
    case FILL:
        for (float y = y0; y < y0 + h; y++) {
            line(renderer, x0, y, x0 + w, y);
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

int cmp(const void *x, const void *y) {
	return (int) (*(float *) x - *(float *) y);
}

void polygon(render_t *r, int m, float *v, int n) {
	if (m == LINE) {
		for (int i = 0; i < n; i++) {
			line(r, v[2 * i + 0], v[2 * i + 1], 
				v[2 * ((i+1) % n) + 0], v[2 * ((i+1) % n) + 1]);
		}
		return;
	}
	typedef struct { float k, x0, y0, ymin, ymax; } edge;
	edge *e = malloc(n * sizeof(edge));
	float ymin = v[0], ymax = v[1];
	for (int i = 0; i < n; i++) {
		float x0 = v[2 * i + 0];
		float y0 = v[2 * i + 1];
		float x1 = v[2 * ((i+1)%n) + 0];
		float y1 = v[2 * ((i+1)%n) + 1];
		float eymin = (y0 < y1) ? y0 : y1;
		float eymax = (y0 > y1) ? y0 : y1;
		if (x0 > x1) {
			swap(float, x0, x1);
			swap(float, y0, y1);
		}
		float k = (y1 - y0) / (x1 - x0);
		e[i] = (edge) { k, x0, y0, eymin, eymax };
        if (eymin < ymin) ymin = eymin;
        if (eymax > ymax) ymax = eymax;
	}
    float *xi = malloc(n * sizeof(float));
    for (float y = ymin; y < ymax; y++) {
        int xn = 0;
        for (int i = 0; i < n; i++) {
            if (e[i].k && y > e[i].ymin && y <= e[i].ymax) {
                xi[xn++] = e[i].x0 + (y - e[i].y0) / e[i].k;
            }
        }
        qsort(xi, xn, sizeof(float), cmp);
        for (int i = 0; i < xn; i += 2) {
            line(r, xi[i], y, xi[i + 1], y);
        }
    }
    free(xi);
    free(e);
}