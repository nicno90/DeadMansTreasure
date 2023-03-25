#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"

enum keyPressed {
    K_UP,
    K_DOWN,
    K_LEFT,
    K_RIGHT,
    K_ESC,
    K_W,
    K_S,
    K_A,
    K_D,
    
    K_LAST
};


class GameEngine {
    private:
        RenderWindow* window;
        Player* player;
        Vector2f directions[4];
        std::vector<Agent> agents;
        std::vector<Floor> floors;
        SDL_Texture* textures[T_LAST];
        void init_textures();
        void init_directions();
        bool inputs_pressed[K_LAST] = {false};
        void setKey(keyPressed key, SDL_Event* windowEvent);
        bool readInput(SDL_Event* windowEvent);
        void check_movement(Vector2f* pos_adj);
        void refresh_timing(int* startTicks, float* newTime, float* currentTime, float* frameTime, float* accumulator);
        void render_entities(std::vector<Entity>* p_entity);

        bool check_valid_move(Vector2f direction);

    public:
        GameEngine(RenderWindow* window, Player* p_player);
        int startGame();
};