#include "../libs/moving_objects.h"
#include <cmath>
#include <cstddef>

char Moving_object::get_char() const{
    size_t abs_hp = abs(hp);
    if (abs_hp == 0) return 0;
    if (abs_hp < 10) return  abs_hp + '0';
    return 'A' + abs_hp - 10;
}

size_t Mv_objects::update(const size_t count, const size_t road_end_idx) {
    size_t damage_to_player = 0;
    for (size_t i = 0; i < count; i++) {
        vec[i].road_index += vec[i].dir;
        if (!vec[i].road_index) vec[i].hp = 0;
        if (vec[i].hp == 0) continue;

        if (vec[i].road_index == road_end_idx) {
            damage_to_player += vec[i].hp;
            vec[i].hp = 0;
            enemies_left--;
        }
    }
    return damage_to_player;
}
