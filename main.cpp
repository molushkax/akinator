#include "tree.h"

int main () {
    tree_t tree = {};

    FILE* in = fopen ("tree_data.txt", "r");
    if (in != NULL) {
        tree.root = read_data_from_file (in);
        tree.size_of_tree = tree_size (tree.root);
    }

    else {
        tree_ctor (&tree, "chelovek");
        init_first_question (&tree);
    }

    FILE* check_out = fopen ("check_out.txt", "w");
    print_tree (tree.root, check_out);

    if (in != NULL) fclose (in);
    fclose (check_out);
    start_game (&tree);

    return 0;
}

