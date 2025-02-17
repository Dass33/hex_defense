#include "../libs/level.h"
#include "../libs/game_state.h"
#include "../libs/moving_objects.h"
#include "../libs/turrets.h"
#include "../libs/win_data.h"
#include <chrono>
#include <cstddef>
#include <ncurses.h>
#include <thread>

#define curr_round game_state.curr_round
#define rounds_count game_state.rounds_count
static constexpr attr_t RED = COLOR_PAIR(1);
static constexpr attr_t GREEN = COLOR_PAIR(2);
static constexpr attr_t YELLOW = COLOR_PAIR(3);
static constexpr attr_t BLUE = COLOR_PAIR(4);
static constexpr attr_t MAGENTA = COLOR_PAIR(5);
static constexpr attr_t BASE_PLAYR_COLOR = BLUE;

struct Player_state {
    std::string icon;
    Player_modes mode;
    attr_t attributes;
};

void pause_game(Win_data* win_data, bool &paused) {
            nodelay(win_data->win, false);
            const char pause_str[] = "PAUSED";
            const int x_start = win_data->width/2 -sizeof pause_str;
            wattron(win_data->win, A_REVERSE | A_BOLD);
            mvwprintw(win_data->win, win_data->height / 2, x_start,"%s", pause_str);
            wattroff(win_data->win, A_REVERSE | A_BOLD);
            wrefresh(win_data->win);
            wgetch(win_data->win);
            nodelay(win_data->win, true);
            paused = !paused;
}

void place_tower(Level& level, Coordinates& pos, Game_state& game_state, Player_state& player) {
    switch (player.mode) {
        case basic: break;
        case anti_hex:
            if (game_state.money >= ANTI_HEX.cost) {
                Turrets* anti_hex = new Anti_hex(pos, level.road);
                game_state.turrets.push_back(anti_hex);
                game_state.money -= ANTI_HEX.cost;
            }
            break;
        case fire_wall:
            if (game_state.money >= FIRE_WALL.cost){
                Turrets* fire_wall = new FireWall(pos);
                game_state.turrets.push_back(fire_wall);
                game_state.money -= FIRE_WALL.cost;
            }
            break;
        case blue_teamer:
            if (game_state.money >= BLUE_TEAMER.cost){
                Turrets* blue_teamer = new Blue_teamer(pos, level.road);
                game_state.turrets.push_back(blue_teamer);
                game_state.money -= BLUE_TEAMER.cost;
            }
            break;
    }
}

bool player_actions(Coordinates& pos, Win_data& win_data, Level& level,
                    Player_state& player, Game_state& game_state) {
    int input = wgetch(win_data.win);
    Turrets** turret_under_player = game_state.turret_collides(pos, true);

    switch (input) {
        case ERR: return true;
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
        case 's': return false;
        case 'f':
            game_state.change_speed();
            break;
        case 'd': if (player.mode != basic || !turret_under_player) break;
            game_state.money += game_state.sell_turret(turret_under_player);
            break;
        case 'u': if (player.mode != basic || !turret_under_player) break;
            if (!(*turret_under_player)->upgrade(pos, game_state.money, level.road)) {
                player.attributes = RED;
                return true;
            }
            break;
        case 'p': game_state.paused = !game_state.paused;
            break;
        case  'q': game_state.quit = true;
            break;
        case 10: //> 10 == KEY_ENTER
            if (!level.clear_n_tiles(pos, TOWERS_SIZE)
                || game_state.turret_collides(pos, false)) break;
            place_tower(level, pos, game_state, player);
        default: break;
    }
    turret_under_player = game_state.turret_collides(pos, true);

    const char upgrade_str[] = "Press 'u' to upgrade.";
    const char sell_str[] = "|| 'd' to sell:";
    const int x_pos = win_data.x_start + win_data.width - sizeof upgrade_str - 3;
    mvprintw(win_data.y_start - 1, x_pos, "                         ");
    mvprintw(win_data.y_start - 2, x_pos, "                         ");

    if (game_state.not_enough_money(player.mode)
        || (player.mode != basic && (!level.clear_n_tiles(pos, TOWERS_SIZE)
        || game_state.turret_collides(pos, false)))) {
        player.attributes = RED;
    }
    else if (player.mode != basic) player.attributes = GREEN;
    else if (turret_under_player) {
        mvprintw(win_data.y_start - 2, x_pos, "%s %2ld", upgrade_str, (*turret_under_player)->get_upgrade_cost());
        mvprintw(win_data.y_start - 1, x_pos, "%s %2ld", sell_str, (*turret_under_player)->get_sell_value());
        player.attributes = YELLOW;
    }
    else player.attributes = BASE_PLAYR_COLOR;
    return true;
}

