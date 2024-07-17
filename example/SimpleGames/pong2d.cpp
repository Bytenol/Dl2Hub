#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>
#include <cmath>

// TODO: reposition player init position to the middle

const int W = 640;
const int H = 480;
constexpr int circSplit = 20;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event evt;
std::vector<std::vector<float>> circleGeometry;

bool init(const char* title, int w, int h);
bool onCreate();
bool onUpdate(float dt);
bool onDraw();
bool onPollEvent(SDL_Event& evt);
bool onExit();
bool mainLoop();

struct Player {
    static SDL_FRect drawRect;

    float x;
    float y;
    int score;
} player, opponent;


int main(int argc, char* argv[]) {
    if (!init("Pong2D", W, H)) {
        SDL_Log("INITIALIZATION FAILED: %s", SDL_GetError());
        return -1;
    }

    onCreate();
    mainLoop();
    onExit();

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


SDL_FRect Player::drawRect = {0.0f, 0.0f, W * 0.02f, H * 0.2f};

bool onUpdate(float dt) {
    return true;
}


bool onDraw() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderLine(renderer, W * 0.5f, 0.0f, W * 0.5f, H);

    SDL_Vertex vertices[20 * 3];
    int i = 0;
    for(const auto& j: circleGeometry) {
        vertices[i].position.x = 100 + j[0] * W * 0.04f;
        vertices[i].position.y = 100 + j[1] * W * 0.04f;
        vertices[i].color.r = 0x00;
        vertices[i].color.g = 0x3d;
        vertices[i].color.b = 0xff;
        vertices[i].color.a = 0xff;
        i++;
    }
    SDL_RenderGeometry(renderer, nullptr, vertices, 20 * 3, nullptr, 0);

    Player::drawRect.x = opponent.x;
    Player::drawRect.y = opponent.y;
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
    SDL_RenderFillRect(renderer, &Player::drawRect);

    Player::drawRect.x = player.x;
    Player::drawRect.y = player.y;
    SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);
    SDL_RenderFillRect(renderer, &Player::drawRect);

    SDL_RenderPresent(renderer);
    return true;
}


bool onPollEvent(SDL_Event& evt) {
    if(evt.type == SDL_EVENT_QUIT)
        return false;

    return true;
}


bool mainLoop() {
    bool windowShouldClose = false;

    while (!windowShouldClose)
    {
        while(SDL_PollEvent(&evt)) 
            windowShouldClose = !onPollEvent(evt);

        onUpdate(0.0f);
        onDraw();
    }
    
    return true;
}


bool onCreate() {
    float spacing = W * 0.10f;
    float midY = (H - Player::drawRect.h) * 0.5f;
    opponent.x = spacing;
    opponent.y = midY;

    player.x = W - spacing - Player::drawRect.w;
    player.y = midY;

    const int circStep = 360 / circSplit;
    for(int i = 0; i < 360; i += circStep) {
        float a1 = i * 180 / 3.1415f;
        float a2 = (i + circStep) * 180 / 3.1415f;
        float x1 = std::cos(a1) * 0.5f;
        float y1 = std::sin(a1) * 0.5f;
        float x2 = std::cos(a2) * 0.5f;
        float y2 = std::sin(a2) * 0.5f;
        circleGeometry.push_back({ 0.0f, 0.0f });
        circleGeometry.push_back({ x1, y1 });
        circleGeometry.push_back({ x2, y2 });
    }

    return true;
}

bool onExit() {
    return true;
}


bool init(const char* title, int w, int h) {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) return false;

    window = SDL_CreateWindow(title, w, h, SDL_WINDOW_RESIZABLE);
    if(!window) return false;

    renderer = SDL_CreateRenderer(window, nullptr);
    if(!renderer) {
        SDL_DestroyWindow(window);
        return false;
    }

    return true;
}