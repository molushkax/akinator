#include "tree.h"

tree_error_t tree_ctor (tree_t* tree, const char* value_of_root) {
    assert (tree != NULL);

    tree->root = (leaf_t*)calloc (1, sizeof (leaf_t));
    if (tree->root == NULL) return TREE_ERROR_WITH_ALLOCATION;
    tree->size_of_tree   = 1;
    (tree->root)->text   = value_of_root;
    (tree->root)->negative  = NULL;
    (tree->root)->positive   = NULL;
    (tree->root)->parent = tree->root;

    TREE_ASSERT (tree);
    return TREE_OK;
}

leaf_t* create_leaf (leaf_t* leaf) {
    leaf = (leaf_t*)calloc (1, sizeof (leaf_t));
    return leaf;
}

void create_children (leaf_t* leaf) {
    assert (leaf != NULL);

    leaf->negative = create_leaf (leaf->negative);
    (leaf->negative)->parent = leaf;
    leaf->positive = create_leaf (leaf->positive);
    (leaf->positive)->parent = leaf;
}

void init_first_question (tree_t* tree) {
    TREE_ASSERT (tree);

    create_children (tree->root);
    ((tree->root)->negative)->text = "poltorashka";
    ((tree->root)->positive)->text = "meee";

    return;
}

leaf_t* read_data_from_file (FILE* in) {
    assert (in != NULL);

    fseek (in, 0, SEEK_END);
    size_t file_size = ftell (in);
    fseek (in, 0, SEEK_SET);

    char* file_info = (char*)calloc (file_size + 1, sizeof (char));
    if (file_info == NULL) return NULL;

    size_t read_bytes = fread (file_info, 1, file_size, in);
    file_info[read_bytes] = '\0';

    buffer buf = { .data = file_info, .position = 0};

    leaf_t* root = read_data_from_buffer (&buf);

    free (file_info);

    return root;
}

leaf_t* read_data_from_buffer (buffer* buf) {
    assert (buf != NULL);

    if (skip_spaces_in_buffer (buf) != '(') return NULL;
    get_char_from_buffer (buf);

    char text[128] = {};
    buffer_read_word (buf, text, sizeof (text));

    size_t len = strlen (text);
    if (len == 0) return NULL;

    if (text[len - 1] == '(' || text[len - 1] == ')') {
        char bracket = text[len - 1];
        text[len - 1] = '\0';
        return_back_in_buffer (bracket, buf);

        if (len == 1) return NULL;
    }

    leaf_t* leaf = (leaf_t*)calloc (1, sizeof (leaf_t));
    if (leaf == NULL) return NULL;

    leaf->text = strdup (text);
    leaf->positive = read_data_from_buffer (buf);
    leaf->negative = read_data_from_buffer (buf);

    if (skip_spaces_in_buffer (buf) == ')') get_char_from_buffer (buf);

    if (leaf->positive) (leaf->positive)->parent = leaf;
    if (leaf->negative) (leaf->negative)->parent = leaf;

    return leaf;
}

void buffer_read_word (buffer* buf, char* text, size_t max_len) {
    assert (buf != NULL);
    assert (text != NULL);

    size_t i = 0;
    int c = 0;

    while ((c = get_char_from_buffer (buf)) != EOF && isspace (c)) {}
    if (c == EOF) {
        text[0] = '\0';
        return;
    }

    while (c != EOF && !isspace(c) && c != ')' && c != '(') {
        if (i < max_len - 1) {
            text[i++] = (char)c;
        }
        c = get_char_from_buffer (buf);
    }

    text[i] = '\0';

    if (c != EOF) return_back_in_buffer (c, buf);
}

int get_char_from_buffer (buffer* buf) {
    assert (buf != NULL);

    if (buf->data[buf->position] == '\0') return EOF;

    return buf->data[buf->position++];
}

void return_back_in_buffer (int c, buffer* buf) {
    assert (buf != NULL);

    if (c != EOF && buf->position > 0) buf->position--;
}

int skip_spaces_in_buffer (buffer* buf) {
    assert (buf != NULL);

    int c = 0;
    while ((c = get_char_from_buffer (buf)) != EOF && isspace (c)) {}
    if (c != EOF) return_back_in_buffer (c, buf);

    return c;
}

size_t tree_size (const leaf_t* leaf) {
    if (leaf == NULL) return 0;
    return 1 + tree_size (leaf->positive) + tree_size (leaf->negative);
}

