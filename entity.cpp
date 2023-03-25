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


// Agent;

void Agent::get_adjusted_rect(Vector2f* direction){
    SDL_Rect* rect = getRect();
}

// Player:


// Floor:

bool Floor::check_collision(SDL_Rect* adj_player){
    
    
    SDL_Rect floorRec = {(int)getPos()->x, (int)getPos()->y, getRect()->w, getRect()->h};

    printf("{%d, %d, %d, %d}\n", adj_player->x, adj_player->y, adj_player->w, adj_player->h);
    printf("{%d, %d, %d, %d}\n\n\n", floorRec.x, floorRec.y, floorRec.w, floorRec.h);


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
    // return ((floorRec.x < adj_player->x && 
    //         adj_player->x + adj_player->w < floorRec.x + floorRec.w) &&
    //         (floorRec.y < adj_player->y && 
    //         adj_player->y + adj_player->h < floorRec.y + floorRec.h));
}

bool Floor::ignore_top_collison(){
    switch (fType) {
        case FT_V_HALL:
            return true;
        case FT_BL_CORNER:
            return true;
        case FT_BR_CORNER:
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
    }
    return false;
}