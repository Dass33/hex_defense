#include "../libs/moving_objects.h"

char Moving_object::get_char() const{
    if (hp == 0) return 0;
    if (hp < 10) return  hp + '0';
    return 'A' + hp - 10;
}

