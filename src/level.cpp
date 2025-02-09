#include "../libs/level.h"
#include "../libs/game_state.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <ncurses.h>
#include <string>
#include <fstream>
#include <vector>
#include <unistd.h> 
/**
 * @file level.cpp
 * @brief Implementation of level management and editing functionality for a game
 */

Level::Level() {
    getmaxyx(stdscr, yMax, xMax);
    winHeight = yMax / (3.0/2);
    winWidth = xMax / (3.0/2);
    // should allocate less memory
    tiles.resize((winWidth-2) * (winHeight-2), ' ');
}

void Level::insert(const int x, const int y, const int val) {
    tiles[(y-1) * (winWidth-2) + (x-1)] = val;
}
 
char Level::at(const size_t x, const size_t y) const {
    if (x > winWidth || y > winHeight) return -1;
    if (x < 1 || y < 1) return -1;
    return tiles[(y-1) * (winWidth-2) + (x-1)];
}

void Level::save_level(std::string& level_path) const{
    std::ofstream ofs(level_path);
    ofs << winHeight << ' ' << winWidth << ' ';
    for(size_t i = 0; i < tiles.size(); i++) {
        ofs.put(tiles[i]);
    }
    ofs.close();
}

void Level::print_level(WINDOW *win) const{
    box(win,0,0);

    int e = 1;
    for (size_t i = 0; i < tiles.size(); i++) {
        if (i % (winWidth-2) == 0) {
            wmove(win,e++,1);
        }
        wprintw(win, "%c", tiles[i]);
    }
}

void Level::get_coordinates(const size_t index, Coordinates& pos) const {
    pos.x = index % (winWidth-2) + 1;
    pos.y = index / (winWidth-2) + 1;
}

size_t Level::find_next_tile(const char key, Coordinates& pos, Coordinates& prev_pos) {
    size_t res = 0;
    const size_t max_cord_x = winWidth-2;
    const size_t max_cord_y = winHeight-2;
    Coordinates tmp_pos(pos);

    const std::vector<std::pair<int, int>> directions =
        {{1,0},{-1,0},{0,1},{0,-1}};

    for (const auto& [dx, dy] : directions) {
        Coordinates new_pos = {pos.x + dx, pos.y + dy};

        if (new_pos.x < 1 || new_pos.x > max_cord_x
         || new_pos.y < 1 || new_pos.y > max_cord_y) continue;

        if (at(new_pos.x, new_pos.y) == key &&
            (new_pos.x != prev_pos.x || new_pos.y != prev_pos.y)) {
            res++;
            tmp_pos.x += dx;
            tmp_pos.y += dy;
        }
    }
    road.push_back(pos);
    prev_pos = pos;
    pos = tmp_pos;
    return res;
}

bool Level::build_path() {
    Coordinates curr_pos(road_start);
    Coordinates prev_pos(SIZE_MAX, SIZE_MAX);

    size_t ends_found = 0;
    for (;;) {
        size_t paths_found = find_next_tile('.', curr_pos, prev_pos);
        if (!paths_found) ends_found += find_next_tile('#', curr_pos, prev_pos);
        if (ends_found == 1 && !paths_found) return true;
        if (paths_found != 1 || ends_found > 1) return false;
    }
    return false;
}

bool Level::build_level() {
    if (START_CHAR == END_CHAR) return false;

    size_t start_char_count = 0, end_char_count = 0;

    for (size_t i = 0; i < tiles.size(); i++) {
        if (tiles[i] == START_CHAR) {
            start_char_count++;
            get_coordinates(i, road_start);
        }
        else if (tiles[i] == END_CHAR) {
            end_char_count++;
            get_coordinates(i, road_end);
        }
    }
    if (start_char_count != 1 || end_char_count != 1) return false;

    return build_path();
}

