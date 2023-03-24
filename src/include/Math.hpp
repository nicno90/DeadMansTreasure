#pragma once
#include <iostream>

struct Vector2f {
    float x, y;
    
    Vector2f():x(0.0f), y(0.0f){}
    Vector2f(float p_x, float p_y):x(p_x), y(p_y){}

    void adjust(float p_x, float p_y);
    void adjust(Vector2f p_v);
    void reset();
    void print(){
        std::cout << x << ", " << y << std::endl;
    }
};
