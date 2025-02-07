#pragma once

#include <cstddef>
class Moving_object {
public:
    void move() const;
    void combine(Moving_object obj);
    void apply_damadge();
private:
    bool check_next_pos() const;
    size_t hp;
    size_t direction;
    size_t curr_round_damadge;
};
