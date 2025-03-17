#include <stdio.h>
#include "graphics.h"

bool init();
void kill();
bool loop();

SDL_Window *window;
SDL_Renderer *renderer;

int main(int argc, char **argv) {
    if (!init()) return 1;
    while (loop()) SDL_Delay(100);
    kill();
    return 0;
}

bool loop(void) {
    SDL_Event e;

    color(renderer, 0, 0, 0, 0);
    clear(renderer);

    while (SDL_PollEvent(&e) != 0)
        switch (e.type) {
        case SDL_EVENT_QUIT:
            return false;
        }

    void draw(void);
    draw();
    update(renderer);
    return true;
}

bool init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("failed to initialize sdl: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Window", 256, 256, 0);
    if (!window) {
        printf("failed to create window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        printf("failed to create renderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void kill(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw(void) {
    color(renderer, 255, 255, 255, 255);
    polygon(renderer, LINE, 
        (float[]){25, 50, 100, 25, 125, 100, 50, 125}, 4);
}
