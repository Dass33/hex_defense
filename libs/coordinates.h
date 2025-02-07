#pragma once
#include <cstddef>

struct Coordinates{
    Coordinates(size_t x = 0, size_t y = 0) :x(x),y(y){}
    size_t x;
    size_t y;
};

