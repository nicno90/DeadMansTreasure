#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"

const int WIDTH = 800, HEIGHT = 600;



int main(int argc, char *argv[]){
    

    SDL_Init(SDL_INIT_EVERYTHING);

    if (!IMG_Init(IMG_INIT_PNG))
        std::cout << "IMG_Init failed. ERROR: " << SDL_GetError() << std::endl;

    RenderWindow window ("Dead Man's Treasure", WIDTH, HEIGHT);

    SDL_Texture* knight = window.loadTexture("res/images/knight.png");
    SDL_Texture* big_opening = window.loadTexture("res/images/big_opening.png");
    SDL_Texture* viking = window.loadTexture("res/images/viking.png");


    Entity main_character = Entity(Vector2f(400, 300), knight);
    std::vector<Entity> entities = {
        Entity(Vector2f(350, 250), big_opening),
        Entity(Vector2f(350, 250), viking),
    };

    bool gameRunning = true;
    SDL_Event windowEvent;
    const float timeStep = 0.01f;
    float accumulator = 0.0f;
    float currentTime =  utils::hireTimeInSeconds();

    bool movement_directions[4] = {false}; // up, down, left, right
    float zoom = 0.0;

    while (gameRunning){

        int startTicks = SDL_GetTicks();
        float newTime = utils::hireTimeInSeconds();
        float frameTime = newTime - currentTime;
        float currentTime = newTime;

        accumulator += frameTime;
        while (accumulator >= timeStep){
            while (SDL_PollEvent(&windowEvent)){
                if (SDL_QUIT == windowEvent.type){
                    gameRunning = false;
                    break;
                }
                
                else if (SDL_KEYDOWN == windowEvent.type){
                    switch (windowEvent.key.keysym.sym) {
                        case SDLK_UP:
                            movement_directions[0] = true;
                            break;
                        case SDLK_DOWN:
                            movement_directions[1] = true;
                            break;
                        case SDLK_LEFT:
                            movement_directions[2] = true;
                            break;
                        case SDLK_RIGHT:
                            movement_directions[3] = true;
                            break;
                    }
                }
                else if (SDL_KEYUP == windowEvent.type){
                    switch (windowEvent.key.keysym.sym) {
                        case SDLK_UP:
                            movement_directions[0] = false;
                            break;
                        case SDLK_DOWN:
                            movement_directions[1] = false;
                            break;
                        case SDLK_LEFT:
                            movement_directions[2] = false;
                            break;
                        case SDLK_RIGHT:
                            movement_directions[3] = false;
                            break;
                    }
                }
            }
            accumulator -= timeStep;
        }
        const float alpha = accumulator / timeStep;
        
        Vector2f pos_delta = Vector2f();

        // up
        if (movement_directions[0]){
            pos_delta.adjust(0.0, 1.0);
        }
        // down
        if (movement_directions[1]){
            pos_delta.adjust(0.0, -1.0);
        }
        // left
        if (movement_directions[2]){
            pos_delta.adjust(1.0, 0.0);
        }
        // right
        if (movement_directions[3]){
            pos_delta.adjust(-1.0, 0.0);
        }



        window.clear();
        for (Entity& e : entities){
            e.move(pos_delta);
            window.render(e);
        }
        window.render(main_character);
        window.display();
    }
    window.cleanUp();
    SDL_Quit();
    return EXIT_SUCCESS;
}



