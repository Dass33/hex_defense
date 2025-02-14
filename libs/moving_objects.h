#pragma once

#include <cstddef>
#include <cstdio>
#include <vector>

class Moving_object {
public:
    explicit Moving_object(const int hp, const size_t road_index = 0, const int dir = 1)
        : hp(hp), road_index(road_index), dir(dir){}
    void combine(Moving_object obj);
    [[nodiscard]] char get_char() const;
    int hp;
    size_t road_index;
    int dir;
};

class Mv_objects {
public:
    std::vector<Moving_object> vec;
    size_t update(size_t count, size_t road_end_idx);
    size_t enemies_left = 0;
    size_t allies_count = 0;
};