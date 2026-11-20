#include "../include/tree.h"

tree_error_t tree_ctor (tree_t* tree, const char* value_of_root) {
    assert (tree != NULL);

    tree->root = (node_t*)calloc (1, sizeof (node_t));
    if (tree->root == NULL) return TREE_ERROR_WITH_ALLOCATION;
    tree->size_of_tree   = 1;
    (tree->root)->text   = value_of_root;
    (tree->root)->negative  = NULL;
    (tree->root)->positive   = NULL;
    (tree->root)->parent = tree->root;

    TREE_ASSERT (tree);
    return TREE_OK;
}

node_t* create_node (node_t* node) {
    node = (node_t*)calloc (1, sizeof (node_t));
    return node;
}

void create_children (node_t* node) {
    assert (node != NULL);

    node->negative = create_node (node->negative);
    (node->negative)->parent = node;
    node->positive = create_node (node->positive);
    (node->positive)->parent = node;
}

void init_first_question (tree_t* tree) {
    TREE_ASSERT (tree);

    create_children (tree->root);
    ((tree->root)->negative)->text = "poltorashka";
    ((tree->root)->positive)->text = "meee";

    return;
}

node_t* read_data_from_file (FILE* in) {
    assert (in != NULL);

    fseek (in, 0, SEEK_END);
    size_t file_size = ftell (in);
    fseek (in, 0, SEEK_SET);

    char* file_info = (char*)calloc (file_size + 1, sizeof (char));
    if (file_info == NULL) return NULL;

    size_t read_bytes = fread (file_info, 1, file_size, in);
    file_info[read_bytes] = '\0';

    buffer_t buf = { .data = file_info, .position = 0};

    node_t* root = read_data_from_buffer (&buf);

    free (file_info);

    return root;
}

node_t* read_data_from_buffer (buffer_t* buf) {
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

    node_t* node = (node_t*)calloc (1, sizeof (node_t));
    if (node == NULL) return NULL;

    node->text = strdup (text);
    node->positive = read_data_from_buffer (buf);
    node->negative = read_data_from_buffer (buf);

    if (skip_spaces_in_buffer (buf) == ')') get_char_from_buffer (buf);

    if (node->positive) (node->positive)->parent = node;
    if (node->negative) (node->negative)->parent = node;

    return node;
}

