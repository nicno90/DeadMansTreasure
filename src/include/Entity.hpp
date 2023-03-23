#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Math.hpp"

class Entity{
    private:
        Vector2f pos;
        SDL_Rect currentFrame;
        SDL_Texture* texture;
    public:
        Entity(Vector2f p_pos, SDL_Texture* p_texture);
        void init();
        Vector2f getPos();
        SDL_Texture* getTexture();
        SDL_Rect getCurrentFrame();
        void move(float px, float py);
        void move(Vector2f p_v);
        
};

