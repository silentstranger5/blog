#ifndef GRAPHICS

#define GRAPHICS

#include <stdlib.h>
#include <SDL3/SDL.h>

#define clear     SDL_RenderClear
#define point     SDL_RenderPoint
#define line      SDL_RenderLine
#define rectangle SDL_RenderRect
#define update    SDL_RenderPresent
#define color     SDL_SetRenderDrawColor
#define render_t  SDL_Renderer

#define PI 3.14159265F
#define swap(t, x, y) { t z = x; x = y; y = z; }

enum {LINE, FILL};

void my_line(render_t *r, float x0, float y0, float x1, float y1);
void my_rectangle(render_t *r, int m, float x0, float y0, float w, float h);
void ellipse(render_t *r, int m, float a, float b, float h, float k);
void circle(render_t *re, int m, float r, float h, float k);
void triangle(render_t *r, int m, float *v);
void polygon(render_t *r, int m, float *v, int n);

#endif