void round_loop(Win_data& win_data,Level& level, Game_state& game_state, Coordinates& pos) {
    nodelay(win_data.win, true);
    size_t max_enemies = game_state.mv_objects.vec.size();
    size_t curr_enemies = 0;
    Player_state player = {PLAYER_ICON, basic, BASE_PLAYR_COLOR};

    auto last_enemy_move = std::chrono::steady_clock::now();
    std::chrono::duration<double>enemy_interval(ENEMY_INTERVAL);
    while (game_state.mv_objects.enemies_left && !game_state.quit) {
        player_actions(pos, win_data, level, player, game_state);
        level.print_level(win_data.win);
        mvprintw(1, 0, "Enemies left: %02ld", game_state.mv_objects.enemies_left);
        mvprintw(2, 0, "hp: %03d", game_state.curr_hp);
        mvprintw(3, 0, "$ %5ld ", game_state.money);
        mvprintw(1, level.xMax - 10, "Speed: %dx", game_state.fast_forwarding + 1);

        for (auto& turret : game_state.turrets) {
            game_state.money += turret->attack(game_state.mv_objects);
            turret->print(win_data.win);
        }

        auto now = std::chrono::steady_clock::now();
        if (now -last_enemy_move > enemy_interval * game_state.tick_length) {
            last_enemy_move = now;
            if (curr_enemies < max_enemies) curr_enemies++;
            const size_t curr_objects = curr_enemies + game_state.mv_objects.allies_count;
            game_state.curr_hp -= game_state.mv_objects.update(curr_objects, level.road.size() -1);
            if (game_state.curr_hp < 1) {
                mvprintw(2, 0, "hp: %03d", game_state.curr_hp);
                refresh();
                wrefresh(win_data.win);
                break;
            }
        }
        game_state.print_road(win_data.win, level.road);
        wattron(win_data.win, player.attributes);
        mvwprintw(win_data.win, pos.y, pos.x, "%s", player.icon.c_str());
        Turrets::print_range(win_data, pos, player.mode);
        wattroff(win_data.win, player.attributes);

        refresh();
        wrefresh(win_data.win);
        if (game_state.paused) pause_game(&win_data, game_state.paused);
        //> Sleep to reduce the load on CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(game_state.tick_length));
    }
    nodelay(win_data.win, false);
}


void round_preparation(Coordinates& pos, Win_data& win_data, Level& level, Game_state& game_state) {
    nodelay(win_data.win, false);
    Player_state player = {PLAYER_ICON, basic, BASE_PLAYR_COLOR};
    mvprintw(win_data.y_start-1, win_data.x_start+1, "Press 's' to start new round");
    do {
        if (game_state.quit) break;
        mvprintw(1, 0, "Enemies left: %02ld", game_state.mv_objects.enemies_left);
        mvprintw(2, 0, "hp: %03d", game_state.curr_hp);
        mvprintw(3, 0, "$ %5ld ", game_state.money);
        mvprintw(1, getmaxx(stdscr) - 10, "Speed: %dx", game_state.fast_forwarding + 1);
        level.print_level(win_data.win);

        for (auto& turret : game_state.turrets) turret->print(win_data.win);

        wattron(win_data.win, player.attributes);
        mvwprintw(win_data.win, pos.y, pos.x, "%s", player.icon.c_str());
        Turrets::print_range(win_data, pos, player.mode);
        wattroff(win_data.win, player.attributes);
        refresh();
        wrefresh(win_data.win);
    } while (player_actions(pos, win_data, level, player, game_state));
    mvprintw(win_data.y_start-1, win_data.x_start+1, "                            ");
}

