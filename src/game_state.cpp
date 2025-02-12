#include "../libs/game_state.h"
#include "../libs/level.h"
#include "../libs/coordinates.h"
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <ncurses.h>
#include <vector>

bool select_game_mode(WINDOW* menu, Level& level) {
    int yMax, xMax;
    getmaxyx(menu, yMax, xMax);

    wclear(menu);
    box(menu, 0, 0);
    int input = ' ';

    mvwprintw(menu,0,2,"Game mode");

    long dff_count = sizeof DIFFICULTIES / sizeof(char[10]);
    for (int i = 0; i < dff_count;i++) {
        mvwprintw(menu, i+2,2,"%s", DIFFICULTIES[i]);
    }

    mvwprintw(menu, dff_count+3,2,"Choose difficulty. (1-3)");

    for(;;) {
        mvprintw(yMax-1,1,"Last input: %c ", input);
        input = wgetch(menu);
        if (input == 'q') return false;
        if (input - '0' <= dff_count && input - '0' >= 1) break;
    }
    clear();

    int difficulty = input - '0';
    Game_state init_game_state(difficulty);
    if (!init_game_state.validate_rounds()) {
        wattron(menu, COLOR_PAIR(1));
        mvwprintw(menu, yMax -1, 1, "Not enough valid rounds.");
        wattroff(menu, COLOR_PAIR(1));
        wrefresh(menu);
        wgetch(menu);
        return false;
    }
    game_loop(level, init_game_state);
    return true;
}

size_t Game_state::from_hex(char c) const{
    if (isdigit(c)) return c - '0';
    if (c < 'A' || c > 'F') return 0;
    return c - 'A' + 10;
}

void Game_state::load_next_round(std::ifstream& ifs) {
    char enemy;
    size_t count;
    mv_objects.enemies_left = 0;
    ifs >> enemy;
    while (enemy != '*') {
        const size_t hp = from_hex(enemy);
        ifs >> count;
        for (size_t i = 0; i < count; i++) {
            mv_objects.vec.emplace_back(hp);
        }
        if (hp > 0) mv_objects.enemies_left += count;
        ifs >> enemy;
    }
}

bool Game_state::validate_rounds() const {
    std::ifstream ifs(ROUNDS_PATH);
    char enemy;
    size_t count;
    for (size_t i = 0; i < rounds_count; i++) {
        for (;;) {
            if (!(ifs >> enemy)) return false;
            if (enemy == '*') break;
            if (!(ifs >> count)) return false;
            if (!isdigit(enemy) && enemy < 'A' && enemy > 'F')
                return false;
        }
    }
    ifs.close();
    return true;
}

void Game_state::print_road(WINDOW* win, std::vector<Coordinates> road) const {
    for (size_t i = 0; i < mv_objects.vec.size(); i++) {
        size_t enemy_index = mv_objects.vec[i].road_index;
        if (enemy_index < road.size() && enemy_index > 0 && mv_objects.vec[i].get_char() != 0) {
            mvwprintw(win, road[enemy_index].y, road[enemy_index].x, "%c", mv_objects.vec[i].get_char());
        }
        // if (i < alies.vec.size() && alies.vec[i].get_char() != 0) {
        //     mvwprintw(win, road[i].y, road[i].x, "%c", alies.vec[i].get_char());
        // }
    }
}

bool Game_state::turret_collides(Coordinates& pos) {
    for (auto& turret : turrets) {
        if (turret->get_pos().y == pos.y &&
            (turret->get_pos().x == pos.x
            || turret->get_pos().x == pos.x -1
            || turret->get_pos().x == pos.x +1))
            return true;
    }
    return false;
}

Game_state::Game_state(int difficulty, int game_mode)
    :difficulty(difficulty), game_mode(game_mode) {
    curr_hp = BASE_HEALTH / difficulty;
    rounds_count = BASE_ROUNDS * difficulty * difficulty;
    money = STARTING_MONEY / difficulty;
}

Game_state::~Game_state() {
    for (auto* turret: turrets) {
        delete turret;
    }
}
