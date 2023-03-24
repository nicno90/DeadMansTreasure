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
        SDL_Rect* getRect();
        SDL_Texture* getTexture();
        SDL_Rect getCurrentFrame();
        void move(float px, float py);
        void move(Vector2f p_v);
        
};

class Agent: Entity{
    private:
        float health;
        int level;
    public:
        Agent(Vector2f p_pos, SDL_Texture* p_texture) : Entity(p_pos, p_texture){};
        float getHealth();
        int getLevel();
        bool hit(int damage);

};

class Player: Agent{
    private:
        int coins = 0;
        float xp = 0;

    public:
        Player();
        int getCoins(){return coins;};
        float getXP(){return xp;};
        void add_coins(int p_coins){coins += p_coins;};
        void add_xp(float p_xp){xp += p_xp;};

};

class Floor: Entity{
    private:

};