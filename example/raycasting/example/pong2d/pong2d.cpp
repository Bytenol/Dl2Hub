/**
 * @file pong2d.cpp
 * @date Aug 2024
 * Modified: 6th oct, 2024
 * 
 * KNOWN ISSUES :
 * - issues with dt
 * - Ball get stucked on paddle sometimes: (Resolve with collision resolution)
 * - Use text instead of console to display game information
 */
#include <cmath>
#include <chrono>
#include <iostream>
#include <random>

#include <SDL3/SDL.h>
#include <emscripten/emscripten.h>


int W = 640;
int H = 480;
const float BALL_DIAM = W * 0.025f;
const float MAX_SPEED = 40.0f;
constexpr int circSplit = 20;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event evt;
std::mt19937 gen;


struct Vec2 { float x, y; } ball, ballVelocity;


struct Player {
    static SDL_FRect drawRect;

    Vec2 position;
    float speed = 0.0f;
    int score = 0;

} player, opponent;


enum class GameState {
    RESET,
    RESTART,
    PAUSE,
    PLAYING,
    OVER,
};


bool init(const char* title, int w, int h);
bool onCreate();
bool onUpdate(float dt);
bool onDraw();
bool onPollEvent(SDL_Event& evt, bool& windowShouldClose);
bool onExit();

void onReset();
void onRestart();
void onGameOver();
void drawFilledCircle(SDL_Renderer* renderer, const float& x, const float& y, const float& radius);
void mainLoop();

float randRange(float min, float max);
void collideWorldBoundary(Player& p);
bool isBallAndPlayerCollision(Player& paddle, Vec2 ball);

GameState state = GameState::RESET;
decltype(std::chrono::system_clock::now().time_since_epoch().count()) t1;
bool windowShouldClose = false;



#ifdef EMSCRIPTEN
extern "C" {

    EMSCRIPTEN_KEEPALIVE 
    void setCanvasSize(int width, int height) {
        W = width;
        H = height;
        std::cout << "set size called" << std::endl;
        emscripten_set_canvas_size(W, H);
    }

    EMSCRIPTEN_KEEPALIVE
    int ymain(int argc, const char** argv)

#else
    int main(int argc, const char** argv)
#endif
// main block content
{
    if (!init("Pong2D", W, H)) {
        SDL_Log("INITIALIZATION FAILED: %s", SDL_GetError());
        return -1;
    }

    onCreate();
    mainLoop();
    onExit();
    return 0;
}

#ifdef EMSCRIPTEN
}
#endif


SDL_FRect Player::drawRect = {0.0f, 0.0f, W * 0.02f, H * 0.2f};


void collideWorldBoundary(Player& p) {
    if(p.position.y <= 0.0f) p.position.y = 0.0f;
    if(p.position.y + Player::drawRect.h >= H) p.position.y = H - Player::drawRect.h;
}


bool isBallAndPlayerCollision(Player& paddle, Vec2 ball) {
    if (ball.y >= paddle.position.y && ball.y <= paddle.position.y + Player::drawRect.h) {
        const auto dx = std::abs(paddle.position.x - ball.x);
        if (dx <= BALL_DIAM) return true;
        return false;
    }
    return false;
}


bool onUpdate(float dt) {
    ball.x += ballVelocity.x * dt;
    ball.y += ballVelocity.y * dt;

    player.position.y += player.speed * dt;
    opponent.position.y = ball.y - 0.5f * Player::drawRect.h;

    collideWorldBoundary(player);
    collideWorldBoundary(opponent);

    if(ball.x < -BALL_DIAM) {
        ball.x = BALL_DIAM;
        ballVelocity.x *= -1.0f;
    }

    if(ball.x + BALL_DIAM > W) {
        ball.x = W - BALL_DIAM;
        ballVelocity.x *= -1.0f;
    }

    if(ball.y < -BALL_DIAM) {
        ball.y = BALL_DIAM;
        ballVelocity.y *= -1.0f;
    }

    if(ball.y + BALL_DIAM > H) {
        ball.y = H - BALL_DIAM;
        ballVelocity.y *= -1.0f;
    }

    if(ball.x > player.position.x && !(state == GameState::OVER)) {
        onGameOver();
    }

    auto bOldPos = ball;  // ballPosition before collision
    if (isBallAndPlayerCollision(player, ball) || isBallAndPlayerCollision(opponent, ball)) {
        ball.x = bOldPos.x;
        ball.y = bOldPos.y;
        ballVelocity.x *= -1.0f;
    }

    return true;
}


bool onDraw() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderLine(renderer, W * 0.5f, 0.0f, W * 0.5f, H);

    SDL_SetRenderDrawColor(renderer, 0x34, 0x54, 0xf2, 0xff);
    drawFilledCircle(renderer, ball.x, ball.y, BALL_DIAM / 2);

    Player::drawRect.x = opponent.position.x;
    Player::drawRect.y = opponent.position.y;
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
    SDL_RenderFillRect(renderer, &Player::drawRect);

    Player::drawRect.x = player.position.x;
    Player::drawRect.y = player.position.y;
    SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);
    SDL_RenderFillRect(renderer, &Player::drawRect);

    SDL_RenderPresent(renderer);
    return true;
}


