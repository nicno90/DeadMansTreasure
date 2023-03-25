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
:window(p_window), player(p_player){

    init_textures();
    init_directions();

    // player = Entity(Vector2f(400, 300), knight);
    agents = {
        Agent(Vector2f(350, 300), textures[T_VIKING]),
    };
    floors = {
        // Floor(Vector2f(350, 250), textures[T_BIG_OPENING]),
        Floor(Vector2f(400, 435), textures[T_VERTICAL_PATH], FT_V_HALL_BOTTOM),
        Floor(Vector2f(400, 235), textures[T_HORIZONTAL_PATH], FT_H_HALL_LEFT),
        Floor(Vector2f(438, 235), textures[T_HORIZONTAL_PATH], FT_H_HALL_RIGHT),
        Floor(Vector2f(400, 245), textures[T_VERTICAL_PATH], FT_V_HALL_TOP),
        Floor(Vector2f(480, 340), textures[T_HORIZONTAL_PATH], FT_H_HALL_RIGHT),
        Floor(Vector2f(300, 340), textures[T_HORIZONTAL_PATH], FT_H_HALL_LEFT),
        Floor(Vector2f(350, 300), textures[T_BIG_OPENING], FT_BIG_AREA),
    };
    Entity* entities_arr[1 + sizeof(floors) + sizeof(agents)];
    
    entities_arr[0] = player;
    for (int i = 1; i < 1 + sizeof(floors); i ++){
        entities_arr[i] = &floors[i-1];
    }
    for (int i = 1 + sizeof(floors); i < 1 + sizeof(floors) + sizeof(agents); i++){
        entities_arr[i] = &agents[i-1-sizeof(floors)];
    }
    std::vector<Entity*> entities(entities_arr, entities_arr + (sizeof(entities_arr) / sizeof(entities_arr[0])));
    std::sort(entities.begin(), entities.end());
    
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
            window->render(f);
        }
        for (Agent& f : agents){
            f.move(pos_delta);
            window->render(f);
        }
        window->render(*player);
        window->display();
    }
    window->cleanUp();
    SDL_Quit();
    return EXIT_SUCCESS;
}

void GameEngine::init_textures(){
    textures[T_KNIGHT] = player->getTexture();
    printf("");
    textures[T_BIG_OPENING] = window->loadTexture("res/images/big_opening.png");
    printf("");
    textures[T_VERTICAL_PATH] = window->loadTexture("res/images/vertical_path.png");
    printf("");
    textures[T_VIKING] = window->loadTexture("res/images/viking.png");
    // printf("");
    // textures[T_CHEST] = window->loadTexture("res/images/chest.png");
    // printf("");
    // textures[T_HORIZONTAL_PATH] = window->loadTexture("res/images/horizontal_path.png"); // Not sure why this crashes it
    printf("");
    // textures[T_BL_CORNER] = window->loadTexture("res/mages/bl_corner.png");
    printf("");
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