bool Level::clear_n_tiles(Coordinates const pos, size_t width) const {
        for (size_t i = 0; i < width; i++) {
            if (at(pos.x + i, pos.y) != ' ') return false;
        }
    return true;
}

void get_level(Level &level, std::string level_path) {
    try {
        std::ifstream ifs(level_path.data());
        if (!ifs.good()) return;

        size_t winHeight, winWidth;
        if (!(ifs >> winHeight >> winWidth)) return;
        if (winHeight < MIN_LEVEL_HEIGHT || winWidth < MIN_LEVEL_WIDTH) return;
        if (winHeight > MAX_LEVEL_HEIGHT || winWidth > MAX_LEVEL_WIDTH) return;

        ifs.get();
        std::vector<char> tmp_vec;
        for (size_t i = 0; i < (winWidth -2) * (winHeight -2);i++) {
            char tmp_char;
            if (!ifs.get(tmp_char)) return;
            tmp_vec.push_back(tmp_char);
        }
        ifs.close();
        
        level.winHeight = winHeight;
        level.winWidth = winWidth;
        level.tiles = std::move(tmp_vec);

    } catch(const std::exception& e) {
        mvprintw(0,1,"%s", e.what());
    }
}

void Level::edit_level() {
    int win_y_start = (yMax-winHeight)/2;
    int win_x_start = (xMax-winWidth)/2;
    WINDOW *level_editor = newwin(winHeight, winWidth, win_y_start, win_x_start);

    print_level(level_editor);
    //box(level_editor, 0, 0);

    keypad(level_editor, true);

    size_t x = 1, y = 1;
    int c;
    int prev_c = 0;
    int print_char = 0;
    bool print_toggle = false;

    mvwprintw(level_editor,1,1,"@");
    //mvprintw(yMax-1,1,"Last input: ");
    wrefresh(level_editor);
    do {
        c = wgetch(level_editor);
        // 10 is the code for Enter, KEY_ENTER did not work
        // 32 is the code for Space...
        if (prev_c == KEY_BACKSPACE) insert(x,y,' ');
        else if (isprint(print_char) && (print_toggle || prev_c == 32)) {
            insert(x,y,print_char);
        }
        mvwprintw(level_editor,y,x, "%c", at(x,y));

        switch (c) {
            case KEY_UP:
            //case 'k':
                y--;
                if (y < 1) y = winHeight -2;
            break;
            case KEY_DOWN:
            //case 'j':
                y++;
                if (y > winHeight -2) y = 1;
            break;
            case KEY_RIGHT:
            //case 'l':
                x++;
                if (x > winWidth -2) x = 1;
            break;
            case KEY_LEFT:
            //case 'h':
                x--;
                if (x < 1) x = winWidth -2;
            break;
            // consts for enter and space don't work
            case 32: break;
            case 10:
                print_toggle = !print_toggle;
            break;
            default: print_char = c;
        }
        mvwprintw(level_editor,y,x, "@");
        box(level_editor, 0, 0);

        prev_c = c;
        mvprintw(yMax-1,1,"Last input: %c ", c);
        mvprintw(yMax-2,1,"at(x,y) = '%c'", at(x,y));
        mvprintw(yMax-3,1,"print char: %c ", print_char);
        mvprintw(yMax-4,1,"pos %ld ", (y-1) * (winWidth-2) + (x-1));
        refresh();
    } while (c != 'q');
    wclear(level_editor);
    wrefresh(level_editor);
}

bool level_fits_on_screen(int index){
    std::string path = LEVEL_PATH + std::to_string(index);
    double yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    std::ifstream ifs(path);

    size_t winHeight, winWidth;
    // if the size can't be read, returns true
    if (!(ifs >> winHeight >> winWidth)) return true;
    ifs.close();
    if (winHeight + LEVEL_HEIGHT_PADDING > yMax) return false;
    if (winWidth + LEVEL_WIDTH_PADDING > xMax) return false;

    return true;
}

//-------------------------------------------------------------------------------

