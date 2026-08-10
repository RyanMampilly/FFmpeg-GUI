#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* TODO
 * Send errors to SDL log instead of (or maybe alongside) stdout
 * Change return values to SDL defined enums to follow style
 * Fix the issue with onClick() - see function declaration
 * Add function pointer to onClick parameters, maybe. So that you can tell it to do a specified function on click?
 */

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
char windowTitle[] = "My SDL Project";

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Event event;
static float mouseX;
static float mouseY;

void renderButton(SDL_FRect *b) {
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, b);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderRect(renderer, b);
}
void onClick(SDL_FRect *b) { // This triggers really fast, leading to tens to hundreds of calls even in a reasonable click duration. Figure out how to fix
    if (SDL_GetMouseState(&mouseX, &mouseY) == 1 && 
        (b->x < mouseX ) && (mouseX < b->x + b->w) &&
        (b->y < mouseY ) && (mouseY < b->y + b->h)) {
        printf("Button Clicked");
    }
}


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

    SDL_FRect testButton = { 10, 10, 150, 50 };

    // Program loop

    while(1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            break;
        }
        onClick(&testButton);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        renderButton(&testButton);
        SDL_RenderPresent(renderer);
    }

    // Quitting

    SDL_Quit();

    printf("Quitting...\n");

    return 0;
}
