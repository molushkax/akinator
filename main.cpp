#include "tree.h"

int main () {
    tree_t tree = {};

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
    //start_game (&tree);

    start_definition (in);
    FILE* check_out = fopen ("check_out.txt", "w");
    int rank = 0;
    print_tree (tree.root, check_out, &rank);
    fclose (check_out);

    dump_tree  (&tree, "akinator.dot");
    destroy_tree (tree.root);

    return 0;
}

