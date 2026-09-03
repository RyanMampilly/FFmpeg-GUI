#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* TODO
 * Send errors to SDL log instead of (or maybe alongside) stdout
 * Change return values to SDL defined enums to follow style
 * Maybe put some of these global-ish variables into a header file?
 * Event processing in a switch block
 * Use SDL_ttf for text rendering in the future?
 * Figure out how to make the text box text buffers work
 */

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
char windowTitle[] = "My SDL Project";

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;

struct {
    float mouseDownX;
    float mouseDownY;
    float mouseUpX;
    float mouseUpY;

    float currentX;
    float currentY;

    int mouseDownButton;
} mouseState;



typedef struct { // 56 bytes
    SDL_FRect box; // Bounding box // 16 bytes
    char string[32]; // Text displayed on button // 32 bytes 
    void (*func)(); // Generic function so that different buttons can do different things when pressed // 8 bytes
    // char padding[8]; // In case I want to pad these objects to 64 bytes
} Button;

typedef struct { // 144 bytes
    SDL_FRect box; // Bounding box // 16 bytes
    char string[128]; // Text that can fit in the text box // 128 bytes
                      // For now, this will be the maximum amount of text a textbox CAN hold
} TextBox;



// Takes in a SDL_FRect and two coordinates and checks if its in bounds of the rect
bool inBounds(SDL_FRect *b, float x, float y) {
    return (b->x < x) && (x < b->x + b->w) && (b->y < y) && (y < b->y + b->h);
}

// Takes in a Button and handles rendering
void renderButton(Button *b) {
    if (mouseState.mouseDownButton == 1 && inBounds(&b->box, mouseState.mouseDownX, mouseState.mouseDownY)) SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    else SDL_SetRenderDrawColor(renderer, 235, 235, 235, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &b->box);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderRect(renderer, &b->box);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugText(renderer, (b->box.x + b->box.w / 2 - strlen(b->string) * 4), (b->box.y + b->box.h / 2 - 4), b->string);
}

// Runs the object's function on click
void onClick(Button *b) { 
    if (mouseState.mouseDownButton == 1 && inBounds(&b->box, mouseState.currentX, mouseState.currentY) && inBounds(&b->box, mouseState.mouseDownX, mouseState.mouseDownY)) {
        b -> func();
    }
}

// Two testing functions
void testFunction() {
    printf("Button Clicked\n");
}
void testFunction1() { 
    printf("Other Button Clicked\n");
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

    Button buttons[] = { //array of all the buttons in the program, and an enum so they can be accessed by name. hacky? maybe, i dont know
        {{ 10, 10, 150, 50 }, "Button number 1", testFunction },
        {{ 10, 70, 150, 50 }, "Button number 2", testFunction1 }
    }; 
    enum buttonNames {
        test,
        otherTest
    };

    // Program loop

    while(1) {
        SDL_PollEvent(&event);

        // Prevent unnecessary rendering (may need to remove later) When there are no events, just repoll
        if (event.type == SDL_EVENT_POLL_SENTINEL) continue; 

        printf("0x%x\n", event.type); //DEBUG
        
        //Maybe replace this if/else thing with a switch block? For SDL_EVENT_QUIT, should I use a goto (*gulp*)?
        if (event.type == SDL_EVENT_QUIT) {
            break;
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            SDL_GetMouseState(&mouseState.mouseDownX, &mouseState.mouseDownY);
            mouseState.mouseDownButton = event.button.button;
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            SDL_GetMouseState(&mouseState.mouseUpX, &mouseState.mouseUpY);
            for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
                onClick(&buttons[i]); //debug kinda
            }
            mouseState.mouseDownButton = 0;
        } else if (event.type = SDL_EVENT_MOUSE_MOTION) {
            SDL_GetMouseState(&mouseState.currentX, &mouseState.currentY);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
            renderButton(&buttons[i]);
        }
        SDL_RenderPresent(renderer);
    }

    // Quitting

    SDL_Quit();

    printf("Quitting...\n");

    return 0;
}
