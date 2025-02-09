#pragma once

#include <cstddef>
#include <cstdio>
#include <vector>

class Moving_object {
public:
    Moving_object(size_t hp, size_t road_index = 0)
        : hp(hp), road_index(road_index){}
    void combine(Moving_object obj);
    void apply_damadge();
    char get_char() const;
    size_t hp;
    size_t road_index;
    size_t curr_round_damadge = 0;
private:
    bool check_next_pos() const;
};

class Enemies {
public:
    void spawn_new_enemy();
    std::vector<Moving_object> vec;
    size_t update(size_t count, size_t road_end_idx);
    size_t enemies_left = 0;
};


class Alies {
public:
    void spawn_new_ally(size_t index);
    void update();
    std::vector<Moving_object> vec;
};
