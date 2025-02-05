#include <cctype>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <ncurses.h>
#include <ostream>
#include <iostream>
#include <string>
#include <vector>

//static constexpr int Y_MIN_SCREEN = 30,X_MIN_SCREEN = 90;
static constexpr int Y_MIN_SCREEN = 24,X_MIN_SCREEN = 80;
static constexpr int MAX_LEVEL = 6;
static constexpr int MIN_LEVEL_WIDTH = 60, MIN_LEVEL_HEIGHT = 16;
static constexpr int MAX_LEVEL_WIDTH = 190, MAX_LEVEL_HEIGHT = 55;
static constexpr char LEVEL_PATH[] = "./assets/level";


class Level {
public:
    Level();

    void insert(int x, int y, int val);
    [[nodiscard]] char at(int x, int y) const;

    void edit_level();
    void print_level(WINDOW *win) const;
    void save_level(std::string& level_path) const;
    friend void get_level(Level &level, std::string level_path);
private:
        std::vector<char> tiles;
        size_t yMax, xMax;
        size_t winHeight, winWidth;
};

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
 
char Level::at(const int x, const int y) const {
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
    //todo improve printing
    box(win,0,0);

    int e = 1;
    for (size_t i = 0; i < tiles.size(); i++) {
        if (i % (winWidth-2) == 0) {
            wmove(win,e++,1);
        }
        wprintw(win, "%c", tiles[i]);
    }
}

bool is_valid_level(Level& level) {
    //todo
    //if (!isprint(level.at(0,0))) return false;
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

        getmaxyx(stdscr, level.yMax, level.xMax);
        level.winHeight = level.yMax / (3.0/2);
        level.winWidth = level.xMax / (3.0/2);
    } catch(const std::exception& e) {
        mvprintw(0,1,"%s", e.what());
    }
}

void Level::edit_level() {
    WINDOW *level_editor = newwin(winHeight, winWidth, yMax/6,xMax/6);

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
    //wclear(level_editor);

}

//todo open level with right size
//or forbid opening it
void level_menu(WINDOW* menu) {
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    wclear(menu);
    box(menu, 0, 0);
    int input = 'e';


    wmove(menu, 3,2);
    wprintw(menu,"Level choses: ");
    wattron(menu, A_REVERSE);
    wprintw(menu, " ");
    wattroff(menu, A_REVERSE);

    do {
        mvprintw(yMax-1,1,"Last input: %c ", input);

        mvwprintw(menu,0,2,"Level Edit Menu");
        mvwprintw(menu,2,2,"Choose level to edit by pressing '0-%d'", MAX_LEVEL);
        mvwprintw(menu,3,2,"Then choose level width '%d-%d", MIN_LEVEL_WIDTH, MAX_LEVEL_WIDTH);
        mvwprintw(menu,4,2,"And level height '%d-%d", MIN_LEVEL_HEIGHT, MAX_LEVEL_HEIGHT);
        mvwprintw(menu,5,2,"Or press q to go back to main menu.");

        input = wgetch(menu);
        if (input == 'q') return;
    } while (input - '0' > MAX_LEVEL  || input - '0' < 0);
    mvwprintw(menu,1,3,"Level choses: %d", input);


    std::string level_path = LEVEL_PATH + std::to_string(input - '0');
    clear();

    Level level;
    get_level(level, level_path);
    level.edit_level();
    level.save_level(level_path);
    wclear(menu);
}

void display_menu(WINDOW* menu) {
    wclear(menu);
    box(menu, 0, 0);
    mvwprintw(menu,0,2,"Main menu");
    mvwprintw(menu,2,2,"To play press 'p'");
    mvwprintw(menu,3,2,"To edit levels press 'e'");
    mvwprintw(menu,4,2,"To exit the game press :q");
    mvwprintw(menu,6,2,"Resizing terminal is at your own risk.");
    mvwprintw(menu,7,2,"Levels have different min terminal sizes.");
}

void display_title(int yStart, int xStart) {
    mvprintw(yStart, xStart, "%s", " _   _                 _       __                    ");
    mvprintw(yStart+1, xStart, "%s", "| | | |               | |     / _|                   ");
    mvprintw(yStart+2, xStart, "%s", "| |_| | _____  __   __| | ___| |_ ___ _ __  ___  ___ ");
    mvprintw(yStart+3, xStart, "%s", "|  _  |/ _ \\ \\/ /  / _` |/ _ \\  _/ _ \\ '_ \\/ __|/ _ \\");
    mvprintw(yStart+4, xStart, "%s", "| | | |  __/>  <  | (_| |  __/ ||  __/ | | \\__ \\  __/");
    mvprintw(yStart+5,xStart, "%s", "\\_| |_/\\___/_/\\_\\  \\__,_|\\___|_| \\___|_| |_|___/\\___|");
    attron(A_BLINK);
    mvprintw(yStart+6,xStart, "%s","              Press any button to continue           ");
    attroff(A_BLINK);
    refresh();
}

int main() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    if (xMax < X_MIN_SCREEN || yMax < Y_MIN_SCREEN) {
        endwin();
        std::cout << "\nTo small screen\n";
        std::cout << "min screen: " << X_MIN_SCREEN << "x"  << Y_MIN_SCREEN << std::endl;
        std::cout << "curr screen: " << xMax << "x"  << yMax << std::endl;
        return 1;
    }
    //these constants should center the title
    display_title(yMax/2-6, xMax/2-27);
    getchar();

    WINDOW *menu = newwin(yMax /2, xMax/2, yMax /4, xMax /4);
    
    int input = ' ';
    int prev_input = ' ';

    for (;;) {
        display_menu(menu);
        input = wgetch(menu);
        mvprintw(yMax-1,1,"Last input: %c ", input);
        refresh();

        if (prev_input == ':' && input == 'q') break;

        if (input == 'e') level_menu(menu);
        else if (input == 'p') {
            wclear(menu);
            mvwprintw(menu, 1, 1, "You choose to play.");
            wgetch(menu);
            wclear(menu);
            wrefresh(menu);
        }
        prev_input = input;
    }

    endwin();
    return 0;
}
