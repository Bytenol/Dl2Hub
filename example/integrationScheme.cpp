#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

#include <SDL.h>

struct {
    SDL_Renderer* renderer = nullptr;
    int w = 320;
    int h = 150;
} canvas;


struct Vec2
{
    float x;
    float y;

    Vec2 operator+(const Vec2& v) {
        return { x + v.x, y + v.y };
    }

    Vec2& operator+=(const Vec2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vec2 operator-(const Vec2& v) {
        return { x - v.x, y - v.y };
    }

    Vec2 operator*(const float& s) {
        return { x * s, y * s };
    }

    Vec2& operator-=(const Vec2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    float dotProduct(const Vec2& v) {
        return x * v.x + y * v.y;
    }

    static Vec2 fromAngle(float angleInRadians, float scale = 1.0f) {
        Vec2 res;
        res.x = std::cos(angleInRadians) * scale;
        res.y = std::sin(angleInRadians) * scale;
        return res;
    }
};


struct Ball
{
    Vec2 pos;
    Vec2 vel;
    Vec2 acc;
    float radius;
    float mass = 1.0f;
    
} ball;


void drawFilledCircle(SDL_Renderer *r, float px, float py, float radius);


float toRadian(float angleInDegrees) {
    return angleInDegrees * 3.14159f / 180;
}

float toDegree(float angleInRadians) {
    return angleInRadians * 180 / 3.14159f;
}


void init()
{
    ball.pos.x = canvas.w / 2;
    ball.pos.y = 0;
    ball.radius = 20;
}

Vec2 getAcc(Vec2 vel) {
    float mg = ball.mass * 20;
    Vec2 weight{ 0, mg };
    Vec2 force = weight;
    Vec2 acc = force * (1 / ball.mass);
    return acc;
}

void eulerExplicit(Vec2& pos, Vec2& vel, Vec2& acc, float& dt) {
    pos += vel * dt;
    vel += acc * dt;
}

void eulerSemiImplicit(Vec2& pos, Vec2& vel, Vec2& acc, float& dt) {
    vel += acc * dt;
    pos += vel * dt;
}

void eulerSemiImplicit2(Ball& b, float& dt) {
    b.pos += b.vel * dt;
    b.acc = getAcc(b.vel);
    b.vel += b.acc * dt;
}

// Runge-kutta scheme RK2
void rk2(Ball& b, float dt) {
    auto p1 = b.pos;
    auto v1 = b.vel;
    auto a1 = getAcc(v1);
    auto p2 = p1 + v1 * dt;
    auto v2 = v1 + a1 * dt;
    auto a2 = getAcc(v2);
    b.pos += ((v1 + v2) * (dt/2));
    b.vel += ((a1 + a2) * (dt/2));
}


void update(float dt)
{   
    // eulerExplicit(ball.pos, ball.vel, acc, dt);
    // eulerSemiImplicit(ball.pos, ball.vel, acc, dt);
    rk2(ball, dt);
}


void render(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
    drawFilledCircle(renderer, ball.pos.x, ball.pos.y, ball.radius);
}


void processEvent(SDL_Event& evt, bool& shouldOpen) {
    if(evt.type == SDL_QUIT) {
        shouldOpen = false;
        return;
    }

    // Vec2 vel = Vec2::fromAngle(toRadian(player.rotation));
    switch (evt.type)
    {
    case SDL_KEYDOWN:
        switch (evt.key.keysym.sym)
        {
        case SDLK_LEFT:
            // player.rotation -= 1.0f;
            break;
        case SDLK_RIGHT:
            // player.rotation += 1.0f;
            break;
        case SDLK_UP:
            // player.pos += vel;
            break;
        case SDLK_DOWN:
            // player.pos -= vel;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
        
}


void mainLoop()
{
    SDL_Event evt;
    bool shouldOpen = true;
    while (shouldOpen)
    {
        while (SDL_PollEvent(&evt))
            processEvent(evt, shouldOpen);
        SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(canvas.renderer);
        render(canvas.renderer);
        update(1 / 60.0f);  //@todo: use real timing
        SDL_RenderPresent(canvas.renderer);
    }
    
}


void drawFilledCircle(SDL_Renderer *r, float px, float py, float radius)
{
    auto drawHorizontalLine = [](SDL_Renderer* renderer, int x1, int x2, int y) -> void {
        for (int x = x1; x <= x2; x++)
            SDL_RenderDrawPoint(renderer, x, y);
    };
    
    int x = 0;
    int y = radius;
    int d = 3 - int(radius) << 1;

    while (y >= x) {
        // Draw horizontal lines (scanlines) for each section of the circle
        drawHorizontalLine(r, px - x, px + x, py - y);
        drawHorizontalLine(r, px - x, px + x, py + y);
        drawHorizontalLine(r, px - y, px + y, py - x);
        drawHorizontalLine(r, px - y, px + y, py + x);

        // Update decision parameter and points
        if (d < 0) {
            d = d + (x << 2) + 6;
        } else {
            d = d + ((x - y) << 2) + 10;
            y--;
        }
        x++;
        
    }
}


int main(int argc, char const *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "INITIALIZATION_ERROR: " << SDL_GetError() << std::endl;
        return -1;
    }

    canvas.w = 640;
    canvas.h = 480;
    auto window = SDL_CreateWindow("RayCasting1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, canvas.w, canvas.h, SDL_WINDOW_SHOWN);
    if(!window) {
        std::cerr << "SDL_WINDOW_CREATION_ERROR: " << SDL_GetError() << std::endl; 
        return -1;
    }

    canvas.renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    if(!canvas.renderer) {
        std::cerr << "RENDERER_CREATION_FAILED: " << SDL_GetError() << std::endl;
        return -1;
    }

    init();
    mainLoop();

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
