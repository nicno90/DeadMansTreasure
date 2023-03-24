#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "Engine.hpp"
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"

const int WIDTH = 800, HEIGHT = 600;



int main(int argc, char *argv[]){

    SDL_Init(SDL_INIT_EVERYTHING);

    if (!IMG_Init(IMG_INIT_PNG))
        std::cout << "IMG_Init failed. ERROR: " << SDL_GetError() << std::endl;
    RenderWindow window = RenderWindow("Dead Man's Treasure", WIDTH, HEIGHT);
    
    SDL_Texture* knight = window.loadTexture("res/images/knight.png");
    Entity player = Entity(Vector2f(400, 300), knight);
    GameEngine gameInstance = GameEngine(&window, &player);
    gameInstance.startGame();
    
    window.cleanUp();
    SDL_Quit();
    return EXIT_SUCCESS;
}



