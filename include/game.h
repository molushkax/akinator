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

tree_t* load_game_tree   (const char* filename);
void start_game_graphics (void);
bool update_button       (button_t* btn, Vector2 mouse_pos, bool is_mouse_released);
void draw_button         (button_t* btn, Vector2 mouse_pos, bool is_mouse_down);

#endif
