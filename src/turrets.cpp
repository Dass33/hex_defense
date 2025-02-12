#include "../libs/turrets.h"
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ncurses.h>
#include <vector>

Turrets::Turrets(const Coordinates pos){
    this->pos.x = pos.x + 1;
    this->pos.y = pos.y;
}

Coordinates Turrets::get_pos() {
    return pos;
}

static constexpr size_t MODE_TO_RANGE[] = {BT_BASE_RANGE, FW_BASE_RANGE, AH_BASE_RANGE};

void Turrets::print_range(Win_data win_data, Coordinates& pos, size_t mode) {
    if (!mode || mode > sizeof MODE_TO_RANGE / sizeof(size_t)) return;
    wattron(win_data.win, COLOR_PAIR(1));
    if (pos.y + 2 < win_data.height) mvwprintw(win_data.win,pos.y + 1, pos.x + 1, "|");
    if (pos.y - 1 > 0) mvwprintw(win_data.win,pos.y - 1, pos.x + 1, "|");

    size_t i = 2;
    for (; i <= MODE_TO_RANGE[mode -1]; i++) {
        if (pos.x + 2 + i < win_data.width) mvwprintw(win_data.win,pos.y, pos.x + 1 + i, "-");
        if (pos.x + 1 - i > 0) mvwprintw(win_data.win,pos.y, pos.x + 1 - i, "-");
        if (pos.y + i + 1 < win_data.height) mvwprintw(win_data.win,pos.y + i, pos.x + 1, "|");
        if (pos.y - i > 0) mvwprintw(win_data.win,pos.y - i, pos.x + 1, "|");
    }
    if (pos.x + 2 + i < win_data.width) mvwprintw(win_data.win,pos.y, pos.x + 1 + i, "-");
    if (pos.x + 1 - i > 0) mvwprintw(win_data.win,pos.y, pos.x + 1 - i, "-");
    wattroff(win_data.win, COLOR_PAIR(1));
}

bool in_range(Coordinates& pos_turret, const Coordinates& pos_enemy, size_t range) {
    long x_diff = std::abs(static_cast<long>(pos_turret.x + 1) - static_cast<long>(pos_enemy.x));
    long y_diff = std::abs(static_cast<long>(pos_turret.y) - static_cast<long>(pos_enemy.y));
    if (hypot(x_diff -1, y_diff) > static_cast<double>(range)) return false;
    return true;
}

FireWall::FireWall(Coordinates pos) :Turrets(pos){}