void buffer_read_word (buffer_t* buf, char* text, size_t max_len) {
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

int get_char_from_buffer (buffer_t* buf) {
    assert (buf != NULL);

    if (buf->data[buf->position] == '\0') return EOF;

    return buf->data[buf->position++];
}

void return_back_in_buffer (int c, buffer_t* buf) {
    assert (buf != NULL);

    if (c != EOF && buf->position > 0) buf->position--;
}

int skip_spaces_in_buffer (buffer_t* buf) {
    assert (buf != NULL);

    int c = 0;
    while ((c = get_char_from_buffer (buf)) != EOF && isspace (c)) {}
    if (c != EOF) return_back_in_buffer (c, buf);

    return c;
}

size_t tree_size (const node_t* node) {
    if (node == NULL) return 0;
    return 1 + tree_size (node->positive) + tree_size (node->negative);
}

tree_error_t start_game (tree_t* tree) {
    TREE_ASSERT (tree);

    tree->current_node = tree->root;
    cool_print (MY_PURPLE "Hello, my dear user. \nLet's play with my akinator\n" NO_COLOR, NORM_TEMP);

    char answer[4] = {};
    while ((tree->current_node)->positive != NULL) {
        cool_print (MY_PURPLE "\nEto ", DED_TEMP);
        printf ("%s?" NO_COLOR " (" MY_GREEN "yes" NO_COLOR "/" MY_RED "no" NO_COLOR ")\n", (tree->current_node)->text);
        scanf ("%s", answer);
        if (strcmp (answer, "yes") == 0)     tree->current_node = (tree->current_node)->positive;
        else if (strcmp (answer, "no") == 0) tree->current_node = (tree->current_node)->negative;
        else {
            printf (MY_YELLOW "\nStupid gamer, you wrote something else\n" NO_COLOR);
            return STUPID_USER;
        }
    }

    char expected_answer[24] = {};
    if (strcmp (answer, "yes") == 0) {
        cool_print (MY_YELLOW"\nThis is ", NORM_TEMP);
        printf ("%s\n\n" NO_COLOR, ((tree->current_node)->text));
        strcpy (expected_answer, ((tree->current_node)->text));
        cool_print (MY_PURPLE "Is this answer correct?\n" NO_COLOR, NORM_TEMP);
    }
    else if (strcmp (answer, "no") == 0) {
        cool_print (MY_YELLOW"\nThis is ", NORM_TEMP);
        printf ("%s\n\n" NO_COLOR, ((tree->current_node)->text));
        strcpy (expected_answer, ((tree->current_node)->text));
        cool_print (MY_PURPLE "Is this answer correct?\n" NO_COLOR, NORM_TEMP);
    }
    else {
        printf (MY_YELLOW "\nStupid gamer, you wrote something else\n" NO_COLOR);
        return STUPID_USER;
    }

    char check_answer[4] = {};
    scanf ("%s", check_answer);
    if (strcmp (check_answer, "yes") == 0) cool_print (MY_PURPLE "\nI'm happy that it's true, thank you for using me\n" NO_COLOR, NORM_TEMP);
    else {
        cool_print (MY_PURPLE "\nWhat did you actually wish for?\n" NO_COLOR, NORM_TEMP);
        char real_answer[24] = {};
        scanf ("%s", real_answer);
        char particularity[24] = {};
        cool_print (MY_PURPLE "\nAnd how is ", NORM_TEMP);
        printf ("%s", real_answer);
        cool_print (" different from ", NORM_TEMP);
        printf ("%s?\n" NO_COLOR, expected_answer);
        cool_print (MY_PURPLE "She/He/It is ... " NO_COLOR, NORM_TEMP);
        scanf ("%s", particularity);

        create_children (tree->current_node);
        (tree->current_node)->text = particularity;
        ((tree->current_node)->positive)->text = real_answer;
        ((tree->current_node)->negative)->text = expected_answer;

        FILE* out = fopen ("data/tree_data.txt", "w");
        update_base (tree->root, out);
    }

    return TREE_OK;
}

void print_tree (node_t* node, FILE* out, int* rank) {
    if (node) {
        fprintf (out, "\n");
        for (int i = 0; i < *rank; i ++) {
            fprintf (out, "    ");
        }
        fprintf (out, "( %s ", node->text);
    }

    else fprintf (out, "( %s ", node->text);
    (*rank)++;

    if (node->positive) print_tree (node->positive, out, rank);
    if (node->negative) print_tree (node->negative, out, rank);
    (*rank)--;

    if (node->positive) {
        fprintf (out, "\n");
            for (int i = 0; i < *rank; i ++) {
                fprintf (out, "    ");
            }
        fprintf (out, ")");
    }

    else fprintf (out, ")");
}

void update_base (node_t* node, FILE* out) {
    fprintf (out, "( %s ", node->text);
    if (node->positive) update_base (node->positive, out);
    if (node->negative) update_base (node->negative, out);
    fprintf (out, " )");
}

int tree_verify (tree_t* tree, const char* function_name, int line) { //TODO verify при загрузке дерева для игры
    if (tree == NULL) return 1;
    int n = 0;

    if (tree->size_of_tree < 1) {
        print_tree_error_context (tree, function_name, line);
        fprintf(stderr, MY_RED "ERROR: size of tree is less than 1, it's abnormal\n" NO_COLOR);
        n++;
    }

    int check_children_value = 0;
    if (check_children (tree->root, &check_children_value) != TREE_OK) {
        print_tree_error_context (tree, function_name, line);
        fprintf(stderr, MY_RED "ERROR: in your tree there is a node with only one child\n" NO_COLOR);
        n++;
    }

    return n;
}

tree_error_t check_children (node_t* node, int* n) {
    assert (node != NULL);

    int r = 0;
    if (node->negative != NULL) r++;
    if (node->positive != NULL) r++;

    (*n) += r;
    if ((*n) % 2 != 0) return ERROR_CHILDREN;
    else {
        if (node->negative) check_children (node->negative, n);
        if (node->positive) check_children (node->positive, n);
    }

    return TREE_OK;
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

void dump_node (const node_t* node, FILE* out, size_t* id) {
    if (node == NULL) return;

    size_t my_id = (*id)++;
    fprintf (out, "    n%zu [label=\"%s\", color=\"#3c84a7\"];\n", my_id, node->text);

    if (node->positive) {
        size_t pos_child_id = *id;
        fprintf (out, "    n%zu -> n%zu [label=\"yes\", color=\"#24e216\"];\n", my_id, pos_child_id);
        dump_node (node->positive, out, id);
    }

    if (node->negative) {
        size_t neg_child_id = *id;
        fprintf (out, "    n%zu -> n%zu [label=\"no\", color=\"#D32F2F\"];\n", my_id, neg_child_id);
        dump_node (node->negative, out, id);
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

    buffer_t buf = { .data = buffer_for_def, .position = 0 };
    node_t* root = read_data_from_buffer (&buf);
    if (root == NULL) {
        free (buffer_for_def);
        return;
    }

    node_t* target = find_character (root, word_to_define);
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

node_t* find_character (node_t* node, const char* name) {
    if (node == NULL) return NULL;

    if (node->positive == NULL && node->negative == NULL) {
        if (strcmp (node->text, name) == 0) return node;

        else return NULL;
    }

    node_t* found = find_character (node->positive, name);
    if (found != NULL) return found;

    found = find_character (node->negative, name);
    if (found != NULL) return found;

    return NULL;
}

char** get_character_buffer (node_t* target, const char* name, int* count, char** path) {
    assert (name   != NULL);
    assert (target != NULL);

    *count = 0;

    node_t* current = target;
    while (current->parent != NULL && current->parent != current) {
        node_t* parent = current->parent;
        int is_yes = (parent->positive == current);

        size_t len = strlen (parent->text) + 10;
        path[*count] = (char*)calloc (len, sizeof (char));

        if (path[*count] == NULL) break;
        if (is_yes == 1) {
            snprintf (path[*count], len, "%s", parent->text);
        }

        else snprintf (path[*count], len, "not %s", parent->text);

        (*count)++;

        current = parent;
    }

    return path;
}

void print_character_features (node_t* target, const char* name) {
    assert (target != NULL);
    assert (name   != NULL);

    int count = 0;
    char* path[64] = {};
    get_character_buffer (target, name, &count, path);

    cool_print (MY_YELLOW "\nDefinition\n" NO_COLOR, NORM_TEMP);
    printf ("%s:\n" NO_COLOR, name);

    for (int i = count - 1; i >= 0; i--) {
        printf (" - %s\n", path[i]);
        free (path[i]);
    }
}

void compare_two_elements (FILE* com_in) {
    assert (com_in != NULL);

    cool_print (MY_PURPLE "What 2 things/people do you want to compare?\n" NO_COLOR, NORM_TEMP);
    cool_print (MY_PURPLE "1 word: " NO_COLOR, NORM_TEMP);

    char word_to_define_first[64] = {};
    node_t* target_first = {};
    get_definition (word_to_define_first, &target_first, com_in);

    char* path_first[64] = {};
    int count_first = 0;
    get_character_buffer (target_first, word_to_define_first, &count_first, path_first);

    cool_print (MY_PURPLE "2 word: " NO_COLOR, NORM_TEMP);

    char word_to_define_second[64] = {};
    node_t* target_second = {};
    get_definition (word_to_define_second, &target_second, com_in);

    char* path_second[64] = {};
    int count_second = 0;
    get_character_buffer (target_second, word_to_define_second, &count_second, path_second);

    comparison (path_first, count_first, path_second, count_second);
}

void get_definition (char* word_to_define, node_t** target, FILE* com_in) {
    assert (word_to_define != NULL);

    if (scanf ("%63s", word_to_define) != 1) return;

    char* buffer_for_def = get_buffer_with_info_from_file (com_in);
    if (buffer_for_def == NULL) return;

    buffer_t buf = { .data = buffer_for_def, .position = 0 };
    node_t* root = read_data_from_buffer (&buf);

    if (root == NULL) {
        free (buffer_for_def);
        return;
    }

    *target = find_character (root, word_to_define);
    if (*target == NULL) {
        cool_print (MY_RED "Not found in base: " NO_COLOR, NORM_TEMP);
        printf ("%s\n", word_to_define);
    }
}

void comparison (char** path_first, int count_first, char** path_second, int count_second) {
    assert (path_first  != NULL);
    assert (path_second != NULL);

    int i = count_first - 1;
    int j = count_second - 1;

    while (i >= 0 && j >= 0 && strcmp (path_first[i], path_second[j]) == 0) {
        i--;
        j--;
    }

    cool_print (MY_PURPLE "\nSame characteristics:\n" NO_COLOR, NORM_TEMP);
    int sovpalo = 0;
    for (int k = i + 1; k < count_first; k++) {
        printf (" - %s\n", path_first[k]);
        free (path_first[k]);
        sovpalo++;
        path_first[k] = NULL;
    }
    if (sovpalo == 0) printf ("0\n");

    for (int k = j + 1; k < count_second; k++) {
        free (path_second[k]);
        path_second[k] = NULL;
    }

    cool_print (MY_PURPLE "But first: \n" NO_COLOR, NORM_TEMP);
    int first = 0;
    for (int k = i; k >= 0; k--) {
        printf (" - %s\n", path_first[k]);
        free (path_first[k]);
        first++;
        path_first[k] = NULL;
    }
    if (first == 0) printf (" - nothing else\n");

    cool_print (MY_PURPLE "And second: \n" NO_COLOR, NORM_TEMP);
    int second = 0;
    for (int k = j; k >= 0; k--) {
        printf (" - %s\n", path_second[k]);
        free (path_second[k]);
        second++;
        path_first[k] = NULL;
    }
    if (second == 0) printf (" - nothing else\n");
}

void destroy_tree (node_t* node) {
    if (node == NULL) return;

    destroy_tree (node->positive);
    destroy_tree (node->negative);
    node->text = NULL;
    free (node);
}

char choose_mode (void) {
    char chosen_mode = 'm';

    cool_print (MY_YELLOW "Choose mode of work:\n" NO_COLOR
            " - play in terminal with opportunity\n"
            "   to update base" MY_YELLOW " (t)\n" NO_COLOR
            " - play in raylib graphic window" MY_YELLOW " (r)\n" NO_COLOR
            " - find out the definition of word in base" MY_YELLOW " (d)\n" NO_COLOR
            " - compare two elements from base" MY_YELLOW " (c)\n" NO_COLOR, NORM_TEMP);
    cool_print (MY_YELLOW "Make your choice: " NO_COLOR, NORM_TEMP);

    if (scanf ("%c", &chosen_mode) == 1) {
        printf ("\n");
        return chosen_mode;
    }

    return 'e';
}

void start_game_in_terminal (void) {
    tree_t tree = {};

    FILE* in = fopen ("data/tree_data.txt", "r");
    if (in != NULL) {
        tree.root = read_data_from_file (in);
        tree.size_of_tree = tree_size (tree.root);
        TREE_ASSERT (&tree);
        fclose (in);
    }

    else {
        tree_ctor (&tree, "chelovek");
        init_first_question (&tree);
        TREE_ASSERT (&tree);
    }

    start_game (&tree);

    FILE* check_out = fopen ("data/check_out.txt", "w");
    int rank = 0;
    print_tree (tree.root, check_out, &rank);
    fclose (check_out);

    dump_tree  (&tree, "data/akinator.dot");
    destroy_tree (tree.root);
}

void start_definition (void) {
    FILE* def_in = fopen ("data/tree_data.txt", "r");
    if (def_in != NULL) {
        cool_print (MY_PURPLE "Of what word do you want to get definition?\n" NO_COLOR, NORM_TEMP);
        start_definition (def_in);
        fclose (def_in);
    }
}

void start_comparison (void) {
    FILE* com_in = fopen ("data/tree_data.txt", "r");
    if (com_in != NULL) {
        compare_two_elements (com_in);
        fclose (com_in);
    }
}
