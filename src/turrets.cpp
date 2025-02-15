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

static constexpr size_t MODE_TO_RANGE[] = {BLUE_TEAMER.range, FIRE_WALL.range, ANTI_HEX.range};

void Turrets::print_range(Win_data win_data, const Coordinates& pos, const size_t mode) {
    if (!mode || mode > sizeof MODE_TO_RANGE / sizeof(size_t)) return;
    size_t range = MODE_TO_RANGE[mode -1];
    wattron(win_data.win, COLOR_PAIR(1));
    if (pos.x + 2 + range < win_data.width) mvwprintw(win_data.win,pos.y, pos.x + 2 + range, ".");
    if (pos.x - range > 0) mvwprintw(win_data.win,pos.y, pos.x - range, ".");
    if (pos.y + range + 1 < win_data.height) mvwprintw(win_data.win,pos.y + range, pos.x + 1, ".");
    if (pos.y - range > 0) mvwprintw(win_data.win,pos.y - range, pos.x + 1, ".");
    wattroff(win_data.win, COLOR_PAIR(1));
}

bool in_range(Coordinates& pos_turret, const Coordinates& pos_enemy, size_t range) {
    long x_diff = std::abs(static_cast<long>(pos_turret.x + 1) - static_cast<long>(pos_enemy.x));
    long y_diff = std::abs(static_cast<long>(pos_turret.y) - static_cast<long>(pos_enemy.y));
    if (hypot(x_diff -1, y_diff) > static_cast<double>(range)) return false;
    return true;
}

FireWall::FireWall(Coordinates pos) :Turrets(pos){}

size_t FireWall::attack(Mv_objects &enemies) {
    if (!attack_interval) {
        attack_interval = FIRE_WALL.attack_interval;
        size_t attack_index = 0;
        while (enemies.vec[attack_index].hp < 1) attack_index++;
        if (attack_index >= enemies.vec.size()) {
            is_attacking = false;
            return 0;
        }
        is_attacking = true;
        if (enemies.vec[attack_index].hp - damage <= 0) {
            enemies.vec[attack_index].hp = 0;
            enemies.enemies_left--;
            return damage + (enemies.vec[attack_index].hp - damage);
        } else enemies.vec[attack_index].hp -= damage;
        return damage;
    }
    attack_interval--;
    return 0;
}

bool FireWall::upgrade(Coordinates &pos, size_t &money) {
    //todo
    return false;
}
void FireWall::print(WINDOW* win) const {
    if (attack_interval < FIRE_WALL.attack_interval / 3
        && attack_interval > FIRE_WALL.attack_interval / 5 && is_attacking) {
        wattron(win, COLOR_PAIR(5));
    }
    mvwprintw(win, pos.y, pos.x -1, "%c", TOWERS_ICONS[1][0]);
    mvwprintw(win, pos.y, pos.x + 1, "%c", TOWERS_ICONS[1][2]);
    wattroff(win, COLOR_PAIR(5));
    if (attack_interval < FIRE_WALL.attack_interval / 5 && is_attacking) {
        wattron(win, COLOR_PAIR(2));
    }
    mvwprintw(win, pos.y, pos.x, "%c", TOWERS_ICONS[1][1]);
    wattroff(win, COLOR_PAIR(2));
}

size_t FireWall::get_sell_value() const {
    return (FIRE_WALL.cost + tier * upgrade_cost) * SELL_RECUPERATION;
}


Blue_teamer::Blue_teamer(Coordinates pos, const std::vector<Coordinates>& road) :Turrets(pos){
    for (long i = road.size() -1; i >= 0; i--) {
        if (in_range(pos, road[i], range)) road_in_range.push_back(i);
    }
}

