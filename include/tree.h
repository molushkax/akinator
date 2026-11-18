#ifndef _TREE_H_
#define _TREE_H_

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "raylib.h"

#define NO_COLOR    "\033[0m"
#define MY_RED      "\033[1;31m"
#define MY_YELLOW   "\033[1;33m"
#define MY_BLUE     "\033[1;34m"
#define MY_PURPLE   "\033[35m"
#define MY_GREEN    "\033[1;32m"

const char TERMINAL   = 't';
const char GRAPHICS   = 'r';
const char DEFINITION = 'd';
const char COMPARISON = 'c';

struct node_t {
    const char* text;
    node_t*     positive;
    node_t*     negative;
    node_t*     parent;
};

struct tree_t {
    node_t* root;
    node_t* current_node;
    size_t  size_of_tree;
};

struct buffer_t {
    const char* data;
    size_t position;
};

enum tree_error_t {
    TREE_OK                    = 0,
    TREE_ERROR_WITH_ALLOCATION = 1,
    STUPID_USER                = 2,
    ERROR_CHILDREN             = 3
};

enum slow_print {TOO_SLOW_TEMP = 70, SLOW_TEMP = 50, DED_TEMP = 33, NORM_TEMP = 0};

tree_error_t tree_ctor        (tree_t* tree, const char* value_of_root);
int tree_verify               (tree_t* tree, const char* function_name, int line);
tree_error_t check_children   (node_t* node, int* n);
node_t* create_node           (node_t* node);
void create_children          (node_t* node);
void init_first_question      (tree_t* tree);
node_t* read_data_from_file   (FILE* in);
node_t* read_data_from_buffer (buffer_t* buf);
void buffer_read_word         (buffer_t* buf, char* text, size_t max_len);
int get_char_from_buffer      (buffer_t* buf);
void return_back_in_buffer    (int c, buffer_t* buf);
int skip_spaces_in_buffer     (buffer_t* buf);
size_t tree_size              (const node_t* node);
tree_error_t start_game       (tree_t* tree);
char* get_buffer_with_info_from_file (FILE* in);
void print_tree               (node_t* node, FILE* out, int* rank);
void update_base              (node_t* node, FILE* out);
void print_tree_error_context (tree_t* tree, const char* func, int line);
void cool_print               (const char* out_text, int delay);
void dump_node                (const node_t* node, FILE* out, size_t* id);
void dump_tree                (const tree_t* tree, const char* filename);
void start_definition         (FILE* tree_data);
node_t* find_character        (node_t* node, const char* name);
char** get_character_buffer   (node_t* target, const char* name, int* count, char** path);
void print_character_features (node_t* target, const char* name);
void compare_two_elements     (FILE* com_in);
void get_definition           (char* word_to_define, node_t** target, FILE* com_in);
void comparison               (char** path_first, int count_first, char** path_second, int count_second);
void destroy_tree             (node_t* node);
char choose_mode              (void);
void start_game_in_terminal   (void);
void start_definition         (void);
void start_comparison         (void);

#define TREE_ASSERT(tree) do {                                                        \
    int _n = tree_verify (tree, __FUNCTION__, __LINE__);                              \
    if (_n != 0) {                                                                    \
        fprintf (stderr, MY_RED "LIST_ASSERT failed (%d errors) at %s:%d\n" NO_COLOR, \
                _n, __FILE__, __LINE__);                                              \
        abort();                                                                      \
    }                                                                                 \
} while (0)

#endif
