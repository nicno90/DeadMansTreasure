#include "Math.hpp"

void Vector2f::adjust(float p_x, float p_y){
    x += p_x;
    y += p_y;
}

void Vector2f::adjust(Vector2f p_v){
    x += p_v.x;
    y += p_v.y;
}
