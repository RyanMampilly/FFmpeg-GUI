#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* TODO
 * Send errors to SDL log instead of (or maybe alongside) stdout
 * Change return values to SDL defined enums to follow style
 */

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
char windowTitle[] = "My SDL Project";

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Event event;

int main(int argc, char **argv) {

    // Initialization

    printf("Starting SDL...\n");
    
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        printf(SDL_GetError());
        return -1;
    } else {
        printf("VIDEO and EVENTS initialized...\n");
    }

    if (!SDL_CreateWindowAndRenderer(windowTitle, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        printf(SDL_GetError());
        return -1;
    } else {
        printf("Window created...\n");
    }

    // Program loop

    while(1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            break;
        }

        SDL_SetRenderDrawColor(renderer, 255, 127, 0, SDL_ALPHA_TRANSPARENT);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

    }

    // Quitting

    SDL_Quit();

    printf("Quitting...\n");

    return 0;
}
