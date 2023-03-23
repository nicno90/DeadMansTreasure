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

Vector2f Entity::getPos(){
    return pos;
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
