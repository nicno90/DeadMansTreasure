#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "Engine.hpp"
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"




GameEngine::GameEngine(RenderWindow* p_window, Entity* p_player):
window(p_window), player(p_player){

    SDL_Texture* big_opening = window->loadTexture("res/images/big_opening.png");
    SDL_Texture* viking = window->loadTexture("res/images/viking.png");

    // player = Entity(Vector2f(400, 300), knight);
    entities = {
        
        Entity(Vector2f(350, 250), big_opening),
        Entity(Vector2f(350, 250), viking),
    };
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
    if (inputs_pressed[K_UP]){
            pos_delta->adjust(0.0, 1.0);
        }
        if (inputs_pressed[K_DOWN]){
            pos_delta->adjust(0.0, -1.0);
        }
        if (inputs_pressed[K_LEFT]){
            pos_delta->adjust(1.0, 0.0);
        }
        if (inputs_pressed[K_RIGHT]){
            pos_delta->adjust(-1.0, 0.0);
        }
}

void GameEngine::refresh_timing(int* startTicks, float* newTime, float* currentTime, float* frameTime, float* accumulator){
    *startTicks = SDL_GetTicks();
    *newTime = utils::hireTimeInSeconds();
    *frameTime = *newTime - *currentTime;
    *currentTime = *newTime;

    *accumulator += *frameTime;
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
        for (Entity& e : entities){
            e.move(pos_delta);
            window->render(e);
        }
        window->render(*player);
        window->display();
    }
    window->cleanUp();
    SDL_Quit();
    return EXIT_SUCCESS;
}


