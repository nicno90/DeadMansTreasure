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
    K_D
};


class GameEngine {
    private:
        RenderWindow* window;
        Entity* player;
        std::vector<Entity> entities;
        bool inputs_pressed[sizeof(keyPressed)] = {false};
        void setKey(keyPressed key, SDL_Event* windowEvent);
        bool readInput(SDL_Event* windowEvent);
        void check_movement(Vector2f* pos_adj);
        void refresh_timing(int* startTicks, float* newTime, float* currentTime, float* frameTime, float* accumulator);

    public:
        GameEngine(RenderWindow* window, Entity* p_player);
        int startGame();
};