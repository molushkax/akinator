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

    if (skip_spaces (in) != '(') return NULL;
    fgetc (in);

    char text[128] = {};
    if (fscanf(in, "%127s", text) != 1) return NULL;

    size_t len = strlen (text);
    if (len > 0 && (text[len - 1] == '(' || text[len - 1] == ')')) {
        char bracket = text[len - 1];
        text[len - 1] = '\0';
        ungetc (bracket, in);

        if (len == 1) return NULL;
    }

    leaf_t* leaf = (leaf_t*)calloc (1, sizeof (leaf_t));
    if (leaf == NULL) return NULL;

    leaf->text = strdup (text);
    leaf->positive = read_data_from_file (in);
    leaf->negative = read_data_from_file (in);

    if (skip_spaces (in) == ')') fgetc (in);

    if (leaf->positive) (leaf->positive)->parent = leaf;
    if (leaf->negative) (leaf->negative)->parent = leaf;

    return leaf;
}

int skip_spaces (FILE* in) {
    assert (in != NULL);

    int c = 0;
    while ((c = fgetc (in)) != EOF && isspace (c)) {}
    if (c != EOF) ungetc (c, in);
    return c;
}

size_t tree_size (const leaf_t* leaf) {
    if (leaf == NULL) return 0;
    return 1 + tree_size (leaf->positive) + tree_size (leaf->negative);
}

tree_error_t start_game (tree_t* tree) {
    TREE_ASSERT (tree);

    tree->current_leaf = tree->root;

    char answer[4] = {};
    while ((tree->current_leaf)->positive != NULL) {
        printf ("\n%s?\n", (tree->current_leaf)->text);
        scanf ("%s", answer);
        if (strcmp (answer, "yes") == 0) tree->current_leaf = (tree->current_leaf)->positive;
        else tree->current_leaf = (tree->current_leaf)->negative;
    }

    char expected_answer[24] = {};
    if (strcmp (answer, "yes") == 0) {
        cool_print ("This is ", NORM_TEMP);
        printf ("%s\n", ((tree->current_leaf)->text));
        strcpy (expected_answer, ((tree->current_leaf)->text));
    }
    else {
        printf ("This is %s\n", ((tree->current_leaf)->text));
        strcpy (expected_answer, ((tree->current_leaf)->text));
    }
    cool_print ("Is this answer correct?\n", NORM_TEMP);

    char check_answer[4] = {};
    scanf ("%s", check_answer);
    if (strcmp (check_answer, "yes") == 0) cool_print ("I'm happy that it's true, thank you for using me\n", NORM_TEMP);
    else {
        cool_print ("What did you actually wish for?\n", NORM_TEMP);
        char real_answer[24] = {};
        scanf ("%s", real_answer);
        char particularity[24] = {};
        cool_print ("And how is ", NORM_TEMP);
        printf ("%s", real_answer);
        cool_print (" different from ", NORM_TEMP);
        printf ("%s?\n", expected_answer);
        cool_print ("It's ... ", NORM_TEMP);
        scanf ("%s", particularity);
        create_children (tree->current_leaf);
        (tree->current_leaf)->text = particularity;
        ((tree->current_leaf)->positive)->text = real_answer;
        ((tree->current_leaf)->negative)->text = expected_answer;
        FILE* out = fopen ("tree_data.txt", "w");
        print_tree (tree->root, out);
    }

    return TREE_OK;
}

void print_tree (leaf_t* leaf, FILE* out) {
    fprintf (out, "( %s ", leaf->text);
    if (leaf->positive) print_tree (leaf->positive, out);
    if (leaf->negative) print_tree (leaf->negative, out);
    fprintf (out, " )");
}

int tree_verify (tree_t* tree, const char* function_name, int line) {
    if (tree == NULL) return 1;
    int n = 0;

    if (tree->size_of_tree < 1) {
        print_tree_error_context (tree, function_name, line);
        fprintf(stderr, RED "ERROR: size of tree is less than 1, it's abnormal\n" NO_COLOR);
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
        Sleep (delay);
    }
}