void game_over_screen(size_t death_round, size_t final_round) {
    Win_data end_win;
    end_win.height = 12;
    end_win.width = 54;

    int yMax,xMax;
    getmaxyx(stdscr, yMax, xMax);
    const int win_y_start = yMax/2 -end_win.height /2;
    const int win_x_start = xMax/2-end_win.width/2;
    end_win.win = newwin(end_win.height, end_win.width, win_y_start, win_x_start);

    box(end_win.win,0,0);
    int xStart = 1;
    int yStart = 1;
    mvwprintw(end_win.win,yStart, xStart, "%s", " _____                         _____");
    mvwprintw(end_win.win,yStart+1, xStart, "%s", "|  __ \\                       |  _  |");
    mvwprintw(end_win.win,yStart+2, xStart, "%s", "| |  \\/ __ _ _ __ ___   ___   | | | |_   _____ _ __ ");
    mvwprintw(end_win.win,yStart+3, xStart, "%s", "| | __ / _` | '_ ` _ \\ / _ \\  | | | \\ \\ / / _ \\ '__|");
    mvwprintw(end_win.win,yStart+4, xStart, "%s", "| |_\\ \\ (_| | | | | | |  __/  \\ \\_/ /\\ V /  __/ |");
    mvwprintw(end_win.win,yStart+5,xStart, "%s", " \\____/\\__,_|_| |_| |_|\\___|   \\___/  \\_/ \\___|_|");
    mvwprintw(end_win.win,yStart+7,xStart, "                       %ld / %ld", death_round, final_round);
    wattron(end_win.win,A_BLINK);
    mvwprintw(end_win.win,yStart+8,xStart, "            Press any button to continue");
    wattroff(end_win.win,A_BLINK);
    wrefresh(end_win.win);
}

void win_screen() {
    Win_data end_win;
    end_win.height = 12;
    end_win.width = 54;

    int yMax,xMax;
    getmaxyx(stdscr, yMax, xMax);
    int win_y_start = yMax/2 -end_win.height /2;
    int win_x_start = xMax/2-end_win.width/2;
    end_win.win = newwin(end_win.height, end_win.width, win_y_start, win_x_start);

    box(end_win.win,0,0);
    int xStart = 5;
    int yStart = 2;
    mvwprintw(end_win.win,yStart, xStart, "%s", "__   __             _    _               _ ");
    mvwprintw(end_win.win,yStart+1, xStart, "%s", "\\ \\ / /            | |  | |             | |");
    mvwprintw(end_win.win,yStart+2, xStart, "%s", " \\ V /___  _   _   | |  | | ___  _ __   | |");
    mvwprintw(end_win.win,yStart+3, xStart, "%s", "  \\ // _ \\| | | |  | |/\\| |/ _ \\| '_ \\  | |");
    mvwprintw(end_win.win,yStart+4, xStart, "%s", "  | | (_) | |_| |  \\  /\\  / (_) | | | | |_|");
    mvwprintw(end_win.win,yStart+5,xStart, "%s", "  \\_/\\___/ \\__,_|   \\/  \\/ \\___/|_| |_| (_)");
    wattron(end_win.win,A_BLINK);
    mvwprintw(end_win.win,yStart+6,xStart, "         Press any button to continue");
    wattroff(end_win.win,A_BLINK);
    wrefresh(end_win.win);
}

void init_color_pairs() {
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
}

void game_loop(Level& level, Game_state& game_state) {

    int win_y_start = (level.yMax-level.winHeight)/2;
    int win_x_start = (level.xMax-level.winWidth)/2;
    WINDOW *play_win = newwin(level.winHeight, level.winWidth, win_y_start, win_x_start);
    keypad(play_win, true);

    init_color_pairs();
    Coordinates pos(1,1);


    mvprintw(0, 0, "Round:  0");
    refresh();
    level.print_level(play_win);
    wrefresh(play_win);

    std::ifstream rounds_file(ROUNDS_PATH);

    Win_data win_data = {play_win, level.winHeight, level.winWidth, win_x_start, win_y_start};
    
    for (;curr_round < rounds_count ;curr_round++) {
        round_preparation(pos, win_data, level, game_state);
        if (game_state.quit) break;
        mvprintw(0, 0, "Round: %2ld", curr_round);
        game_state.load_next_round(rounds_file);
        for (auto& turret :game_state.turrets) turret->round_reset();
        round_loop(win_data, level, game_state, pos);
        if (game_state.quit) break;
        if (game_state.curr_hp < 1) {
            game_over_screen(curr_round, rounds_count);
            wgetch(stdscr);
            break;
        }
        game_state.mv_objects.vec.clear();
        game_state.money += ROUND_BONUS;
    }
    if (game_state.curr_hp > 0 && !game_state.quit) {
        win_screen();
        wgetch(stdscr);
    }
    rounds_file.close();

    wclear(play_win);
    wrefresh(play_win);
    clear();
    refresh();
}
