#include "tree.h"
#include "raylib.h"
#include "game.h"

int main () {

    char chosen_mode = choose_mode ();

    switch (chosen_mode) {
        case 't': start_game_in_terminal ();
                    break;
        case 'r': start_game_graphics ();
                    break;
        case 'd': start_definition ();
                    break;
        default:  printf ("You wrote something else");
                    break;
    }

    return 0;
}
    /*tree_t tree = {};

    FILE* in = fopen ("tree_data.txt", "r");
    if (in != NULL) {
        tree.root = read_data_from_file (in);
        tree.size_of_tree = tree_size (tree.root);
        fclose (in);
    }

    else {
        tree_ctor (&tree, "chelovek");
        init_first_question (&tree);
    }

    FILE* def_in = fopen ("tree_data.txt", "r");
    if (def_in != NULL) {
        start_definition (def_in);
        fclose (def_in);
    }
    start_game (&tree);
    InitWindow (800, 450, "raylib test");
    SetTargetFPS (60);

    while (!WindowShouldClose()) {
        BeginDrawing ();
        ClearBackground (RAYWHITE);
        EndDrawing ();
    }

    CloseWindow ();

    start_definition (in);
    FILE* check_out = fopen ("check_out.txt", "w");
    int rank = 0;
    print_tree (tree.root, check_out, &rank);
    fclose (check_out);

    dump_tree  (&tree, "akinator.dot");
    destroy_tree (tree.root);*/