tree_error_t start_game (tree_t* tree) {
    TREE_ASSERT (tree);

    tree->current_leaf = tree->root;
    cool_print (MY_PURPLE "Hello, my dear user. \nLet's play with my akinator\n" NO_COLOR, NORM_TEMP);

    char answer[4] = {};
    while ((tree->current_leaf)->positive != NULL) {
        cool_print (MY_PURPLE "\nEto ", DED_TEMP);
        printf ("%s?\n" NO_COLOR, (tree->current_leaf)->text);
        scanf ("%s", answer);
        if (strcmp (answer, "yes") == 0) tree->current_leaf = (tree->current_leaf)->positive;
        else tree->current_leaf = (tree->current_leaf)->negative;
    }

    char expected_answer[24] = {};
    if (strcmp (answer, "yes") == 0) {
        cool_print (MY_YELLOW"\nThis is ", NORM_TEMP);
        printf ("%s\n\n" NO_COLOR, ((tree->current_leaf)->text));
        strcpy (expected_answer, ((tree->current_leaf)->text));
    }
    else {
        cool_print (MY_YELLOW"\nThis is ", NORM_TEMP);
        printf ("%s\n\n" NO_COLOR, ((tree->current_leaf)->text));
        strcpy (expected_answer, ((tree->current_leaf)->text));
    }
    cool_print (MY_PURPLE "Is this answer correct?\n" NO_COLOR, NORM_TEMP);

    char check_answer[4] = {};
    scanf ("%s", check_answer);
    if (strcmp (check_answer, "yes") == 0) cool_print (MY_PURPLE "I'm happy that it's true, thank you for using me\n" NO_COLOR, NORM_TEMP);
    else {
        cool_print (MY_PURPLE "What did you actually wish for?\n" NO_COLOR, NORM_TEMP);
        char real_answer[24] = {};
        scanf ("%s", real_answer);
        char particularity[24] = {};
        cool_print (MY_PURPLE "And how is ", NORM_TEMP);
        printf ("%s", real_answer);
        cool_print (" different from ", NORM_TEMP);
        printf ("%s?\n" NO_COLOR, expected_answer);
        cool_print (MY_PURPLE "She/He/It is ... ", NORM_TEMP);
        scanf ("%s" NO_COLOR, particularity);

        create_children (tree->current_leaf);
        (tree->current_leaf)->text = particularity;
        ((tree->current_leaf)->positive)->text = real_answer;
        ((tree->current_leaf)->negative)->text = expected_answer;

        FILE* out = fopen ("tree_data.txt", "w");
        update_base (tree->root, out);
    }

    return TREE_OK;
}

void print_tree (leaf_t* leaf, FILE* out, int* rank) {
    if (leaf) {
        fprintf (out, "\n");
        for (int i = 0; i < *rank; i ++) {
            fprintf (out, "    ");
        }
        fprintf (out, "( %s ", leaf->text);
    }

    else fprintf (out, "( %s ", leaf->text);
    (*rank)++;

    if (leaf->positive) print_tree (leaf->positive, out, rank);
    if (leaf->negative) print_tree (leaf->negative, out, rank);
    (*rank)--;

    if (leaf->positive) {
        fprintf (out, "\n");
            for (int i = 0; i < *rank; i ++) {
                fprintf (out, "    ");
            }
        fprintf (out, ")");
    }

    else fprintf (out, ")");
}

void update_base (leaf_t* leaf, FILE* out) {
    fprintf (out, "( %s ", leaf->text);
    if (leaf->positive) update_base (leaf->positive, out);
    if (leaf->negative) update_base (leaf->negative, out);
    fprintf (out, " )");
}

int tree_verify (tree_t* tree, const char* function_name, int line) {
    if (tree == NULL) return 1;
    int n = 0;

    if (tree->size_of_tree < 1) {
        print_tree_error_context (tree, function_name, line);
        fprintf(stderr, MY_RED "ERROR: size of tree is less than 1, it's abnormal\n" NO_COLOR);
        n++;
    }

    return n;
}

void print_tree_error_context (tree_t* tree, const char* func, int line) {
    fprintf(stderr,
            "Error context:\n"
            "  file:       %s\n"
            "  function:   %s\n"
            "  line:       %d\n"
            "  struct:     tree_t at %p\n",
            __FILE__, func, line, (void*)tree);

    if (tree != NULL) {
        fprintf(stderr,
            "  tree: %p\n"
            "  size:      %zu\n",
            (void*)tree,
            tree->size_of_tree);
    }

    else {
        fprintf(stderr, "\n");
    }
}

void cool_print (const char* out_text, int delay) {
    assert (out_text != NULL);

    for (unsigned int i = 0;  i < strlen (out_text); i++) {
        putchar (out_text [i]);
        fflush (stdout);
        usleep (delay * 1000);
    }
}

void dump_node (const leaf_t* leaf, FILE* out, size_t* id) {
    if (leaf == NULL) return;

    size_t my_id = (*id)++;
    fprintf (out, "    n%zu [label=\"%s\", color=\"#3c84a7\"];\n", my_id, leaf->text);

    if (leaf->positive) {
        size_t pos_child_id = *id;
        fprintf (out, "    n%zu -> n%zu [label=\"yes\", color=\"#24e216\"];\n", my_id, pos_child_id);
        dump_node (leaf->positive, out, id);
    }

    if (leaf->negative) {
        size_t neg_child_id = *id;
        fprintf (out, "    n%zu -> n%zu [label=\"no\", color=\"#D32F2F\"];\n", my_id, neg_child_id);
        dump_node (leaf->negative, out, id);
    }
}

