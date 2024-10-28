#include <iostream>
#include <SDL.h>
#include <memory>
#include <vector>
#include <cmath>

#include "./include/rcc.h"


struct {
    unsigned int w;
    unsigned int h;
    SDL_Renderer* renderer;
} canvas;


std::unique_ptr<rcc::World> world;

std::vector<int> levelMap {
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,1,0,1,
    1,0,0,1,0,0,0,1,
    1,0,1,1,1,1,0,1,
    1,0,0,1,0,1,0,1,
    1,0,0,0,1,0,0,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,  
};


rcc::RayCastable player;


void init()
{
    // setup and initialize world
    rcc::Vector worldPos{ 0.0f, 0.0f };
    rcc::Vector worldSize{ static_cast<float>(canvas.w * 0.5), static_cast<float>(canvas.h * 0.5) };

    world = rcc::createWorld(64, worldSize);
    world->setWorldInfo(levelMap, 8, 8);
    world->setPlayer(player);

    // setup and initialize player
    player = rcc::RayCastable(60.0f, 0.0f, world->getSize().x);
    player.pos.x = 276.0f;
    player.pos.y = 276.0f;

    auto g = rcc::RayCastable{ 45.0f, 0.0f, 3 };
    g.pos.x = 80;
    g.pos.y = 80;
    world->addCastable(g);

    g = rcc::RayCastable{ 45.0f, 90.0f, 30 };
    g.pos.x = 80;
    g.pos.y = 280;
    world->addCastable(g);
}

void update(float dt)
{
    // player.castRay(*world);
    world->update(dt);
}


void render(SDL_Renderer* renderer)
{

    auto minMapPos = rcc::Vector{ world->getSize().x, 0 };

    // render minMap
    for(size_t i = 0; i < world->getRowSize(); i++) 
    {
        for(size_t j = 0; j < world->getColSize(); j++)
        {
            const int& id = world->getMapId(i, j);
            int px = minMapPos.x + j * world->getTileSize();
            int py = minMapPos.y + i * world->getTileSize();
            SDL_Rect rect{ px, py, world->getTileSize(), world->getTileSize() };

            if(id == 0) {
                // SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
                // SDL_RenderFillRect(renderer, &rect);
                // SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
                // SDL_RenderDrawRect(renderer, &rect);
            } else {
                SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
                SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }

    // render player
    for(auto entity = world->getCastables().begin(); entity != world->getCastables().end(); entity++)
    {
        int entitySize = world->getTileSize() * 0.1;
        int px = entity->pos.x - entitySize * 0.5;
        int py = entity->pos.y - entitySize * 0.5;
        SDL_Rect rect{ int(minMapPos.x + px), int(minMapPos.y + py), entitySize, entitySize};
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
        SDL_RenderFillRect(renderer, &rect);

        const auto& rays = entity->getRays();
        SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
        int i = 0;
        float maxDist = 200.0f;
        for(auto it = rays.begin(); it != rays.end(); it++)
        {
            if(it->dist < maxDist) {
                float h = (maxDist / it->dist) * 64;
                float py = world->getSize().y * 0.5 - h * 0.5;

                SDL_SetRenderDrawColor(renderer, 0x00, 0x32, 0xaa, 0xff);
                SDL_RenderDrawLineF(renderer, i, 0, i, py);

                if(it->isVert) SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
                else SDL_SetRenderDrawColor(renderer, 0xaa, 0x00, 0x00, 0xff);
                SDL_RenderDrawLineF(renderer, i, py, i, h);
            }
            
            i++;

            SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
            SDL_RenderDrawLineF(renderer, minMapPos.x + it->start.x, minMapPos.y + it->start.y, 
                minMapPos.x + it->end.x, minMapPos.y + it->end.y);
        }
    }

    int playerSize = world->getTileSize() * 0.1;
    int px = player.pos.x - playerSize * 0.5;
    int py = player.pos.y - playerSize * 0.5;
    SDL_Rect rect{ int(minMapPos.x + px), int(minMapPos.y + py), playerSize, playerSize};
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
    SDL_RenderFillRect(renderer, &rect);

    const auto& rays = player.getRays();
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
    int i = 0;
    float maxDist = 200.0f;
    for(auto it = rays.begin(); it != rays.end(); it++)
    {
        if(it->dist < maxDist) {
            float h = (maxDist / it->dist) * 64;
            float py = world->getSize().y * 0.5 - h * 0.5;

            SDL_SetRenderDrawColor(renderer, 0x00, 0x32, 0xaa, 0xff);
            SDL_RenderDrawLineF(renderer, i, 0, i, py);

            if(it->isVert) SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
            else SDL_SetRenderDrawColor(renderer, 0xaa, 0x00, 0x00, 0xff);
            SDL_RenderDrawLineF(renderer, i, py, i, h);
        }
        
        i++;

        SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
        SDL_RenderDrawLineF(renderer, minMapPos.x + it->start.x, minMapPos.y + it->start.y, 
            minMapPos.x + it->end.x, minMapPos.y + it->end.y);
    }

    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
    SDL_RenderDrawLine(renderer, 0, world->getSize().y, world->getSize().x, world->getSize().y);
}


void processEvent(SDL_Event& evt, bool& shouldClose)
{
    if(evt.type == SDL_QUIT) {
        shouldClose = true;
        return;
    }

    if(evt.type == SDL_KEYDOWN) {
        switch (evt.key.keysym.sym)
        {
        case SDLK_UP:
            player.vel.x = std::cos(player.rotation * 3.14159f / 180);
            player.vel.y = std::sin(player.rotation * 3.14159f / 180);
            player.pos += player.vel;
            break;
        case SDLK_LEFT:
            player.rotation -= 1.0f;
            if(player.rotation < -360) player.rotation = 0;
            std::cout << player.rotation << std::endl;
            break;
        case SDLK_RIGHT:
            player.rotation += 1.0f;
            if(player.rotation > 360) player.rotation = 0;
            std::cout << player.rotation << std::endl;
            break;
        case SDLK_DOWN:
            player.vel.x = std::cos(player.rotation * 3.14159f / 180);
            player.vel.y = std::sin(player.rotation * 3.14159f / 180);
            player.pos -= player.vel;
            break;
        }
    }
}


void mainLoop()
{
    bool shouldQuit = false;
    SDL_Event evt;

    while (!shouldQuit)
    {
        while (SDL_PollEvent(&evt))
            processEvent(evt, shouldQuit);
        
        SDL_SetRenderDrawColor(canvas.renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(canvas.renderer);
        render(canvas.renderer);
        update(1 / 60.0f);
        SDL_RenderPresent(canvas.renderer);
    }
    
}


int main(int argc, char const *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Unable to initialize SDL" << std::endl;
        return -1;
    }

    canvas.w = 1024;
    canvas.h = canvas.w / 2;
    auto window = SDL_CreateWindow("RayCasting3D", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, canvas.w, canvas.h, SDL_WINDOW_SHOWN);
    if(!window) {
        std::cerr << "Unable to create window" << std::endl;
        return -1;
    }

    canvas.renderer = SDL_CreateRenderer(window, 0, 0);
    if(!canvas.renderer) {
        std::cerr << "Unable to initialize renderer" << std::endl;
        return -1;
    }

    init();
    mainLoop();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
