#include "Math.hpp"

void Vector2f::adjust(float p_x, float p_y){
    x += p_x;
    y += p_y;
}

void Vector2f::adjust(Vector2f p_v){
    x += p_v.x;
    y += p_v.y;
}

void Vector2f::neg_adjust(float p_x, float p_y){
    x -= p_x;
    y -= p_y;
}

void Vector2f::neg_adjust(Vector2f p_v){
    x -= p_v.x;
    y -= p_v.y;
}

Vector2f Vector2f::scale(float mod) {
    return Vector2f(x * mod, y * mod);
}

void Vector2f::reset(){
    x = 0;
    y = 0;
}