void dump_tree (const tree_t* tree, const char* filename) {
    if (tree == NULL || tree->root == NULL) return;

    FILE* out = fopen (filename, "w");
    if (out == NULL) return;

    fprintf (out, "digraph Tree {\n");
    fprintf (out, "    node [shape=box, fontsize=12];\n");
    fprintf (out, "    graph [overlap=false, splines=true];");
    size_t id = 0;
    dump_node (tree->root, out, &id);

    fprintf (out, "}\n");
    fclose (out);
}

char* get_buffer_with_info_from_file (FILE* in) {
    assert (in != NULL);

    fseek (in, 0, SEEK_END);
    size_t file_size = ftell (in);
    fseek (in, 0, SEEK_SET);

    char* file_info = (char*)calloc (file_size + 1, sizeof (char));
    if (file_info == NULL) return NULL;

    size_t read_bytes = fread (file_info, 1, file_size, in);
    file_info[read_bytes] = '\0';

    return file_info;
}

void start_definition (FILE* tree_data) {
    assert (tree_data != NULL);

    char word_to_define[64] = {};
    if (scanf ("%63s", word_to_define) != 1) return;

    char* buffer_for_def = get_buffer_with_info_from_file (tree_data);
    if (buffer_for_def == NULL) return; //TODO errors

    buffer buf = { .data = buffer_for_def, .position = 0 };
    leaf_t* root = read_data_from_buffer (&buf);
    if (root == NULL) {
        free (buffer_for_def);
        return;
    }

    leaf_t* target = find_character (root, word_to_define);
    if (target == NULL) {
        cool_print (MY_RED "Not found in base: " NO_COLOR, NORM_TEMP);
        printf ("%s\n", word_to_define);
    }

    else {
        print_character_features (target, word_to_define);
    }

    free (buffer_for_def);
    destroy_tree (root);
}

leaf_t* find_character (leaf_t* leaf, const char* name) {
    if (leaf == NULL) return NULL;

    if (leaf->positive == NULL && leaf->negative == NULL) {
        if (strcmp (leaf->text, name) == 0) return leaf;

        else return NULL;
    }

    leaf_t* found = find_character (leaf->positive, name);
    if (found != NULL) return found;

    found = find_character (leaf->negative, name);
    if (found != NULL) return found;

    return NULL;
}

void print_character_features (leaf_t* target, const char* name) {
    assert (target != NULL);
    assert (name   != NULL);

    char* path[64] = {};
    int count = 0;

    leaf_t* current = target;
    while (current->parent != NULL && current->parent != current) {
        leaf_t* parent = current->parent;
        int is_yes = (parent->positive == current);

        size_t len = strlen (parent->text) + 10;
        path[count] = (char*)calloc (len, sizeof (char));

        if (path[count] == NULL) break;
        if (is_yes == 1) {
            snprintf (path[count], len, "%s", parent->text);
        }

        else snprintf (path[count], len, "not %s", parent->text);

        count++;

        current = parent;
    }

    cool_print (MY_YELLOW "\nDefinition:\n" NO_COLOR, NORM_TEMP);
    printf ("%s:\n" NO_COLOR, name);

    for (int i = count - 1; i >= 0; i--) {
        printf (" - %s\n", path[i]);
        free (path[i]);
    }
}

void destroy_tree (leaf_t* leaf) {
    if (leaf == NULL) return;

    destroy_tree (leaf->positive);
    destroy_tree (leaf->negative);
    free ((void*)leaf->text);
    free (leaf);
}

char choose_mode (void) {
    char chosen_mode = 'm';

    cool_print (MY_YELLOW "Choose mode of work:\n" NO_COLOR
            " - play in terminal with opportunity\n"
            "   to update base" MY_YELLOW " (t)\n" NO_COLOR
            " - play in raylib graphic window" MY_YELLOW " (r)\n" NO_COLOR
            " - find out the definition of word in base" MY_YELLOW " (d)\n" NO_COLOR, NORM_TEMP);
    cool_print (MY_YELLOW "Make your choice: " NO_COLOR, NORM_TEMP);

    if (scanf ("%c", &chosen_mode) == 1) {
        printf ("\n");
        return chosen_mode;
    }

    return 'e';
}

void start_game_in_terminal (void) {
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

    start_game (&tree);

    FILE* check_out = fopen ("check_out.txt", "w");
    int rank = 0;
    print_tree (tree.root, check_out, &rank);
    fclose (check_out);

    dump_tree  (&tree, "akinator.dot");
    destroy_tree (tree.root);
}

void start_definition (void) {
    FILE* def_in = fopen ("tree_data.txt", "r");
    if (def_in != NULL) {
        cool_print (MY_PURPLE "Of what word do you want to get definition?\n" NO_COLOR, NORM_TEMP);
        start_definition (def_in);
        fclose (def_in);
    }
}
