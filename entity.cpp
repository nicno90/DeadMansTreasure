#include "Entity.hpp"
#include "Math.hpp"

Entity::Entity(Vector2f p_pos, SDL_Texture* p_texture)
:pos(p_pos), texture(p_texture){
    currentFrame.x = 0;
    currentFrame.y = 0;
    SDL_QueryTexture(texture, NULL, NULL, &currentFrame.w, &currentFrame.h);
    currentFrame.w *= 1.75;
    currentFrame.h *= 1.75;
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

SDL_Rect Entity::getCurrentFrame(){
    return currentFrame;
}

void Entity::move(float px, float py){
    pos.adjust(px, py);
}

void Entity::move(Vector2f p_v){
    pos.adjust(p_v);
}


bool Entity::inFront(Entity* e1, Entity* e2){
    return e1->getPos()->x > e2->getPos()->y; 
}

bool Entity::operator < (Entity& e){
    return e.getPos()->y > Entity::getPos()->y;
}

// Agent;

void Agent::get_adjusted_rect(Vector2f* direction){
    SDL_Rect* rect = getRect();
}

// Player:


// Floor:

Floor::Floor(SDL_Texture* p_texture, FloorType p_fType, AttachmentPoint p_attachSide, Floor* parent): Entity(Vector2f(0.0, 0.0), p_texture), fType(p_fType){
    
    Entity::move(calc_position(parent, p_attachSide, AP_NULL));
}

Floor::Floor(SDL_Texture* p_texture, FloorType p_fType, AttachmentPoint p_attachSide, AttachmentPoint p_attachPoint, Floor* parent):Entity(Vector2f(0.0, 0.0), p_texture), fType(p_fType){
    Entity::move(calc_position(parent, p_attachSide, p_attachPoint));
}

Vector2f Floor::calc_position(Floor* parent, AttachmentPoint p_attachSide, AttachmentPoint p_attachPoint){
    float x, y;
    Vector2f* parent_pos = parent->getPos();
    SDL_Rect* parent_rect = parent->getRect();
    switch (p_attachSide){
        case AP_RIGHT:
            x = parent_pos->x + parent_rect->w;
            y = parent_pos->y + (parent_rect->h / 2.0) - (getRect()->h / 2);
            if (parent->is_vertical()){
                x-= 2;
            }
            break;
        case AP_LEFT:
            x = parent_pos->x - getRect()->w;
            y = parent_pos->y + (parent_rect->h / 2.0) - (getRect()->h / 2);
            if (parent->is_vertical()){
                x += 5;
            }
            break;
        case AP_TOP:
            x = parent_pos-> x + (parent_rect->w / 2.0) - (getRect()->w / 2.0);
            y = parent_pos-> y - getRect()->h + 20;
            if (is_vertical() && !parent->is_vertical()){
                x -= 4;
            }
            break;
        case AP_BOTTOM:
            x = parent_pos-> x + (parent_rect->w / 2.0) - (getRect()->w / 2.0);
            y = parent_pos-> y + parent_rect->h - 20;
            if (is_vertical() && !parent->is_vertical()){
                x -= 4;
            }
            break;
        default:
            x = 0.0;
            y = 0.0;
    };
    switch (p_attachPoint){
        case AP_TOP:
            y = parent_pos->y; 
        case AP_BOTTOM:
            y = parent_pos->y+ parent_rect->h - getRect()->h;
            break; 
        case AP_LEFT:
            break;
        case AP_RIGHT:
            break;
    }
    return Vector2f(x, y);
}

bool Floor::check_collision(SDL_Rect* adj_player){
    
    
    SDL_Rect floorRec = {(int)getPos()->x + 3, (int)getPos()->y - 20, getRect()->w, getRect()->h};

    // printf("Check collision\n");

    if (!(floorRec.x < adj_player->x) && !(ignore_left_collison() && !(floorRec.x > adj_player->x + adj_player->w))){
        return false;
    }
    else if (!(adj_player->x + adj_player->w < floorRec.x + floorRec.w) && !(ignore_right_collison() && !(adj_player->x > floorRec.x + floorRec.w))){
        return false;
    }
    else if (!(floorRec.y < adj_player->y) && !(ignore_top_collison() && !(adj_player->y + adj_player->h < floorRec.y))){
        return false;
    }
    else if (!(adj_player->y + adj_player->h < floorRec.y + floorRec.h) && !(ignore_bottom_collison() && !(adj_player->y > floorRec.y + floorRec.h))){
        return false;
    }

    return true;
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
        default:
            return 3;
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