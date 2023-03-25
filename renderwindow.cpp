#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "RenderWindow.hpp"
#include "Entity.hpp"

RenderWindow::RenderWindow(const char* p_title, int p_width, int p_height)
:window(NULL), renderer(NULL){
    window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_width, p_height, SDL_WINDOW_SHOWN);

    if (window == NULL){
        std::cout << "Window failed to init. ERROR: " << SDL_GetError << std::endl;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

SDL_Texture* RenderWindow::loadTexture(const char* p_filePath){
    SDL_Texture* texture = NULL;
    texture = IMG_LoadTexture(renderer, p_filePath);

    if (texture == NULL)
        std::cout << "Failed to load texture. Error: " << SDL_GetError() << std::endl;
    
    return texture;
}

void RenderWindow::cleanUp(){
    SDL_DestroyWindow(window);
}

void RenderWindow::clear(){
    SDL_RenderClear(renderer);
}

void RenderWindow::render(Entity& p_entity){
    SDL_Rect img_src;
    img_src.x = p_entity.getCurrentFrame().x;
    img_src.y = p_entity.getCurrentFrame().y;
    img_src.w = p_entity.getCurrentFrame().w;
    img_src.h = p_entity.getCurrentFrame().h;

    SDL_Rect img_dst;
    img_dst.x = p_entity.getPos()->x;
    img_dst.y = p_entity.getPos()->y;
    img_dst.w = p_entity.getCurrentFrame().w;
    img_dst.h = p_entity.getCurrentFrame().h;

    SDL_RenderCopy(renderer, p_entity.getTexture(), &img_src, &img_dst);
}

void RenderWindow::display(){
    SDL_RenderPresent(renderer);
}