void level_edit_menu(WINDOW* menu) {
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    wclear(menu);
    box(menu, 0, 0);
    // e is button used to get to level edit menu
    int input = 'e';

    mvwprintw(menu,0,2,"Level Edit Menu");

    init_pair(1, COLOR_RED, COLOR_BLACK);

    for (int i = 0; i <= MAX_LEVEL;i++) {
        bool fits_screen = true;
        if (std::filesystem::exists(LEVEL_PATH + std::to_string(i))) {
            wattron(menu, A_REVERSE);
            fits_screen = level_fits_on_screen(i);

            if (!fits_screen) wattron(menu, COLOR_PAIR(1));
        }
        mvwprintw(menu,i+2,2,"Level%d", i);
        wattroff(menu, A_REVERSE | COLOR_PAIR(1));
        if (!fits_screen) wprintw(menu," //too small terminal");
    }
    mvwprintw(menu,MAX_LEVEL + 4,2,"Choose level to edit by pressing '0-%d'", MAX_LEVEL);
    mvwprintw(menu,MAX_LEVEL + 5,2,"Highlited levels already exist.");
    mvwprintw(menu,MAX_LEVEL + 7,2,"Or press q to go back to main menu.");

    for(;;) {
        mvprintw(yMax-1,1,"Last input: %c ", input);
        input = wgetch(menu);
        if (input == 'q') return;
        if (!level_fits_on_screen(input - '0')) continue;
        if (input - '0' <= MAX_LEVEL && input - '0' >= 0) break;
    }

    //mvwprintw(menu,1,3,"Level choses: %d", input);


    std::string level_path = LEVEL_PATH + std::string(1, input);
    clear();

    Level level;
    get_level(level, level_path);
    level.edit_level();
    level.save_level(level_path);
    wclear(menu);
}


void level_play_menu(WINDOW* menu) {
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    wclear(menu);
    box(menu, 0, 0);
    // p is button used to get to play menu
    int input = 'p';

    mvwprintw(menu,0,2,"Level Play Menu");

    init_pair(1, COLOR_RED, COLOR_BLACK);


    size_t levels_exist = 0;
    for (int i = 0; i <= MAX_LEVEL;i++) {
        bool fits_screen = true;
        if (!std::filesystem::exists(LEVEL_PATH + std::to_string(i))) continue;
        if (!level_fits_on_screen(input - '0')) continue;
        wattron(menu, A_REVERSE);
        fits_screen = level_fits_on_screen(i);

        if (!fits_screen) wattron(menu, COLOR_PAIR(1));
        mvwprintw(menu,++levels_exist+1,2,"Level%d", i);
        wattroff(menu, A_REVERSE | COLOR_PAIR(1));
        if (!fits_screen) wprintw(menu," //too small terminal");
    }

    mvwprintw(menu,levels_exist + 3,2,"Choose level to play by pressing '0-%d'", MAX_LEVEL);
    mvwprintw(menu,levels_exist + 4,2,"Or press q to go back to main menu.");

    for(;;) {
        mvprintw(yMax-1,1,"Last input: %c ", input);
        input = wgetch(menu);
        if (input == 'q') return;
        // put this if back in!!!
        //if (!level_fits_on_screen(input - '0')) continue;
        if (input - '0' <= MAX_LEVEL && input - '0' >= 0) break;
    }

    std::string level_path = LEVEL_PATH + std::string(1, input);
    clear();

    Level level;
    get_level(level, level_path);

    yMax = getmaxy(menu);
    if (!level.build_level()) {
        wattron(menu, COLOR_PAIR(1));
        mvwprintw(menu, yMax -1, 1, "Can't build level.");
        wattroff(menu, COLOR_PAIR(1));
        wgetch(menu);
        level_play_menu(menu);
    } else {
        if (!select_game_mode(menu, level))
            level_play_menu(menu);
    }
    wclear(menu);
}
