#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
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

void RenderWindow::setIcon(SDL_Surface* icon){
    SDL_SetWindowIcon(window, icon);
}

void RenderWindow::cleanUp(){
    SDL_DestroyWindow(window);
}

void RenderWindow::clear(){
    SDL_RenderClear(renderer);
}

void RenderWindow::render(Entity* p_entity){
    SDL_Rect img_src;
    img_src.x = p_entity->getCurrentFrame().x;
    img_src.y = p_entity->getCurrentFrame().y;
    img_src.w = p_entity->getCurrentFrame().w;
    img_src.h = p_entity->getCurrentFrame().h;

    SDL_Rect img_dst;
    img_dst.x = p_entity->getPos()->x;
    img_dst.y = p_entity->getPos()->y;
    img_dst.w = p_entity->getCurrentFrame().w;
    img_dst.h = p_entity->getCurrentFrame().h;

    SDL_RenderCopy(renderer, p_entity->getTexture(), &img_src, &img_dst);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, p_entity->getText());
    SDL_Rect text_dst = { img_dst.x, img_dst.y, 0, 0};
    SDL_RenderCopy(renderer, text_texture, NULL, &text_dst);

}

void RenderWindow::display(){
    SDL_RenderPresent(renderer);
}