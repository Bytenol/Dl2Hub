#include <SDL3/SDL.h>
#include <iostream>

const int W = 640;
const int H = 480;

SDL_Window* window;
SDL_Renderer* renderer;


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("INITIALIZATION FAILED: %s", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Pong2D", W, H, SDL_WINDOW_RESIZABLE);

    std::cout << "Initialized SDL3" << std::endl;

    SDL_Delay(5000);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
