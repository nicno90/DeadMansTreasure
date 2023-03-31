#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
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
    window.setIcon(IMG_Load("res/images/chest.png"));
    
    // this needs to change when there is a main menue
    Player player = Player(window.loadTexture("res/images/knight.png"));

    GameEngine gameInstance = GameEngine(&window, &player);
    gameInstance.startGame();
    
    window.cleanUp();
    SDL_Quit();
    return EXIT_SUCCESS;
}



