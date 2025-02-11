#include "../libs/moving_objects.h"
#include <cstddef>

char Moving_object::get_char() const{
    if (hp == 0) return 0;
    if (hp < 10) return  hp + '0';
    return 'A' + hp - 10;
}

size_t Enemies::update(size_t count, size_t road_end_idx) {
    size_t damadge_to_player = 0;
    for (size_t i = 0; i < count; i++) {
        vec[i].road_index++;
        if (vec[i].hp == 0) continue;

        if (vec[i].curr_round_damadge) {
            vec[i].hp -= vec[i].curr_round_damadge;
            vec[i].curr_round_damadge = 0;
            if (vec[i].hp == 0) {
                enemies_left--;
                continue;
            }
        }
        if (vec[i].road_index == road_end_idx) {
            damadge_to_player += vec[i].hp;
            vec[i].hp = 0;
            enemies_left--;
        }
    }
    return damadge_to_player;
}