bool onPollEvent(SDL_Event& evt, bool& windowShouldClose) {
    bool res = true;
    switch(evt.type) {
        case SDL_EVENT_QUIT:
            windowShouldClose = true;
            res = false;
        case SDL_EVENT_KEY_DOWN:
            switch (evt.key.key) {
                case SDLK_UP:
                    player.speed = -MAX_SPEED;
                    break;
                case SDLK_DOWN:
                    player.speed = MAX_SPEED;
                    break;
            }
            break;
        case SDL_EVENT_KEY_UP:
            switch(evt.key.key) {
                case SDLK_SPACE:
                    switch(state) {
                        case GameState::RESET:
                            onRestart();
                            break;
                        case GameState::OVER:
                            onReset();
                            break;
                        case GameState::PAUSE:
                        case GameState::PLAYING:
                        case GameState::RESTART:
                            break;
                    }
                    break;
            }
            player.speed = 0.0f;
            break;
    }

    return res;
}


void gameLoop() 
{
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    auto dt = (now - t1) * 10e-9f;
    t1 = now;  

    if(state != GameState::PLAYING) dt = 0.0f;

    while(SDL_PollEvent(&evt)) 
        onPollEvent(evt, windowShouldClose);

    onUpdate(dt);
    onDraw();
}


void mainLoop() {
    #ifdef EMSCRIPTEN
        emscripten_set_main_loop(gameLoop, 0, 1);
        // emscripten_set_main_loop_timing(EMSCRIPTEN_SET_MAIN_LOOP_TIMING_RAF, 0, nullptr);
    #else
        while (!windowShouldClose) 
            gameLoop();
    #endif
}



float randRange(float min, float max) {
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}


void onReset() {
    state = GameState::RESET;
    std::cout << "Press the space key to start" << std::endl;
    float spacing = W * 0.06f;
    float midY = (H - Player::drawRect.h) * 0.5f;
    opponent.position.x = spacing;
    opponent.position.y = midY;

    player.position.x = W - spacing - Player::drawRect.w;
    player.position.y = midY;

    ball.x = W * 0.5f;
    ball.y = H * 0.5f;
    t1 = std::chrono::system_clock::now().time_since_epoch().count();
}


void onGameOver() {
    std::cout << "GameOver..." << std::endl;
    std::cout << (ball.x > W ? "You wins...Shame!" : "Computer win! shame") << std::endl;
    state = GameState::OVER;
    std::cout << "Press the space key to reset" << std::endl;
}


void onRestart() {
    state = GameState::PLAYING;
    ballVelocity.x = randRange(20, 30);
    ballVelocity.y = randRange(20, 30);
    ballVelocity.x *= (randRange(0.0f, 1.0f) > 0.5f ? 1.0f : -1.0f);
    ballVelocity.y *= (randRange(0.0f, 1.0f) > 0.5f ? 1.0f : -1.0f);
    std::cout << std::flush;
}


bool onCreate() {
    std::random_device rd;
    gen = std::mt19937(rd());

    std::cout << "Game initializing....\n";

    std::cout << "Initialization done...Ready to start\n";
    onReset();
    std::cout << std::flush;
    return true;
}


bool onExit() {
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << std::flush;    // incase there is something hanging on the stdout buffer
    return true;
}


void drawFilledCircle(SDL_Renderer* renderer, const float& px, const float& py, const float& radius)
{
    auto drawHorizontalLine = [](SDL_Renderer* renderer, int x1, int x2, int y) -> void {
        for(int x = x1; x <= x2; x++)
            SDL_RenderPoint(renderer, x, y);
    };

    int x = 0;
    int y = radius;
    int d = 3 - (int(radius) << 1);

    while (y >= x)
    {
        drawHorizontalLine(renderer, px - x, px + x, py - y);
        drawHorizontalLine(renderer, px - x, px + x, py + y);
        drawHorizontalLine(renderer, px - y, px + y, py - x);
        drawHorizontalLine(renderer, px - y, px + y, py + x);

        if(d < 0) 
            d = d + (x << 2) + 6;
        else {
            d = d + ((x - y) << 2) + 10;
            y--;
        }
        x++;
    }
    
}


bool init(const char* title, int w, int h) {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) return false;
    std::cout << "SDL3 initialized\n";

    window = SDL_CreateWindow(title, w, h, SDL_WINDOW_RESIZABLE);
    if(!window) return false;
    std::cout << "window object created successfully\n";

    renderer = SDL_CreateRenderer(window, nullptr);
    if(!renderer) {
        SDL_DestroyWindow(window);
        return false;
    }

    std::cout << "renderer object created successfully\n";

    return true;
}