size_t FireWall::attack(Enemies &enemies) {
    if (!attack_interval) {
        attack_interval = FW_ATTACK_INTERVAL;
        size_t attack_index = 0;
        while (enemies.vec[attack_index].hp < 1) attack_index++;
        if (attack_index >= enemies.vec.size()) {
            is_attacking = false;
            return 0;
        }
        is_attacking = true;
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
    if (attack_interval < FW_ATTACK_INTERVAL / 3
        && attack_interval > FW_ATTACK_INTERVAL / 5 && is_attacking) {
        wattron(win, COLOR_PAIR(5));
    }
    mvwprintw(win, pos.y, pos.x -1, "%c", TOWERS_ICONS[1][0]);
    mvwprintw(win, pos.y, pos.x + 1, "%c", TOWERS_ICONS[1][2]);
    wattroff(win, COLOR_PAIR(5));
    if (attack_interval < FW_ATTACK_INTERVAL / 5 && is_attacking) {
        wattron(win, COLOR_PAIR(2));
    }
    mvwprintw(win, pos.y, pos.x, "%c", TOWERS_ICONS[1][1]);
    wattroff(win, COLOR_PAIR(2));
}

Blue_teamer::Blue_teamer(Coordinates pos, const std::vector<Coordinates>& road) :Turrets(pos){
    for (long i = road.size() -1; i >= 0; i--) {
        if (in_range(pos, road[i], range)) road_in_range.push_back(i);
    }
}

size_t Blue_teamer::find_enemy(Enemies& enemies) {
    size_t road_idx = 0;
    for (size_t enemy_idx = 0; enemy_idx < enemies.vec.size();enemy_idx++) {
        size_t enemy_road = enemies.vec[enemy_idx].road_index;

        while (road_in_range[road_idx] > enemy_road && road_idx +1 < road_in_range.size()) {
            road_idx++;
        }
        if (enemy_road == road_in_range[road_idx] && enemies.vec[enemy_idx].hp > 0){
            return enemy_idx;
        }
    }
    return enemies.vec.size();
}

size_t Blue_teamer::attack(Enemies &enemies) {
    if (!attack_interval && road_in_range.size()) {
        attack_interval = BT_ATTACK_INTERVAL;
        size_t res = find_enemy(enemies);
        if (res < enemies.vec.size()) {
            is_attacking = true;
            if (enemies.vec[res].hp <= damadge) {
                enemies.vec[res].hp = 0;
                enemies.enemies_left--;
                return damadge + (enemies.vec[res].hp - damadge);
            } else {
                enemies.vec[res].hp -= damadge;
                return damadge;
            }
        } is_attacking = false;
    }
    attack_interval--;
    return 0;
}

void Blue_teamer::upgrade() {}

void Blue_teamer::print(WINDOW* win) const {
    if (attack_interval < BT_ATTACK_INTERVAL / 2
        && attack_interval > BT_ATTACK_INTERVAL / 5 && is_attacking) {
        wattron(win, COLOR_PAIR(5));
    }
    mvwprintw(win, pos.y, pos.x -1, "%c", TOWERS_ICONS[2][0]);
    mvwprintw(win, pos.y, pos.x + 1, "%c", TOWERS_ICONS[2][2]);
    wattroff(win, COLOR_PAIR(5));
    if (attack_interval < BT_ATTACK_INTERVAL / 5 && is_attacking) {
        wattron(win, COLOR_PAIR(2));
    }
    mvwprintw(win, pos.y, pos.x, "%c", TOWERS_ICONS[2][1]);
    wattroff(win, COLOR_PAIR(2));
}

void Blue_teamer::round_reset() {
    attack_idx = 0;
    is_attacking = false;
}

Anti_hex::Anti_hex(Coordinates pos, const std::vector<Coordinates>& road) :Turrets(pos){ 
    for (long i = road.size() -1; i >= 0; i--) {
        if (in_range(pos, road[i], AH_BASE_RANGE)) {
            spawn_index = i;
            return;
        }
    }
}

size_t Anti_hex::attack(Enemies &enemies) {
    if (!attack_interval) {
        attack_interval = FW_ATTACK_INTERVAL;
        is_attacking = true;
        //todo attack
        if (enemies.vec[0].hp) enemies.vec[0].hp -= damadge;
        return damadge;
    }
    attack_interval--;
    return 0;
}

void Anti_hex::upgrade() {}

void Anti_hex::print(WINDOW* win) const {
    if (attack_interval *1.3 < AH_ATTACK_INTERVAL
        && attack_interval > AH_ATTACK_INTERVAL / 3 && is_attacking) {
        wattron(win, COLOR_PAIR(3));
    }
    mvwprintw(win, pos.y, pos.x -1, "%c", TOWERS_ICONS[0][0]);
    wattroff(win, COLOR_PAIR(3));
    if (attack_interval < AH_ATTACK_INTERVAL /3 && attack_interval && is_attacking) {
        wattron(win, COLOR_PAIR(5));
    }
    mvwprintw(win, pos.y, pos.x, "%c", TOWERS_ICONS[0][1]);
    wattroff(win, COLOR_PAIR(5));
    if (attack_interval < AH_ATTACK_INTERVAL / 6 && is_attacking) {
        wattron(win, COLOR_PAIR(2));
    }
    mvwprintw(win, pos.y, pos.x + 1, "%c", TOWERS_ICONS[0][2]);
    wattroff(win, COLOR_PAIR(2));
}