size_t Blue_teamer::find_enemy(Mv_objects& enemies) {
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

size_t Blue_teamer::attack(Mv_objects &enemies) {
    if (!attack_interval && !road_in_range.empty()) {
        attack_interval = BLUE_TEAMER.attack_interval;
        const size_t res = find_enemy(enemies);
        if (res < enemies.vec.size()) {
            is_attacking = true;
            if (enemies.vec[res].hp <= damage) {
                enemies.vec[res].hp = 0;
                enemies.enemies_left--;
                return damage + (enemies.vec[res].hp - damage);
            }
            enemies.vec[res].hp -= damage;
            return damage;
        } is_attacking = false;
    }
    attack_interval--;
    return 0;
}

bool Blue_teamer::upgrade(Coordinates &pos, size_t &money) {
    //todo
    return false;
}

void Blue_teamer::print(WINDOW* win) const {
    if (attack_interval < BLUE_TEAMER.attack_interval / 2
        && attack_interval > BLUE_TEAMER.attack_interval / 5 && is_attacking) {
        wattron(win, COLOR_PAIR(5));
    }
    mvwprintw(win, pos.y, pos.x -1, "%c", TOWERS_ICONS[2][0]);
    mvwprintw(win, pos.y, pos.x + 1, "%c", TOWERS_ICONS[2][2]);
    wattroff(win, COLOR_PAIR(5));
    if (attack_interval < BLUE_TEAMER.attack_interval / 5 && is_attacking) {
        wattron(win, COLOR_PAIR(2));
    }
    mvwprintw(win, pos.y, pos.x, "%c", TOWERS_ICONS[2][1]);
    wattroff(win, COLOR_PAIR(2));
}

void Blue_teamer::round_reset() {
    attack_idx = 0;
    is_attacking = false;
    attack_interval = BLUE_TEAMER.attack_interval;
}

size_t Blue_teamer::get_sell_value() const {
    return (FIRE_WALL.cost + tier * upgrade_cost) * SELL_RECUPERATION;
}

Anti_hex::Anti_hex(Coordinates pos, const std::vector<Coordinates>& road) :Turrets(pos){ 
    for (long i = road.size() -1; i >= 0; i--) {
        if (in_range(pos, road[i], ANTI_HEX.range)) {
            spawn_index = i;
            return;
        }
    }
}

long Anti_hex::spawn_collision(const Mv_objects &mv_objects) const {
    for (size_t idx = mv_objects.vec.size() - 1; mv_objects.vec[idx].dir < 0;idx--) {
        if (mv_objects.vec[idx].road_index == static_cast<size_t>(spawn_index)) return idx;
    }
    return -1;
}

size_t Anti_hex::attack(Mv_objects &mv_objects) {
    if (!attack_interval && spawn_index ) {
        attack_interval = ANTI_HEX.attack_interval;
        is_attacking = true;
        const long collision_idx = spawn_collision(mv_objects);
        if (collision_idx > -1) {
            if (mv_objects.vec[collision_idx].hp - damage > -15)
                mv_objects.vec[collision_idx].hp -= damage + 1;
            else attack_interval = 5; //>wait 5 ticks
            return damage + 1;
        }
        const Moving_object ally(-damage, spawn_index, -1);
        mv_objects.vec.emplace_back(ally);
        mv_objects.allies_count++;
        return damage;
    }
    attack_interval--;
    return 0;
}

bool Anti_hex::upgrade(Coordinates &pos, size_t &money) {
    //todo
    return false;
}

void Anti_hex::print(WINDOW* win) const {
    if (attack_interval *1.3 < ANTI_HEX.attack_interval
        && attack_interval > ANTI_HEX.attack_interval / 3 && is_attacking) {
        wattron(win, COLOR_PAIR(3));
    }
    mvwprintw(win, pos.y, pos.x -1, "%c", TOWERS_ICONS[0][0]);
    wattroff(win, COLOR_PAIR(3));
    if (attack_interval < ANTI_HEX.attack_interval /3 && attack_interval && is_attacking) {
        wattron(win, COLOR_PAIR(5));
    }
    mvwprintw(win, pos.y, pos.x, "%c", TOWERS_ICONS[0][1]);
    wattroff(win, COLOR_PAIR(5));
    if (attack_interval < ANTI_HEX.attack_interval / 6 && is_attacking) {
        wattron(win, COLOR_PAIR(2));
    }
    mvwprintw(win, pos.y, pos.x + 1, "%c", TOWERS_ICONS[0][2]);
    wattroff(win, COLOR_PAIR(2));
}

size_t Anti_hex::get_sell_value() const {
    return (FIRE_WALL.cost + tier * upgrade_cost) * SELL_RECUPERATION;
}

