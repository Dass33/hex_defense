#include <cstdio>
#include <fstream>
#include <functional>
#include <ncurses.h>
#include <ostream>
#include <iostream>
#include <string>

void create_level(std::string level_path) {
    keypad(stdscr, true);
    std::ofstream ofs(level_path.data());

    int x = 0, y = 0;
    int c;
    int prev_c = 0;

    printw("@");
    do {
        c = getch();
        if (prev_c == 10) mvprintw(y,x, ".");
        else mvprintw(y,x, " ");

        switch (c) {
            case KEY_UP:
                y--;
            break;
            case KEY_DOWN:
                y++;
            break;
            case KEY_RIGHT:
                x++;
            break;
            case KEY_LEFT:
                x--;
            break;
            case 10:
                ofs << x << ' ' << y << "\t";
            break;
        }
        mvprintw(y,x, "@");
        refresh();

        prev_c = c;
    } while (c != 'q');

    ofs.close();
}

void display_menu(WINDOW* menu) {
    box(menu, 0, 0);
    mvwprintw(menu,0,2,"Main menu");
    mvwprintw(menu,2,1,"To play press 'p'");
    mvwprintw(menu,3,1,"To edit levels press 'e'");
    mvwprintw(menu,4,1,"To exit the game press :q");
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

    constexpr int yMin = 30,xMin = 90;
    if (xMax < xMin || yMax < yMin) {
        endwin();
        std::cout << "\nTo small screen, min screen: " << xMin << "x"  << yMin << std::endl;
        return 1;
    }

    display_title(yMax/2-6, xMax/2-27);
    getchar();

    WINDOW *menu = newwin(yMax /2, xMax/2, yMax /4, xMax /4);
    
    int input = ' ';
    int prev_input = ' ';

    for (;;) {
        display_menu(menu);
        input = wgetch(menu);
        mvprintw(yMax-1,1,"Last input: %c", input);
        refresh();

        if (prev_input == ':' && input == 'q') break;

        if (input == 'e') {
            clear();
            std::string level_path = "./assets/level1";
            create_level(level_path);

        } else if (input == 'p') {
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
