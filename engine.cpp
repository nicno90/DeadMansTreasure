#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <algorithm>

#include "Engine.hpp"
#include "GameMap.hpp"
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"




GameEngine::GameEngine(RenderWindow* p_window, Player* p_player)
:window(p_window), player(p_player),  viking_chad(Agent(Vector2f(350, 300), textures[T_VIKING])), map(GameMap()) {

	init_textures();
	init_directions();
	map.init_map(&textures[0]);

	viking_chad = Agent(Vector2f(350, 300), textures[T_VIKING]);
	agents = {
		player,
		&viking_chad,
	};

	// put this back when working
	map.generate();
	std::cout << "size2: " << map.getFloors().size() << std::endl;
   
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
	std::cout << "size 3: " << map.getFloors().size() << std::endl;
	// Game Loop:
	while (gameRunning){

		refresh_timing(&startTicks, &newTime, &currentTime, &frameTime, &accumulator);
		
		while (accumulator >= timeStep){
			gameRunning = readInput(&windowEvent);
			accumulator -= timeStep;
		}
		
		check_movement(&pos_delta);
		window->clear();
		for (Floor* f : map.getFloors()){
			// std::cout << "floors-" << f << std::endl;
			f->move(pos_delta);
			window->render(f);
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
	// delete map;
	window->cleanUp();
	SDL_Quit();
	return EXIT_SUCCESS;
}

void GameEngine::init_textures(){
	textures[T_KNIGHT] = player->getTexture();
	textures[T_BIG_OPENING] = window->loadTexture("res/images/big_opening.png");
	textures[T_VERTICAL_PATH] = window->loadTexture("res/images/vertical_path.png");
	textures[T_VERTICAL_UP] = window->loadTexture("res/images/vertical_path_up.png");
	textures[T_VERTICAL_DOWN] = window->loadTexture("res/images/vertical_path_down.png");
	textures[T_VIKING] = window->loadTexture("res/images/viking.png");
	textures[T_CHEST] = window->loadTexture("res/images/chest.png");
	textures[T_HORIZONTAL_PATH] = window->loadTexture("res/images/horizontal_path.png");
	textures[T_HORIZONTAL_LEFT] = window->loadTexture("res/images/horizontal_path_left.png");
	textures[T_HORIZONTAL_RIGHT] = window->loadTexture("res/images/horizontal_path_right.png");
	textures[T_BL_CORNER] = window->loadTexture("res/images/bl_corner.png");
	textures[T_TL_CORNER] = window->loadTexture("res/images/tl_corner.png");
	textures[T_BR_CORNER] = window->loadTexture("res/images/br_corner.png");
	textures[T_TR_CORNER] = window->loadTexture("res/images/tr_corner.png");
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
			case SDLK_LSHIFT:
				setKey(K_SHIFT, windowEvent);
				break;
			case SDLK_SPACE:
				setKey(K_SPACE, windowEvent);
				break;
			case SDLK_ESCAPE:
				setKey(K_ESC, windowEvent);
				return false;
				break;
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
	float speed = 1.0;
	if (inputs_pressed[K_SHIFT]) {
		speed = 2;
	}
	if (inputs_pressed[K_UP] && (check_valid_move(directions[K_UP]) || inputs_pressed[K_SPACE])) {
		pos_delta->adjust(directions[K_UP].scale(speed));
	}
	if (inputs_pressed[K_DOWN] && (check_valid_move(directions[K_DOWN]) || inputs_pressed[K_SPACE])){
		pos_delta->adjust(directions[K_DOWN].scale(speed));
	}
	if (inputs_pressed[K_LEFT] && (check_valid_move(directions[K_LEFT]) || inputs_pressed[K_SPACE])){
		pos_delta->adjust(directions[K_LEFT].scale(speed));
	}
	if (inputs_pressed[K_RIGHT] && (check_valid_move(directions[K_RIGHT]) || inputs_pressed[K_SPACE])){
		pos_delta->adjust(directions[K_RIGHT].scale(speed));
	}
	if (inputs_pressed[K_ESC]) {
		// map.add_path();
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
		(int)(player->get_hitbox()->x - (direction.x) + 8), (int)(player->get_hitbox()->y - (direction.y)),
		player->get_hitbox()->w, player->get_hitbox()->h
	};
	for (Floor* f : map.getFloors())
	{
		if (f->check_collision(&adjustedRect)){
			//printf("in floor: {%d, %d, %d, %d}\n", f->get_hitbox()->x, f->get_hitbox()->y, f->get_hitbox()->w, f->get_hitbox()->h);
			return true;
		}

	}
	return false;
}

