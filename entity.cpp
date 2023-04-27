#include "Entity.hpp"
#include "Math.hpp"

TTF_Font* Entity::font = TTF_OpenFont("res/Samson.ttf", 24);

Entity::Entity(Vector2f p_pos, SDL_Texture* p_texture)
	:pos(p_pos), texture(p_texture), text_texture(TTF_RenderUTF8_Blended(font, (const char*)this, { 241, 71, 35 })) {
	currentFrame.x = 0;
	currentFrame.y = 0;
	SDL_QueryTexture(texture, NULL, NULL, &currentFrame.w, &currentFrame.h);
	currentFrame.w *= 2;
	currentFrame.h *= 2;
	hitbox.x = p_pos.x;
	hitbox.y = p_pos.y;
	hitbox.w = currentFrame.w;
	hitbox.h = currentFrame.h;
}

Vector2f* Entity::getPos(){
	return &pos;
}

SDL_Rect* Entity::getRect(){
	return &currentFrame;
}

SDL_Texture* Entity::getTexture(){
	return texture;
}

void Entity::setText(const char* p_text) {
	text_texture = TTF_RenderUTF8_Blended(font, p_text, { 241, 71, 35 });
}

SDL_Rect Entity::getCurrentFrame(){
	return currentFrame;
}

void Entity::move(float px, float py){
	pos.adjust(px, py);
	adjust_hitbox((int)px, (int)py, 0, 0);
}

void Entity::move(Vector2f p_v){
	pos.adjust(p_v);
	adjust_hitbox((int)p_v.x, (int)p_v.y, 0, 0);
}


bool Entity::inFront(Entity* e1, Entity* e2){
	return e1->getPos()->x > e2->getPos()->y; 
}

bool Entity::operator < (const Entity& e){
	return e.pos.y < Entity::getPos()->y;
}



// Agent;

void Agent::get_adjusted_rect(Vector2f* direction){
	SDL_Rect* rect = getRect();
}

// Player:


// Floor:

Floor::Floor(SDL_Texture* p_texture, FloorType p_fType, PathType p_pType, AttachmentPoint p_attachSide, Floor* parent): Entity(Vector2f(0.0, 0.0), p_texture), fType(p_fType), pType(p_pType) {
	
	Entity::move(calc_position(parent, p_attachSide, AP_NULL));
}

Floor::Floor(SDL_Texture* p_texture, FloorType p_fType, PathType p_pType, AttachmentPoint p_attachSide, AttachmentPoint p_attachPoint, Floor* parent):Entity(Vector2f(0.0, 0.0), p_texture), fType(p_fType), pType(p_pType) {
	if ((p_attachSide == AP_TOP || p_attachSide == AP_BOTTOM) && (parent->getPtype() == PT_UP || parent->getPtype() == PT_DOWN) && !(p_pType == PT_DOWN || p_pType == PT_UP)) {
		Entity::move(calc_position(parent, p_attachSide, p_attachPoint, 3, 0));
	}
	else {
		Entity::move(calc_position(parent, p_attachSide, p_attachPoint));
	}
	if (p_pType != PT_UP) {
		Entity::adjust_hitbox(0, -20, 0, 0);
	}
}

Vector2f Floor::calc_position(Floor* parent, AttachmentPoint p_attachSide, AttachmentPoint p_attachPoint){
	return calc_position(parent, p_attachSide, p_attachPoint,  0,  0);
}

Vector2f Floor::calc_position(Floor* parent, AttachmentPoint p_attachSide, AttachmentPoint p_attachPoint, int x_mod, int y_mod){
	float x, y;
	Vector2f* parent_pos = parent->getPos();
	SDL_Rect* parent_rect = parent->getRect();
	switch (p_attachSide) {
	case AP_RIGHT:
		x = parent_pos->x + parent_rect->w + x_mod;
		y = parent_pos->y + (parent_rect->h / 2.0) - (getRect()->h / 2) + y_mod;
		
		break;
	case AP_LEFT:
		x = parent_pos->x - getRect()->w + x_mod;
		y = parent_pos->y + (parent_rect->h / 2.0) - (getRect()->h / 2) + y_mod;
		
		break;
	case AP_TOP:
		x = parent_pos->x + (parent_rect->w / 2.0) - (getRect()->w / 2.0) + x_mod;
		y = parent_pos->y - getRect()->h + y_mod;
		
		break;
	case AP_BOTTOM:
		x = parent_pos->x + (parent_rect->w / 2.0) - (getRect()->w / 2.0) + x_mod;
		y = parent_pos->y + parent_rect->h - 20 + y_mod;
		
		break;
	default:
		std::cout << "calc position error" << std::endl;
		printf("side: %d, point: %d\n", p_attachSide, p_attachPoint);
		x = x_mod;
		y = y_mod;
	};
	switch (p_attachPoint) {
	case AP_TOP:
		y = parent_pos->y + y_mod;
		break;
	case AP_BOTTOM:
		y = parent_pos->y + parent_rect->h - getRect()->h + y_mod;
		break;
	case AP_LEFT:
		x = parent_pos->x + x_mod;
		break;
	case AP_RIGHT:
		x = parent_pos->x + parent_rect->w - getRect()->w + x_mod;
		break;
	}
	return Vector2f(x, y);
}

