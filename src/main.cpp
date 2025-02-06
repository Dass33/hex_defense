#include <cctype>
#include <cstdio>
#include <ncurses.h>
#include <ostream>
#include <iostream>
#include "../libs/level.h"

//static constexpr int Y_MIN_SCREEN = 30,X_MIN_SCREEN = 90;
static constexpr int Y_MIN_SCREEN = 24,X_MIN_SCREEN = 80;


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
    start_color();

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

    
    int input = ' ';
    int prev_input = ' ';

    for (;;) {
        getmaxyx(stdscr, yMax, xMax);
        WINDOW *menu = newwin(yMax /2, xMax/2, yMax /4, xMax /4);
        display_menu(menu);
        input = wgetch(menu);
        mvprintw(yMax-1,1,"Last input: %c ", input);
        refresh();

        if (prev_input == ':' && input == 'q') break;

        if (input == 'e') level_edit_menu(menu);
        else if (input == 'p') level_play_menu(menu);
        prev_input = input;
    }

    endwin();
    return 0;
}
