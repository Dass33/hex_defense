#include "../libs/level.h"
#include "../libs/game_state.h"
#include "../libs/moving_objects.h"
#include "../libs/turrets.h"
#include <chrono>
#include <cstddef>
#include <fstream>
#include <ncurses.h>
#include <string>
#include <thread>
//#include <chrono>

#define curr_round game_state.curr_round
#define rounds_count game_state.rounds_count

struct Player_state {
    std::string icon;
    Player_modes mode;
    attr_t attributes;
};

void player_actions(Coordinates& pos, WINDOW* win, Level& level,
                    Player_state& player, Game_state& game_state) {
    int input = wgetch(win);
    const int towers_size = sizeof TOWERS_ICONS[0] / sizeof(char) -1;

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
        case 'j':
            player.icon.assign(TOWERS_ICONS[0]);
            player.mode = anti_hex;
        break;
        case 'k':
            player.icon.assign(TOWERS_ICONS[1]);
            player.mode = fire_wall;
        break;
        case 'l':
            player.icon.assign(TOWERS_ICONS[2]);
            player.mode = blue_teamer;
        break;
        //> 10 == KEY_ENTER
        case 10:
            if (!level.clear_n_tiles(pos, towers_size)
                || game_state.turret_collides(pos)) break;
            switch (player.mode) {
                case basic: break;
                case anti_hex:
                    if (game_state.money < ANTI_HEX_COST) {
                        //todo
                    } else {
                        Turrets* anti_hex = new Anti_hex(pos);
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
                        Turrets* blue_teamer = new Blue_teamer(pos);
                        game_state.turrets.push_back(blue_teamer);
                        game_state.money -= BLUE_TEAMER_COST;
                    }
                    break;
            }
        default: break;
    }
    if (player.mode != basic && (!level.clear_n_tiles(pos, towers_size)
        || game_state.turret_collides(pos))) {
        player.attributes = COLOR_PAIR(1);
    } else player.attributes = 0;
}

void round_loop(WINDOW* play_win,Level& level, Game_state& game_state, Coordinates& pos) {
    nodelay(play_win, true);
    size_t max_enemies = game_state.enemies.vec.size();
    size_t curr_enemies = 0;
    Player_state player;
    player.mode = basic;
    player.attributes = 0;
    player.icon = PLAYER_ICON;

    auto last_enemy_move = std::chrono::steady_clock::now();
    std::chrono::duration<double>enemy_interval(0.1);
    while (game_state.enemies.enemies_left) {
        player_actions(pos, play_win, level, player, game_state);
        level.print_level(play_win);
        mvprintw(1, 0, "Enemies left: %02ld", game_state.enemies.enemies_left);
        mvprintw(2, 0, "hp: %03ld", game_state.curr_hp);
        mvprintw(3, 0, "$ %5ld ", game_state.money);

        for (auto& turret : game_state.turrets) {
            turret->print(play_win);
            turret->attack();
        }

        auto now = std::chrono::steady_clock::now();
        if (now -last_enemy_move > enemy_interval) {
            last_enemy_move = now;
            if (curr_enemies < max_enemies) curr_enemies++;
            game_state.curr_hp -= game_state.enemies.update(curr_enemies, level.road.size() -1);
        }
        game_state.print_road(play_win, level.road);
        wattron(play_win, player.attributes);
        mvwprintw(play_win, pos.y, pos.x, "%s", player.icon.c_str());
        wattroff(play_win, player.attributes);

        refresh();
        wrefresh(play_win);
        //> Sleep for 10 milliseconds to reduce the load on CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    nodelay(play_win, false);
}

void game_loop(Level& level, Game_state& game_state) {

    int win_y_start = (level.yMax-level.winHeight)/2;
    int win_x_start = (level.xMax-level.winWidth)/2;
    WINDOW *play_win = newwin(level.winHeight, level.winWidth, win_y_start, win_x_start);
    keypad(play_win, true);

    Coordinates pos(1,1);

    level.print_level(play_win);
    mvwprintw(play_win,1,1, PLAYER_ICON);
    wrefresh(play_win);

    std::ifstream rounds_file(ROUNDS_PATH);
    
    for (;curr_round < rounds_count ;curr_round++) {
        mvprintw(0, 0, "Round %ld", curr_round);
        refresh();
        game_state.load_next_round(rounds_file);
        round_loop(play_win, level, game_state, pos);
        game_state.enemies.vec.clear();
    }

    rounds_file.close();

    wclear(play_win);
    wrefresh(play_win);
}
