#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Entity.hpp"

class RenderWindow{
    public:
        RenderWindow(const char* p_title, int p_width, int p_height);
        SDL_Texture* loadTexture(const char* p_filePath);
        

        void setIcon(SDL_Surface* icon);
        void cleanUp();
        void clear();
        void render(Entity* p_texture);
        void display();
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;

};