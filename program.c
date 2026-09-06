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
const int FONT_WIDTH = 8;
const int FONT_HEIGHT = 8;
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
} Button;

typedef struct { // 144 bytes
    SDL_FRect box; // Bounding box // 16 bytes
    char string[128]; // Text that can fit in the text box // 128 bytes
                      // For now, this will be the maximum amount of text a textbox CAN hold
} TextBox;

enum objectType {
    BUTTON,
    TEXT_BOX
};

typedef struct {
    enum objectType type;
    union {
        Button button;
        TextBox textBox;
    };
} Object;




// Button constructor
Object makeButton( float x, float y, float w, float h, char string[], void (*func)()) {
    Object o; Button b;

    int maximum = 32;
    if (w / FONT_WIDTH < 32) maximum = w / FONT_WIDTH; 
    if (strlen(string) > maximum) { 
        strncpy(b.string, string, maximum - 4);
        b.string[maximum - 4] = '\0';
        strcat(b.string, "...");
        b.string[maximum - 1] = '\0';
    } else {
        strcpy(b.string, string);
    }
    b.box = (SDL_FRect) {x, y, w, h}; b.func = func;
    o.type = BUTTON; o.button = b;
    return o;
}

// Takes in a SDL_FRect and two coordinates and checks if its in bounds of the rect
bool inBounds(SDL_FRect *box, float x, float y) {
    return (box->x < x) && (x < box->x + box->w) && (box->y < y) && (y < box->y + box->h);
}

// Takes in a Button and handles rendering
void renderObject(Object *o) {
    switch (o->type) {
        case BUTTON:
            if (mouseState.mouseDownButton == 1 && inBounds(&o->button.box, mouseState.mouseDownX, mouseState.mouseDownY)) 
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
            else 
                SDL_SetRenderDrawColor(renderer, 235, 235, 235, SDL_ALPHA_OPAQUE);

            SDL_RenderFillRect(renderer, &o->button.box);
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderRect(renderer, &o->button.box);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderDebugText(
                    renderer, 
                    (o->button.box.x + o->button.box.w / 2 - strlen(o->button.string) * (FONT_WIDTH / 2)), 
                    (o->button.box.y + o->button.box.h / 2 - (FONT_HEIGHT / 2)),
                    o->button.string
            );
        case TEXT_BOX:
            break; //Write later
        default:
            printf("Invalid object sent for rendering\n");
            break;
    }
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

    Object objects[] = { //array of all the objects in the program, and an enum so they can be accessed by name. hacky? maybe, i dont know
        makeButton( 10, 10, 150, 50, "Button number 1", testFunction ),
        makeButton( 10, 70, 150, 50, "abcdefghijklmnopqrstuvwxyzabcdefg", testFunction1 )
    }; 
    enum objectNames {
        test,
        otherTest
    };


    // Program loop

    while(1) {
        SDL_PollEvent(&event);

        // Prevent unnecessary rendering (may need to remove later) When there are no events, just repoll
        if (event.type == SDL_EVENT_POLL_SENTINEL) continue; 

        switch (event.type) {
            case SDL_EVENT_QUIT:
                goto quit;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                SDL_GetMouseState(&mouseState.mouseDownX, &mouseState.mouseDownY);
                mouseState.mouseDownButton = event.button.button;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                SDL_GetMouseState(&mouseState.mouseUpX, &mouseState.mouseUpY);
                for (int i = 0; i < sizeof(objects)/sizeof(objects[0]); i++) {
                    onClick(&objects[i].button); //debug kinda
                }
                mouseState.mouseDownButton = 0;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                SDL_GetMouseState(&mouseState.currentX, &mouseState.currentY);
                break;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (int i = 0; i < sizeof(objects)/sizeof(objects[1]); i++) {
            renderObject(&objects[i]);
        }
        SDL_RenderPresent(renderer);
    }

    // Quitting

quit:

    SDL_Quit();

    printf("Quitting...\n");

    return 0;
}
