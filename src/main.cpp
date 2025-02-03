#include <ncurses.h>
#include <iostream>

int main() {
    // screan initialization
    // memory setup && screen clear
    initscr();

    //print message to the window
    printw("ahoj");

    // refresh screen to mach what is in the user memory
    refresh();

    // get user input
    int c = getchar();
    printw("%d", c);
    refresh();


    //wait for keypress to close window
    getchar();

    endwin();

    return 0;
}
