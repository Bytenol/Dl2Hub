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

struct
{
    Vec2 pos;
    float fov;      // in degrees
    float rotation; // in degrees
    //float viewFar;
} player;


struct Ray
{
    float angle;    // in radians
    bool isLeft;
    bool isUp;
    Vec2 pos;
};


std::vector<int> levelMap {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 1,
    1, 0, 1, 1, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 1,
    1, 0, 0, 0, 1, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,  
};

float RAY_INC;
const int MAP_COL = 8;
const int MAP_ROW = 8;
const int TILE_SIZE = 32;

void drawFilledCircle(SDL_Renderer *r, float px, float py, float radius);


float toRadian(float angleInDegrees) {
    return angleInDegrees * 3.14159f / 180;
}

float toDegree(float angleInRadians) {
    return angleInRadians * 180 / 3.14159f;
}


void init()
{
    player.fov = 60;
    player.pos.x = 45;
    player.pos.y = 75;
    player.rotation = -90.0f;
    RAY_INC = player.fov / float(canvas.w);
}


void update(float dt)
{

}


void render(SDL_Renderer* renderer)
{

    for(size_t i = 0; i < MAP_ROW; i++)
    for(size_t j = 0; j < MAP_COL; j++)
    {
        const int id = levelMap[i * MAP_COL + j];
        int px = j * TILE_SIZE;
        int py = i * TILE_SIZE;
        SDL_Rect rect{ px, py, TILE_SIZE, TILE_SIZE };
        if(id == 0) {
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
            SDL_RenderDrawRect(renderer, &rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }

    const int xPos = std::floor(player.pos.x / TILE_SIZE);
    const int yPos = std::floor(player.pos.y / TILE_SIZE);
    const float xPosOffset = player.pos.x - TILE_SIZE * xPos;
    const float yPosOffset = player.pos.y - TILE_SIZE * yPos;
    assert(xPosOffset >= 0);
    assert(yPosOffset >= 0);
    
    int j = 0.0f;
    for(float i = -player.fov/2; i < player.fov/2; i += RAY_INC)
    {
        j++;
        float angle = toRadian(player.rotation + i);
        auto rayDir = Vec2::fromAngle(angle, 50);
        float dxCoeff = (Vec2{ -1, 0 }).dotProduct(rayDir) >= 0.0f ? -1 : 1;
        float dyCoeff = (Vec2{ 0, -1 }).dotProduct(rayDir) >= 0.0f ? -1 : 1;

        float px = 0, py = 0;
        if(dyCoeff == -1) {
            float dy = player.pos.y - std::floor(player.pos.y / TILE_SIZE) * TILE_SIZE;
            float dx = std::tan(angle) * dy;
            float hyp = std::hypot(dx, dy);
            if(dxCoeff == -1) {
                px = player.pos.x + std::cos(angle) * hyp;
                py = player.pos.y + std::sin(angle) * hyp;
            } else if(dxCoeff == 1) {
                px = player.pos.x + std::cos(angle) * hyp;
                py = player.pos.y + std::sin(angle) * hyp;
            }
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
            SDL_RenderDrawLine(renderer, player.pos.x, player.pos.y, px, py);
            
        }
        
        if(dyCoeff == 1) {
            float dy = TILE_SIZE - yPosOffset;
            float dx = std::tan(angle) * dy;
            float hyp = std::hypot(dx, dy);
            if(dxCoeff == -1) {
                px = player.pos.x + std::cos(angle) * hyp;
                py = player.pos.y + std::sin(angle) * hyp;
            } else if(dxCoeff == 1) {
                px = player.pos.x + std::cos(angle) * hyp;
                py = player.pos.y + std::sin(angle) * hyp;
            }
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
            SDL_RenderDrawLine(renderer, player.pos.x, player.pos.y, px, py);
        }
        
    }

    float sdx = player.pos.x + std::cos(toRadian(player.rotation)) * 50;
    float sdy = player.pos.y + std::sin(toRadian(player.rotation)) * 50;
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
    SDL_RenderDrawLine(renderer, player.pos.x, player.pos.y, sdx, sdy);

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
    drawFilledCircle(renderer, player.pos.x, player.pos.y, 5);    

}


void processEvent(SDL_Event& evt, bool& shouldOpen) {
    if(evt.type == SDL_QUIT) {
        shouldOpen = false;
        return;
    }

    Vec2 vel = Vec2::fromAngle(toRadian(player.rotation));
    switch (evt.type)
    {
    case SDL_KEYDOWN:
        switch (evt.key.keysym.sym)
        {
        case SDLK_LEFT:
            player.rotation -= 1.0f;
            break;
        case SDLK_RIGHT:
            player.rotation += 1.0f;
            break;
        case SDLK_UP:
            player.pos += vel;
            break;
        case SDLK_DOWN:
            player.pos -= vel;
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
    canvas.h = 300;
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