void Floor::set_fType(FloorType p_fType) {
	fType = p_fType;
}

bool Floor::check_collision(SDL_Rect* adj_player){
	

	// printf("Check collision\n");

	if (!(Entity::get_hitbox()->x < adj_player->x) && !(ignore_left_collison() && !(Entity::get_hitbox()->x > adj_player->x + adj_player->w))) {
		return false;
	}
	else if (!(adj_player->x + adj_player->w < Entity::get_hitbox()->x + Entity::get_hitbox()->w) && !(ignore_right_collison() && !(adj_player->x > Entity::get_hitbox()->x + Entity::get_hitbox()->w))){
		return false;
	}
	else if (!(Entity::get_hitbox()->y < adj_player->y) && !(ignore_top_collison() && !(adj_player->y + adj_player->h < Entity::get_hitbox()->y))){
		return false;
	}
	else if (!(adj_player->y + adj_player->h < Entity::get_hitbox()->y + Entity::get_hitbox()->h) && !(ignore_bottom_collison() && !(adj_player->y > Entity::get_hitbox()->y + Entity::get_hitbox()->h))){
		return false;
	}

	return true;
}

bool Floor::floor_collision(SDL_Rect* adj_floor) {
	
	SDL_Rect floorRect = { (int)get_hitbox()->x, (int)get_hitbox()->y, get_hitbox()->w, get_hitbox()->h };
	
	if (SDL_HasIntersection(&floorRect, adj_floor)) {
		if (pType == PT_CORNER_TL || pType == PT_CORNER_TR) {
			printf("");
		}
		return true;
	}
	return false;
}


bool Floor::ignore_top_collison(){
	switch (fType) {
		case FT_V_HALL:
			return true;
		case FT_BL_CORNER:
			return true;
		case FT_BR_CORNER:
			return true;
		case FT_V_HALL_BOTTOM:
			return true;
	}
	return false;
}
bool Floor::ignore_bottom_collison(){
	switch (fType) {
		case FT_V_HALL:
			return true;
		case FT_TL_CORNER:
			return true;
		case FT_TR_CORNER:
			return true;
		case FT_V_HALL_TOP:
			return true;
	}
	return false;
}
bool Floor::ignore_left_collison(){
	switch (fType) {
		case FT_H_HALL:
			return true;
		case FT_BR_CORNER:
			return true;
		case FT_TR_CORNER:
			return true;
		case FT_H_HALL_RIGHT:
			return true;
	}
	return false;
}
bool Floor::ignore_right_collison(){
	switch (fType) {
		case FT_H_HALL:
			return true;
		case FT_BL_CORNER:
			return true;
		case FT_TL_CORNER:
			return true;
		case FT_H_HALL_LEFT:
			return true;
	}
	return false;
}

int Floor::x_mod(){
	switch (fType){
	case FT_V_HALL:
		return 0;
	case FT_V_HALL_BOTTOM:
		return 0;
	case FT_V_HALL_TOP:
		return 0;
	default:
		return 0;
	}
}

int Floor::y_mod(){
	switch (fType){
		
		default:
			return 20;
	}
	return 0;
}

bool Floor::is_vertical(){
	return fType == FT_V_HALL || fType == FT_V_HALL_TOP || fType == FT_V_HALL_BOTTOM;
				
}

bool Floor::is_horizontal(){
	return fType == FT_H_HALL || fType == FT_H_HALL_LEFT || fType == FT_H_HALL_RIGHT;
}