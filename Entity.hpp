#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
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
	T_TL_CORNER,
	T_TR_CORNER,
	T_BR_CORNER,
	T_CHEST,

	T_LAST
};


class Entity{
	private:
		Vector2f pos;
		SDL_Rect currentFrame;
		SDL_Rect hitbox;
		SDL_Texture* texture;
		SDL_Surface* text_texture;
		static TTF_Font* font; // = TTF_OpenFont("res/Samson.ttf", 24);
	public:
		Entity() {};
		Entity(Vector2f p_pos, SDL_Texture* p_texture);
		virtual Vector2f* getPos();
		SDL_Rect* getRect();
		SDL_Texture* getTexture();
		//void setTexture(SDL_Texture* p_texture);
		void setText(const char* p_text);
		SDL_Surface* getText() { return text_texture; };
		SDL_Rect getCurrentFrame();
		void move(float px, float py);
		void move(Vector2f p_v);
		static bool inFront(Entity* e1, Entity* e2);
		virtual bool operator <(const Entity& e);

		void adjust_hitbox(int x_mod, int y_mod, int w_mod, int h_mod) {
			hitbox.x += x_mod;
			hitbox.y += y_mod;
			hitbox.w += w_mod;
			hitbox.h += h_mod;
		}
		SDL_Rect* get_hitbox() {
			return &hitbox;
		};
};


class Agent: public Entity{
	private:
		float maxHealth;
		float health;
		int level;
	public:
		Agent(Vector2f p_pos, SDL_Texture* p_texture) : Entity(p_pos, p_texture) {};
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
		Player(SDL_Texture* p_texture): Agent(Vector2f(400, 300), p_texture){

			Agent::adjust_hitbox(10, 5, -20, -5);
		};
		void setTexture(SDL_Texture* p_texture){setTexture(p_texture);};
		int getCoins(){return coins;};
		float getXP(){return xp;};
		void add_coins(int p_coins){coins += p_coins;};
		void add_xp(float p_xp){xp += p_xp;};

		void get_adjusted_rect(Vector2f* direction){Agent::get_adjusted_rect(direction);};

};


enum AttachmentPoint{
	AP_TOP,
	AP_BOTTOM,
	AP_LEFT,
	AP_RIGHT,
	AP_NULL
};

enum PathType {
	PT_AREA,
	PT_UP,
	PT_DOWN,
	PT_LEFT,
	PT_RIGHT,
	PT_END
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
		PathType pType;
		Vector2f calc_position(Floor* parent, AttachmentPoint p_attachSide, AttachmentPoint p_attachPoint);
		Vector2f calc_position(Floor* parent, AttachmentPoint p_attachSide, AttachmentPoint p_attachPoint, int x_mod, int y_mod);
	public:
		Floor() {};
		Floor(Vector2f p_pos, SDL_Texture* p_texture): Entity(p_pos, p_texture), fType(FT_BLANK){
			Entity::adjust_hitbox(0, -20, 0, 0);
		};
		Floor(Vector2f p_pos, SDL_Texture* p_texture, FloorType p_fType, PathType p_pType): Entity(p_pos, p_texture), fType(p_fType), pType(p_pType){
			Entity::adjust_hitbox(0, -20, 0, 0);
		};
		Floor(SDL_Texture* p_texture, FloorType p_fType, PathType p_pType, AttachmentPoint p_attachSide, Floor* parent);
		Floor(SDL_Texture* p_texture, FloorType p_fType, PathType p_pType, AttachmentPoint p_attachSide, AttachmentPoint p_attachPoint, Floor* parent);
		
		void set_fType(FloorType p_fType);
		bool check_collision(SDL_Rect* adj_player);
		bool floor_collision(SDL_Rect* adj_floor);
		
		FloorType getFtype(){return fType;};
		PathType getPtype() { return pType; }
		
		bool ignore_top_collison();
		bool ignore_bottom_collison();
		bool ignore_left_collison();
		bool ignore_right_collison();
		int x_mod();
		int y_mod();
		bool is_vertical();
		bool is_horizontal();
		
};