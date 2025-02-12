#include "../libs/level.h"
#include "../libs/game_state.h"
#include "../libs/moving_objects.h"
#include "../libs/turrets.h"
#include "../libs/win_data.h"
#include <chrono>
#include <cstddef>
#include <fstream>
#include <ncurses.h>
#include <string>
#include <thread>

#define curr_round game_state.curr_round
#define rounds_count game_state.rounds_count

struct Player_state {
    std::string icon;
    Player_modes mode;
    attr_t attributes;
};

void place_tower(Level& level, Coordinates& pos, Game_state& game_state, Player_state player) {
    switch (player.mode) {
        case basic: break;
        case anti_hex:
            if (game_state.money < ANTI_HEX_COST) {
                //todo
            } else {
                Turrets* anti_hex = new Anti_hex(pos, level.road);
                game_state.turrets.push_back(anti_hex);
                game_state.money -= ANTI_HEX_COST;
            }
            break;
        case fire_wall:
            if (game_state.money < FIRE_WALL_COST) {
                //todo
            } else {
                Turrets* fire_wall = new FireWall(pos);
                game_state.turrets.push_back(fire_wall);
                game_state.money -= FIRE_WALL_COST;
            }
            break;
        case blue_teamer:
            if (game_state.money < BLUE_TEAMER_COST) {
                //todo
            } else {
                Turrets* blue_teamer = new Blue_teamer(pos, level.road);
                game_state.turrets.push_back(blue_teamer);
                game_state.money -= BLUE_TEAMER_COST;
            }
            break;
    }
}

void player_actions(Coordinates& pos, WINDOW* win, Level& level,
                    Player_state& player, Game_state& game_state) {
    int input = wgetch(win);

    switch (input) {
        case ERR: return;
        case KEY_UP:
            pos.y--;
            if (pos.y < 1) pos.y = level.winHeight -2;
        break;
        case KEY_DOWN:
            pos.y++;
            if (pos.y > level.winHeight -2) pos.y = 1;
        break;
        case KEY_RIGHT:
            pos.x++;
            if (pos.x > level.winWidth -2) pos.x = 1;
            else if (player.mode != basic && pos.x > level.winWidth -4)
                pos.x = 1;
        break;
        case KEY_LEFT:
            pos.x--;
            if (pos.x < 1) {
                if (player.mode != basic) pos.x = level.winWidth -4;
                else pos.x = level.winWidth -2;
            }
        break;
        case 'h':
            player.icon.assign(PLAYER_ICON);
            player.mode = basic;
            break;
        case 'l':
            player.icon.assign(TOWERS_ICONS[0]);
            player.mode = anti_hex;
        break;
        case 'k':
            player.icon.assign(TOWERS_ICONS[1]);
            player.mode = fire_wall;
        break;
        case 'j':
            player.icon.assign(TOWERS_ICONS[2]);
            player.mode = blue_teamer;
        break;
        //> 10 == KEY_ENTER
        case 10:
            if (!level.clear_n_tiles(pos, TOWERS_SIZE)
                || game_state.turret_collides(pos)) break;
            place_tower(level, pos, game_state, player);
        default: break;
    }
    if (player.mode != basic && (!level.clear_n_tiles(pos, TOWERS_SIZE)
        || game_state.turret_collides(pos))) {
        player.attributes = COLOR_PAIR(1);
    } else player.attributes = 0;
}

void round_loop(Win_data& win_data,Level& level, Game_state& game_state, Coordinates& pos) {
    nodelay(win_data.win, true);
    size_t max_enemies = game_state.mv_objects.vec.size();
    size_t curr_enemies = 0;
    Player_state player;
    player.mode = basic;
    player.attributes = 0;
    player.icon = PLAYER_ICON;

    auto last_enemy_move = std::chrono::steady_clock::now();
    std::chrono::duration<double>enemy_interval(ENEMY_INTERVAL);
    while (game_state.mv_objects.enemies_left) {
        player_actions(pos, win_data.win, level, player, game_state);
        level.print_level(win_data.win);
        mvprintw(1, 0, "Enemies left: %02ld", game_state.mv_objects.enemies_left);
        mvprintw(2, 0, "hp: %03ld", game_state.curr_hp);
        mvprintw(3, 0, "$ %5ld ", game_state.money);

        for (auto& turret : game_state.turrets) {
            game_state.money += turret->attack(game_state.mv_objects);
            turret->print(win_data.win);
        }

        auto now = std::chrono::steady_clock::now();
        if (now -last_enemy_move > enemy_interval) {
            last_enemy_move = now;
            if (curr_enemies < max_enemies) curr_enemies++;
            game_state.curr_hp -= game_state.mv_objects.update(curr_enemies, level.road.size() -1);
        }
        game_state.print_road(win_data.win, level.road);
        wattron(win_data.win, player.attributes);
        mvwprintw(win_data.win, pos.y, pos.x, "%s", player.icon.c_str());
        Turrets::print_range(win_data, pos, player.mode);
        wattroff(win_data.win, player.attributes);

        refresh();
        wrefresh(win_data.win);
        //> Sleep to reduce the load on CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_LENGTH_MS));
    }
    nodelay(win_data.win, false);
}

void init_color_pairs() {
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
}

void game_loop(Level& level, Game_state& game_state) {

    int win_y_start = (level.yMax-level.winHeight)/2;
    int win_x_start = (level.xMax-level.winWidth)/2;
    WINDOW *play_win = newwin(level.winHeight, level.winWidth, win_y_start, win_x_start);
    keypad(play_win, true);

    init_color_pairs();
    Coordinates pos(1,1);

    level.print_level(play_win);
    mvwprintw(play_win,1,1, PLAYER_ICON);
    wrefresh(play_win);

    std::ifstream rounds_file(ROUNDS_PATH);

    Win_data win_data = {play_win, level.winHeight, level.winWidth};
    
    for (;curr_round < rounds_count ;curr_round++) {
        mvprintw(0, 0, "Round %ld", curr_round);
        refresh();
        game_state.load_next_round(rounds_file);
        for (auto& turret :game_state.turrets) turret->round_reset();
        round_loop(win_data, level, game_state, pos);
        game_state.mv_objects.vec.clear();
        game_state.money += ROUND_BONUS;
    }

    rounds_file.close();

    wclear(play_win);
    wrefresh(play_win);
}
