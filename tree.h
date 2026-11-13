#ifndef _TREE_H_
#define _TREE_H_

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define NO_COLOR "\033[0m"
#define RED      "\033[1;31m"
#define YELLOW   "\033[1;33m"
#define BLUE     "\033[1;34m"
#define PURPLE   "\033[35m"
#define GREEN    "\033[1;32m"

struct leaf_t {
    const char* text;
    leaf_t*     positive;
    leaf_t*     negative;
    leaf_t*     parent;
};

struct tree_t {
    leaf_t* root;
    leaf_t* current_leaf;
    size_t  size_of_tree; //TODO глубина хз
};

struct buffer {
    const char* data;
    size_t position;
};

enum tree_error_t {
    TREE_OK = 0,
    TREE_ERROR_WITH_ALLOCATION = 1
};

enum slow_print {TOO_SLOW_TEMP = 70, SLOW_TEMP = 50, DED_TEMP = 33, NORM_TEMP = 20};

tree_error_t tree_ctor        (tree_t* tree, const char* value_of_root);
int tree_verify               (tree_t* tree, const char* function_name, int line);
leaf_t* create_leaf           (leaf_t* leaf);
void create_children          (leaf_t* leaf);
void init_first_question      (tree_t* tree);
leaf_t* read_data_from_file   (FILE* in);
leaf_t* read_data_from_buffer (buffer* buf);
void buffer_read_word         (buffer* buf, char* text, size_t max_len);
int get_char_from_buffer      (buffer* buf);
void return_back_in_buffer    (int c, buffer* buf);
int skip_spaces_in_buffer     (buffer* buf);
size_t tree_size              (const leaf_t* leaf);
tree_error_t start_game       (tree_t* tree);
void print_tree               (leaf_t* leaf, FILE* out, int* rank);
void update_base              (leaf_t* leaf, FILE* out);
void print_tree_error_context (tree_t* tree, const char* func, int line);
void cool_print               (const char* out_text, int delay);
void dump_node                (const leaf_t* leaf, FILE* out, size_t* id);
void dump_tree                (const tree_t* tree, const char* filename);

//TODO destroy
#define TREE_ASSERT(list) do {                                                     \
    int _n = tree_verify (tree, __FUNCTION__, __LINE__);                           \
    if (_n != 0) {                                                                 \
        fprintf (stderr, RED "LIST_ASSERT failed (%d errors) at %s:%d\n" NO_COLOR, \
                _n, __FILE__, __LINE__);                                           \
        abort();                                                                   \
    }                                                                              \
} while (0)

#endif
