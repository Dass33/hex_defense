#include "../libs/turrets.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <vector>

Turrets::Turrets(const Coordinates pos){
    this->pos.x = pos.x + 1;
    this->pos.y = pos.y;
}

Coordinates Turrets::get_pos() {
    return pos;
}

bool in_range(Coordinates& pos_turret, Coordinates& pos_enemy, size_t range) {
    long x_diff = std::abs(static_cast<long>(pos_turret.x) - static_cast<long>(pos_enemy.x));
    long y_diff = std::abs(static_cast<long>(pos_turret.y) - static_cast<long>(pos_enemy.y));
    if (hypot(x_diff, y_diff) > static_cast<double>(range)) return false;
    return true;
}

FireWall::FireWall(Coordinates pos) :Turrets(pos){}

size_t FireWall::attack(Enemies &enemies) {
    if (!attack_interval) {
        attack_interval = FW_ATTACK_INTERVAL;
        size_t attack_index = 0;
        while (enemies.vec[attack_index].hp < 1) attack_index++;
        if (attack_index >= enemies.vec.size()) return 0;
        if (enemies.vec[attack_index].hp - damadge <= 0) {
            enemies.vec[attack_index].hp = 0;
            enemies.enemies_left--;
            return damadge + (enemies.vec[attack_index].hp - damadge);
        } else enemies.vec[attack_index].hp -= damadge;
        return damadge;
    }
    attack_interval--;
    return 0;
}

void FireWall::upgrade() {}

void FireWall::print(WINDOW* win) const {
    mvwprintw(win, pos.y, pos.x -1, "%s", TOWERS_ICONS[1]);
}

Blue_teamer::Blue_teamer(Coordinates pos, std::vector<Coordinates>road) :Turrets(pos){
    for (long i = road.size() -1; i >= 0; i--) {
        if (in_range(pos, road[i], range)) road_in_range.push_back(i);
    }
}

size_t Blue_teamer::attack(Enemies &enemies) {
    if (!attack_interval) {
        attack_interval = BT_ATTACK_INTERVAL;
        for (size_t i = 0; i < road_in_range.size(); i++) {
            auto res = std::lower_bound(enemies.vec.rbegin(), enemies.vec.rend(), road_in_range[i],
            [](auto& enemy, auto& index) {
                return enemy.road_index > index;
            });
            if (res->road_index == road_in_range[i]) {
                if (enemies.vec[i].hp - damadge <= 0) {
                    enemies.vec[i].hp = 0;
                    enemies.enemies_left--;
                    return damadge + (enemies.vec[i].hp - damadge);
                } else enemies.vec[i].hp -= damadge;
                return damadge;
            }
            if (res->road_index < road_in_range.back()) return 0;
        }
    }
    attack_interval--;
    return 0;
}

void Blue_teamer::upgrade() {}

void Blue_teamer::print(WINDOW* win) const {
    mvwprintw(win, pos.y, pos.x -1, "%s", TOWERS_ICONS[2]);
}

void Blue_teamer::round_reset() {
    enemy_attack_index = 0;
}

Anti_hex::Anti_hex(Coordinates pos) :Turrets(pos){}

size_t Anti_hex::attack(Enemies &enemies) {
    if (!attack_interval) {
        attack_interval = FW_ATTACK_INTERVAL;
        //todo attack
        enemies.vec[0].hp -= damadge;
        return damadge;
    }
    attack_interval--;
    return 0;
}

void Anti_hex::upgrade() {}

void Anti_hex::print(WINDOW* win) const {
    mvwprintw(win, pos.y, pos.x -1, "%s", TOWERS_ICONS[0]);
}
