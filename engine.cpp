#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <algorithm>

#include "Engine.hpp"
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"




GameEngine::GameEngine(RenderWindow* p_window, Player* p_player)
:window(p_window), player(p_player),  viking_chad(Agent(Vector2f(350, 300), textures[T_VIKING])){

    init_textures();
    init_directions();

    viking_chad = Agent(Vector2f(350, 300), textures[T_VIKING]);
    agents = {
        player,
        &viking_chad,
    };
    floors = {
        Floor(Vector2f(350, 250), textures[T_BIG_OPENING], FT_BIG_AREA), // Base floor
    };
    floors.push_back(Floor(textures[T_HORIZONTAL_PATH], FT_H_HALL, AP_RIGHT, &floors[0]));
    floors.push_back(Floor(textures[T_HORIZONTAL_PATH], FT_H_HALL_LEFT, AP_LEFT, &floors[0]));
    floors.push_back(Floor(textures[T_VERTICAL_PATH], FT_V_HALL_TOP, AP_TOP, &floors[0]));
    floors.push_back(Floor(textures[T_VERTICAL_PATH], FT_V_HALL, AP_BOTTOM, &floors[0]));
    floors.push_back(Floor(textures[T_VERTICAL_PATH], FT_V_HALL_BOTTOM, AP_BOTTOM, &floors[floors.size()-1]));
    floors.push_back(Floor(textures[T_HORIZONTAL_PATH], FT_H_HALL, AP_RIGHT, AP_BOTTOM, &floors[floors.size()-1]));
    floors.push_back(Floor(textures[T_BIG_OPENING], FT_BIG_AREA, AP_RIGHT, &floors[floors.size()-1]));
    floors.push_back(Floor(textures[T_VERTICAL_PATH], FT_V_HALL, AP_TOP, &floors[floors.size()-1]));
    floors.push_back(Floor(textures[T_VERTICAL_PATH], FT_V_HALL_TOP, AP_TOP, &floors[floors.size()-1]));
    

    std::sort(floors.begin(), floors.end());
}

int GameEngine::startGame(){
    bool gameRunning = true;
    SDL_Event windowEvent;
    const float timeStep = 0.01f;
    float accumulator = 0.0f;
    float currentTime =  utils::hireTimeInSeconds();

    int startTicks;
    float newTime;
    float frameTime;

    Vector2f pos_delta = Vector2f();

    // Game Loop:
    while (gameRunning){

        refresh_timing(&startTicks, &newTime, &currentTime, &frameTime, &accumulator);
        
        while (accumulator >= timeStep){
            gameRunning = readInput(&windowEvent);
            accumulator -= timeStep;
        }
        
        check_movement(&pos_delta);

        window->clear();
        for (Floor& f : floors){
            f.move(pos_delta);
            window->render(&f);
        }
        order_agents();
        for (Agent* f : agents){
            if (dynamic_cast<Player*>(f) == nullptr){
                f->move(pos_delta);
            }
            window->render(f);
        }
        window->display();
    }
    window->cleanUp();
    SDL_Quit();
    return EXIT_SUCCESS;
}

void GameEngine::init_textures(){
    textures[T_KNIGHT] = player->getTexture();
    textures[T_BIG_OPENING] = window->loadTexture("res/images/big_opening.png");
    textures[T_VERTICAL_PATH] = window->loadTexture("res/images/vertical_path.png");
    textures[T_VIKING] = window->loadTexture("res/images/viking.png");
    textures[T_CHEST] = window->loadTexture("res/images/chest.png");
    textures[T_HORIZONTAL_PATH] = window->loadTexture("res/images/horizontal_path.png");
    textures[T_BL_CORNER] = window->loadTexture("res/images/bl_corner.png");
    textures[T_TL_CORNER] = window->loadTexture("res/images/tl_corner.png");
    }

void GameEngine::init_directions(){
    float v = 1.0;
    directions[K_UP] = Vector2f(0.0, 1 * (v));
    directions[K_DOWN] = Vector2f(0.0, -1 * (v));
    directions[K_LEFT] = Vector2f(1 * (v), 0.0);
    directions[K_RIGHT] = Vector2f(-1 * (v), 0.0);
}

bool GameEngine::readInput(SDL_Event* windowEvent){
    while (SDL_PollEvent(windowEvent)){
        switch (windowEvent->key.keysym.sym) {
            case SDLK_UP:
                setKey(K_UP, windowEvent);
                break;
            case SDLK_DOWN:
                setKey(K_DOWN, windowEvent);
                break;
            case SDLK_LEFT:
                setKey(K_LEFT, windowEvent);
                break;
            case SDLK_RIGHT:
                setKey(K_RIGHT, windowEvent);
                break;
            case SDLK_ESCAPE:
                setKey(K_ESC, windowEvent);
                return false;
        }
        if (windowEvent->type == SDL_QUIT){
            return false;
        }
    }
    return true;
}

void GameEngine::setKey(keyPressed key, SDL_Event* windowEvent){
    inputs_pressed[key] = windowEvent->type == SDL_KEYDOWN;
}

void GameEngine::check_movement(Vector2f* pos_delta){
    pos_delta->reset();
    if (inputs_pressed[K_UP] && check_valid_move(directions[K_UP])) {
        pos_delta->adjust(directions[K_UP]);
    }
    if (inputs_pressed[K_DOWN] && check_valid_move(directions[K_DOWN])){
        pos_delta->adjust(directions[K_DOWN]);
    }
    if (inputs_pressed[K_LEFT] && check_valid_move(directions[K_LEFT])){
        pos_delta->adjust(directions[K_LEFT]);
    }
    if (inputs_pressed[K_RIGHT] && check_valid_move(directions[K_RIGHT])){
        pos_delta->adjust(directions[K_RIGHT]);
    }
}

void GameEngine::refresh_timing(int* startTicks, float* newTime, float* currentTime, float* frameTime, float* accumulator){
    *startTicks = SDL_GetTicks();
    *newTime = utils::hireTimeInSeconds();
    *frameTime = *newTime - *currentTime;
    *currentTime = *newTime;
    *accumulator += *frameTime;
}

void GameEngine::order_agents(){
    int j;
    Agent* key;
    for (int i = 1; i < agents.size(); ++i) {
        key = agents[i];
        j = i - 1;
        while (j >= 0 && key->getPos()->y < agents[j]->getPos()->y) {
            agents[j + 1] = agents[j];
            --j;
        }
        agents[j + 1] = key;
    }
}

bool GameEngine::check_valid_move(Vector2f direction){
    SDL_Rect adjustedRect = {
        (int)(player->getPos()->x - (direction.x) + 8), (int)(player->getPos()->y - (direction.y)), 
        player->getRect()->w - 8, player->getRect()->h
    };
    for (Floor& f : floors)
    {
        if (f.check_collision(&adjustedRect))
        {
            return true;
        }
    }
    return false;
}

