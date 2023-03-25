#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <tuple>
#include <string>

#include "Math.hpp"


enum Texture {
    T_VIKING,
    T_KNIGHT,
    T_BIG_OPENING,
    T_VERTICAL_PATH,
    T_HORIZONTAL_PATH,
    T_BL_CORNER,
    T_CHEST
};

class Entity{
    private:
        Vector2f pos;
        SDL_Rect currentFrame;
        SDL_Texture* texture;
    public:
        Entity(Vector2f p_pos, SDL_Texture* p_texture);
        virtual Vector2f* getPos();
        SDL_Rect* getRect();
        SDL_Texture* getTexture();
        void setTexture(SDL_Texture* p_texture);
        SDL_Rect getCurrentFrame();
        void move(float px, float py);
        void move(Vector2f p_v);
        static bool inFront(Entity* e1, Entity* e2);
        virtual bool operator <(Entity& e);
};

class Agent: public Entity{
    private:
        float maxHealth;
        float health;
        int level;
    public:
        Agent(Vector2f p_pos, SDL_Texture* p_texture) : Entity(p_pos, p_texture){};
        float getHealth();
        int getLevel();
        bool hit(int damage);
        void get_adjusted_rect(Vector2f* direction);
};

class Player: public Agent{
    private:
        int coins = 0;
        float xp = 0;

    public:
        Player(SDL_Texture* p_texture): Agent(Vector2f(400, 300), p_texture){};
        void setTexture(SDL_Texture* p_texture){setTexture(p_texture);};
        int getCoins(){return coins;};
        float getXP(){return xp;};
        void add_coins(int p_coins){coins += p_coins;};
        void add_xp(float p_xp){xp += p_xp;};

        void get_adjusted_rect(Vector2f* direction){Agent::get_adjusted_rect(direction);};

};


enum FloorType{
    FT_BLANK,
    FT_BIG_AREA,
    FT_V_HALL,
    FT_V_HALL_TOP,
    FT_V_HALL_BOTTOM,
    FT_H_HALL,
    FT_H_HALL_LEFT,
    FT_H_HALL_RIGHT,
    FT_TL_CORNER, 
    FT_TR_CORNER,
    FT_BR_CORNER,
    FT_BL_CORNER // L <- what this corner looks like
};


class Floor: public Entity{
    private:
        FloorType fType;
    public:
        Floor(Vector2f p_pos, SDL_Texture* p_texture): Entity(p_pos, p_texture), fType(FT_BLANK){};
        Floor(Vector2f p_pos, SDL_Texture* p_texture, FloorType p_fType): Entity(p_pos, p_texture), fType(p_fType){};
        
        void conform_to_type();
        bool check_collision(SDL_Rect* adj_player);
        virtual bool operator < (Floor& f);
        
        FloorType getType(){return fType;};
        
        bool ignore_top_collison();
        bool ignore_bottom_collison();
        bool ignore_left_collison();
        bool ignore_right_collison();
        int x_mod();
        int y_mod();
        
};