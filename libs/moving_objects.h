#pragma once

#include <cstddef>
#include <cstdio>
#include <vector>
class Moving_object {
public:
    Moving_object(size_t hp, int direction)
        : hp(hp), direction(direction){}
    void move() const;
    void combine(Moving_object obj);
    void apply_damadge();
    char get_char() const;
private:
    bool check_next_pos() const;
    size_t hp;
    int direction;
    size_t curr_round_damadge;
};

class Enemies {
public:
    void spawn_new_enemy();
    std::vector<Moving_object> vec;
private:
    size_t enemies_left;
};


class Alies {
public:
    void spawn_new_ally(size_t index);
    std::vector<Moving_object> vec;
};
