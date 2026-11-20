#include "../include/tree.h"
#include "raylib.h"
#include "../include/game.h"

int main () {

    start_game_graphics ();
    
    return 0;
}

/*
char chosen_mode = choose_mode ();

switch (chosen_mode) {
    case TERMINAL:   start_game_in_terminal ();
                break;
    case GRAPHICS:   start_game_graphics ();
                break;
    case DEFINITION: start_definition ();
                break;
    case COMPARISON: start_comparison ();
                break;
    default: printf ("You wrote something else");
                break;
}*/
