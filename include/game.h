#ifndef _GAME_H_
#define _GAME_H_

#define MAX_INPUT_LEN 28

struct mouse_t {
    Vector2 mouse_pos;  // положение
    bool is_down;       // зажата ли 
    bool is_pressed;    // нажата ли
    bool is_released;   // отпустили ли, те верно, когда игрок вот только нажал
};

struct button_t {
    Rectangle rect;      // задаем координаты и размер
    const char* text;    // текст на кнопке
    Color base_color;    // цвет в обычном состоянии
    Color hovered_color; // цвет, когда наведена мышка
    Color pressed_color; // цвет, когда нажата 
    bool  is_down_on_button; // наведена ли мышка
};

struct slider_t {
    Rectangle slider_track_rect; // дорожка ползунка
    float value;                // значение громкости
    bool   is_dragging;          // тянет ли юзер ползунок
};

struct text_input_t {
    char buffer[MAX_INPUT_LEN + 1];
    int  length;
    Rectangle text_rect;
    bool active;
};

enum state_of_game_t {
    START_MENU,
    START_PLAY,
    GUESS_CHECK,
    ASK_NEW_ITEM,
    WON,
    ASK_DIFFERENCE,
    STATE_ADDED
};

tree_t* load_game_tree   (const char* filename);
void start_game_graphics (void);
bool update_button       (button_t* btn, Vector2 mouse_pos, bool is_mouse_released);
void draw_button         (button_t* btn, Vector2 mouse_pos, bool is_mouse_down, int shrift);
float update_and_draw_slider (slider_t* slider, Vector2 mouse_pos, bool is_mouse_down, bool is_mouse_released);
void draw_menu_screen    (button_t* btn, mouse_t mouse);
void draw_game_screen    (tree_t* tree, button_t* btn_yes, button_t* btn_no, button_t* btn_restart, button_t* btn_exit, 
                          slider_t* volume_slider, mouse_t mouse, Sound answer, bool* answer_sound_play);
void update_text_input   (text_input_t* text); 
bool input_text_submitted (void);
void draw_input_text     (text_input_t* text);
void text_input_clear    (text_input_t* text);
node_t* make_leaf        (const char* text);
void add_new_item        (tree_t* tree, node_t* current, const char* guess, const char* difference);
void draw_guess_check    (node_t* current, button_t* btn_yes, button_t* btn_no, mouse_t mouse);

#endif
