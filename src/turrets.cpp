#include "../libs/turrets.h"

Turrets::Turrets(const Coordinates pos){
    this->pos.x = pos.x + 1;
    this->pos.y = pos.y;
}

Coordinates Turrets::get_pos() {
    return pos;
}

FireWall::FireWall(Coordinates pos) :Turrets(pos){}

void FireWall::attack() {}

void FireWall::upgrade() {}

void FireWall::print(WINDOW* win) const {
    mvwprintw(win, pos.y, pos.x -1, "%s", TOWERS_ICONS[1]);
}

Blue_teamer::Blue_teamer(Coordinates pos) :Turrets(pos){}

void Blue_teamer::attack() {}

void Blue_teamer::upgrade() {}

void Blue_teamer::print(WINDOW* win) const {
    mvwprintw(win, pos.y, pos.x -1, "%s", TOWERS_ICONS[2]);
}

Anti_hex::Anti_hex(Coordinates pos) :Turrets(pos){}

void Anti_hex::attack() {}

void Anti_hex::upgrade() {}

void Anti_hex::print(WINDOW* win) const {
    mvwprintw(win, pos.y, pos.x -1, "%s", TOWERS_ICONS[0]);
}
