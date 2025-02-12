#pragma once
#include <cstddef>
#include <ncurses.h>

struct Win_data{
    WINDOW* win;
    size_t height;
    size_t width;
};
