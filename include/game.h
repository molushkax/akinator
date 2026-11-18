#ifndef _GAME_H_
#define _GAME_H_

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

enum state_of_game_t {
    START_MENU,
    START_PLAY
};

tree_t* load_game_tree   (const char* filename);
void start_game_graphics (void);
bool update_button       (button_t* btn, Vector2 mouse_pos, bool is_mouse_released);
void draw_button         (button_t* btn, Vector2 mouse_pos, bool is_mouse_down, int shrift);
float update_and_draw_slider (slider_t* slider, Vector2 mouse_pos, bool is_mouse_down, bool is_mouse_released);
void draw_menu_screen    (button_t* btn, mouse_t mouse);
void draw_game_screen    (tree_t* tree, button_t* btn_yes, button_t* btn_no, button_t* btn_restart, button_t* btn_exit, 
                          slider_t* volume_slider, mouse_t mouse, Sound answer, bool* answer_sound_play);
#endif
