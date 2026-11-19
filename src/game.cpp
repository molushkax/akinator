#include "raylib.h"
#include "../include/tree.h"
#include "../include/game.h"

tree_t* load_game_tree (const char* filename) {
    assert (filename != NULL);

    FILE* in = fopen (filename, "r");
    if (in == NULL) return NULL;

    tree_t* tree = (tree_t*)calloc (1, sizeof (tree_t));
    if (tree == NULL) return NULL;

    tree->root = read_data_from_file (in);
    if (tree->root != NULL) {
        tree->size_of_tree = tree_size (tree->root);
        tree->current_node = tree->root;
        TREE_ASSERT (tree);
    }

    fclose (in);

    return tree;
}

void start_game_graphics (void) { //TODO error return
    tree_t* tree = load_game_tree ("data/tree_data.txt");
    TREE_ASSERT (tree);
    if (tree == NULL || tree->root == NULL) {
        printf (MY_RED "ERROR: cannot load tree_data.txt\n" NO_COLOR);
        return;
    }

    InitWindow (655, 850, "my lovely akinator");
    InitAudioDevice ();

    Texture2D background = LoadTexture ("data/peppa.png");
    Sound answer = LoadSound ("data/true_answer.mp3");
    Music dinastiya = LoadMusicStream ("data/dinastiya.mp3");
    PlayMusicStream (dinastiya);

    SetTargetFPS (60);

    mouse_t mouse = {};

    button_t btn_letsgo = { 
        .rect = { 235, 470, 200, 60 },
        .text = "LET'S GO",
        .base_color = GRAY, .hovered_color = PURPLE, .pressed_color = DARKPURPLE,
        .is_down_on_button = false
    };
    
    button_t btn_yes = { 
        .rect = { 100, 410, 200, 60 },
        .text = "YES",
        .base_color = GRAY, .hovered_color = GREEN, .pressed_color = DARKGREEN,
        .is_down_on_button = false
    };
    
    button_t btn_no = { 
        .rect = { 350, 410, 200, 60 },
        .text = "NO",
        .base_color = GRAY, .hovered_color = RED, .pressed_color = MAROON,
        .is_down_on_button = false
    };

    button_t btn_ok = { 
        .rect = { 250, 480, 150, 55 },
        .text = "OK",
        .base_color = GRAY, .hovered_color = GREEN, .pressed_color = DARKGREEN,
        .is_down_on_button = false
    };

    button_t btn_restart = { 
        .rect = { 550, 20, 80, 25 },
        .text = "RESTART",
        .base_color = SKYBLUE, .hovered_color = BLUE, .pressed_color = DARKBLUE,
        .is_down_on_button = false
    };

    button_t btn_exit = { 
        .rect = { 550, 50, 80, 25 },
        .text = "EXIT",
        .base_color = SKYBLUE, .hovered_color = BLUE, .pressed_color = DARKBLUE,
        .is_down_on_button = false
    };

    slider_t volume_slider {
        .slider_track_rect = { 552, 107, 77, 6 },
        .value = 1.0f,
        .is_dragging = false
    };

    text_input_t input_text = {
        .buffer = {0}, .length = 0,
        .text_rect = { 100, 400, 450, 50 },
        .active = false
    };

    bool answer_sound_played = false;
    state_of_game_t state = START_MENU;

    char pending_guess[MAX_INPUT_LEN + 1] = {};

    while (!WindowShouldClose ()) {
        UpdateMusicStream (dinastiya);
        mouse.mouse_pos = GetMousePosition ();
        mouse.is_down = IsMouseButtonDown (MOUSE_BUTTON_LEFT);
        mouse.is_released = IsMouseButtonReleased (MOUSE_BUTTON_LEFT);
    
        BeginDrawing ();
        ClearBackground (RAYWHITE);
        DrawTexture (background, 0, 0, WHITE);

        if (state != START_MENU) {
            draw_button (&btn_restart, mouse.mouse_pos, mouse.is_down, 15);
            draw_button (&btn_exit,    mouse.mouse_pos, mouse.is_down, 15);

            if (update_button (&btn_restart, mouse.mouse_pos, mouse.is_released)) {
                tree->current_node = tree->root;
                StopSound (answer);
                state = START_PLAY;
            }

            if (update_button (&btn_exit, mouse.mouse_pos, mouse.is_released)) {
                state = START_MENU;
            }

            float volume = update_and_draw_slider (&volume_slider, mouse.mouse_pos, mouse.is_down, mouse.is_released);
            int volume_in_procents = (int)(volume * 100);
            const char* text_pro = TextFormat ("SOUND: %d %%", volume_in_procents);
            DrawText (text_pro, 551, 87, 14, BLACK);
        }
        
        switch (state) {
            case START_MENU: {
                draw_menu_screen (&btn_letsgo, mouse);

                if (update_button (&btn_letsgo, mouse.mouse_pos, mouse.is_released)) {
                    state = START_PLAY;
                }

                break;
            }

            case START_PLAY: {
                draw_game_screen (tree, &btn_yes, &btn_no, &btn_restart, &btn_exit,
                                &volume_slider, mouse, answer, &answer_sound_played);

                node_t* current = tree->current_node;
                bool is_leaf = (current->positive == NULL);

                if (is_leaf) {
                    answer_sound_played = false;
                    state = GUESS_CHECK;
                }

                break;
            }

            case GUESS_CHECK: {
                node_t* current = tree->current_node;
                draw_guess_check (current, &btn_yes, &btn_no, mouse);

                if (!answer_sound_played) {
                    PlaySound (answer);
                    answer_sound_played = true;
                }

                if (update_button (&btn_yes, mouse.mouse_pos, mouse.is_released)) {
                    state = WON;
                }

                if (update_button (&btn_no, mouse.mouse_pos, mouse.is_released)) {
                    text_input_clear (&input_text);
                    input_text.active = true;
                    state = ASK_NEW_ITEM;
                }

                break;
            }

            case WON: {
                DrawText ("Yuhuuuuu! I' m so happy!\n"
                          "  Thanks for playing", 80, 380, 40, DARKGRAY);

                break;
            }

            case ASK_NEW_ITEM: {
                DrawText ("Who did you actually wish for?", 15, 340, 40, DARKGRAY);
                draw_input_text (&input_text);

                draw_button (&btn_ok, mouse.mouse_pos, mouse.is_down, 30);
                bool ok = update_button (&btn_ok, mouse.mouse_pos, mouse.is_released);
                bool enter = input_text_submitted ();

                if ((ok || enter) && input_text.length > 0) {
                    strncpy (pending_guess, input_text.buffer, MAX_INPUT_LEN);
                    pending_guess[MAX_INPUT_LEN] = '\0';
                    text_input_clear (&input_text);
                    state = ASK_DIFFERENCE;
                }

                break;
            }

            case ASK_DIFFERENCE: {
                DrawText (TextFormat ("    How is %s different\n" 
                                      "from %s? He/she/it is...", pending_guess, tree->current_node->text),
                                      100, 320, 30, DARKGRAY);
                draw_input_text (&input_text);

                draw_button (&btn_ok, mouse.mouse_pos, mouse.is_down, 30);
                bool ok = update_button (&btn_ok, mouse.mouse_pos, mouse.is_released);
                bool enter = input_text_submitted ();

                if ((ok || enter) && input_text.length > 0) {
                    add_new_item (tree, tree->current_node, pending_guess, input_text.buffer);
                    tree->size_of_tree = tree_size (tree->root);
                    text_input_clear (&input_text);
                    input_text.active = false;
                    tree->current_node = tree->root;
                    state = STATE_ADDED;
                }

                break;
            }

            case STATE_ADDED: {                                        //TODO functions po krasote
                DrawText ("Thank you for your support\n"
                          "         I remembered", 35, 350, 40, DARKGRAY);
                draw_button (&btn_ok, mouse.mouse_pos, mouse.is_down, 30);
                FILE* out = fopen ("data/tree_data.txt", "w");
                update_base (tree->root, out);
                fclose (out);

                if (update_button (&btn_ok, mouse.mouse_pos, mouse.is_released)) {
                    input_text.active = true;
                    state = START_MENU;
                }

                break;
            }

            default: printf ("something strange\n"); break;
        } 
        
        if (input_text.active) update_text_input (&input_text);

        EndDrawing ();
    }

    UnloadMusicStream (dinastiya);
    CloseAudioDevice ();

    UnloadTexture (background);
    CloseWindow ();
    UnloadSound (answer);

    destroy_tree (tree->root);
    free (tree);
}

bool update_button (button_t* btn, Vector2 mouse_pos, bool is_mouse_released) { //проверяет, кликнули ли по кнопке
    assert (btn != NULL);

    bool is_hovered = CheckCollisionPointRec (mouse_pos, btn->rect);
    bool is_pressed = false;

    if (is_hovered && is_mouse_released) is_pressed = true;

    return is_pressed;
}

void draw_button (button_t* btn, Vector2 mouse_pos, bool is_mouse_down, int shrift) {
    assert (btn != NULL);

    bool is_hovered = CheckCollisionPointRec (mouse_pos, btn->rect);

    if (is_hovered && is_mouse_down) {
        DrawRectangleRec (btn->rect, btn->pressed_color);
    }

    else if (is_hovered && !is_mouse_down) {
        DrawRectangleRec (btn->rect, btn->hovered_color);
    }

    if (!is_hovered) {
        DrawRectangleRec (btn->rect, btn->base_color);
    }

    int text_width = MeasureText (btn->text, shrift);
    DrawText (btn->text, (int)(btn->rect.x + ((float)btn->rect.width - (float)text_width) / 2), (int)btn->rect.y + shrift/3, shrift, WHITE);
}

float update_and_draw_slider (slider_t* slider, Vector2 mouse_pos, bool is_mouse_down, bool is_mouse_released) {
    assert (slider != NULL);

    float slider_x = slider->slider_track_rect.x + (slider->slider_track_rect.width*slider->value);
    Vector2 slider_pos = { slider_x, slider->slider_track_rect.y + slider->slider_track_rect.height / 2 };

    float radius_kruga_slidera = 7;

    bool is_hovered = CheckCollisionPointCircle (mouse_pos, slider_pos, radius_kruga_slidera) ||
                      CheckCollisionPointRec    (mouse_pos, (Rectangle){slider->slider_track_rect.x, slider->slider_track_rect.y - 10,
                                                 slider->slider_track_rect.width, 30});
    
    if (is_mouse_down && is_hovered) {
        slider->is_dragging = true;
    }

    if (is_mouse_released) {
        slider->is_dragging = false;
    }

    if (slider->is_dragging) {
        float new_value = (mouse_pos.x - slider->slider_track_rect.x) / slider->slider_track_rect.width;

        if (new_value > 1.0f) {
            slider->value = 1.0f;
        }

        if (new_value < 0.0f) {
            slider->value = 0.0f;
        }

        slider->value = new_value;
        SetMasterVolume (slider->value);
    }

    DrawRectangleRec (slider->slider_track_rect, LIGHTGRAY);
    DrawRectangle ((int)slider->slider_track_rect.x, (int)slider->slider_track_rect.y,
                   (int)(slider->slider_track_rect.width * slider->value), (int)slider->slider_track_rect.height, BLACK);
    DrawCircleV (slider_pos, radius_kruga_slidera, slider->is_dragging ? DARKBLUE : BLUE);

    return slider->value;
}

void draw_menu_screen (button_t* btn, mouse_t mouse) {
    assert (btn != NULL);

    const char* text_hello = TextFormat ("   Hello, my dear user!\n" 
                                         "Thank you for choosing\n"
                                         "   mashka's akinator");
    DrawText (text_hello, 90, 320, 40, DARKGRAY);
    draw_button (btn, mouse.mouse_pos, mouse.is_down, 40);
}

void draw_game_screen (tree_t* tree, button_t* btn_yes, button_t* btn_no, button_t* btn_restart, button_t* btn_exit, 
                       slider_t* volume_slider, mouse_t mouse, Sound answer, bool* answer_sound_played) {
    assert (tree    != NULL);
    assert (btn_yes != NULL);
    assert (btn_no  != NULL);
    assert (btn_restart != NULL);
    assert (btn_exit != NULL);
    assert (volume_slider != NULL);
    assert (answer_sound_played != NULL);

    node_t* current = tree->current_node;
    bool is_leaf = (current->positive == NULL && current->negative == NULL);

    draw_button (btn_restart, mouse.mouse_pos, mouse.is_down, 15);
    draw_button (btn_exit,    mouse.mouse_pos, mouse.is_down, 15);

    if (update_button (btn_restart, mouse.mouse_pos, mouse.is_released)) {
        tree->current_node = tree->root;
        current  = tree->current_node;
        StopSound (answer);
        *answer_sound_played = true;
    }

    if (!is_leaf) {
        const char* text = TextFormat ("Is it %s?", current->text); //TODO озвучка
        int text_width = MeasureText (text, 70);

        DrawText (text, (800 - text_width) / 2, 330, 50, DARKGRAY);

        draw_button (btn_yes, mouse.mouse_pos, mouse.is_down, 40);
        draw_button (btn_no,  mouse.mouse_pos, mouse.is_down, 40);

        if (update_button (btn_yes, mouse.mouse_pos, mouse.is_released)) {
            tree->current_node = current->positive;
        }

        if (update_button (btn_no, mouse.mouse_pos, mouse.is_released)) {
            tree->current_node = current->negative;
        } 
    }
}

void update_text_input (text_input_t* text) {
    assert (text != NULL);

    if (!text->active) return;

    int code_of_char = GetCharPressed ();

    while (code_of_char > 0) {
        if (code_of_char >= 32 && code_of_char <= 125 && text->length < MAX_INPUT_LEN) {
            text->buffer[text->length++] = (char)code_of_char;
            text->buffer[text->length]   = '\0';
        }
        code_of_char = GetCharPressed ();
    }

    if (IsKeyPressed (KEY_BACKSPACE) && text->length > 0) {
        text->length--;
        text->buffer[text->length] = '\0';
    }
}

bool input_text_submitted (void) {
    return IsKeyPressed (KEY_ENTER) || IsKeyPressed (KEY_KP_ENTER);
}

void draw_input_text (text_input_t* text) {
    assert (text != NULL);

    DrawRectangleRec (text->text_rect, text->active ? LIGHTGRAY : RAYWHITE);
    DrawRectangleLinesEx  (text->text_rect, 2, text->active ? BLUE : DARKGRAY);
    DrawText (text->buffer[0] ? text->buffer : "type here...",
              (int)text->text_rect.x + 8, (int)text->text_rect.y + 8, 30,
              text->buffer[0] ? BLACK : GRAY);

    int padding = 8;
    int font_size = 30;
    int text_x = (int)text->text_rect.x + padding;
    int text_y = (int)text->text_rect.y + padding;

    const char* shown = text->buffer[0] ? text->buffer : "type here...";
    Color color = text->buffer[0] ? BLACK : GRAY;
    DrawText (shown, text_x, text_y, font_size, color);

    if (text->active) {
        float blink = fmodf ((float)GetTime (), 1.0f);
        if (blink < 0.5f) {
            int typed_width = MeasureText (text->buffer, font_size);
            int caret_x = text_x + typed_width;
            int caret_top = text_y;
            int caret_bottom = text_y + font_size;
            DrawLine (caret_x, caret_top, caret_x, caret_bottom, BLACK);
        }
    }
}

void text_input_clear (text_input_t* text) {
    assert (text != NULL);

    text->buffer[0] = '\0';
    text->length    = 0;
}

node_t* make_leaf (const char* text) {
    assert (text != NULL);

    node_t* n = (node_t*)calloc (1, sizeof (node_t));
    n->text = strdup (text);
    n->positive = NULL;
    n->negative = NULL;
    return n;
}

void add_new_item (tree_t* tree, node_t* current, const char* guess, const char* difference) {
    assert (tree != NULL);
    assert (current != NULL);
    assert (guess != NULL);
    assert (difference != NULL);

    const char* old_text = current->text;

    node_t* new_yes = make_leaf (guess);
    node_t* new_no  = make_leaf (old_text);

    current->text     = strdup (difference);
    current->positive = new_yes;
    current->negative = new_no;
}

void draw_guess_check (node_t* current, button_t* btn_yes, button_t* btn_no, mouse_t mouse) {
    assert (current != NULL);
    assert (btn_yes != NULL);
    assert (btn_no  != NULL);

    const char* text = TextFormat ("I think it/he/she is %s.\n"
                                   "Am I right?", current->text);
    int text_width = MeasureText (text, 40);
    DrawText (text, (655 - text_width) / 2, 310, 40, DARKGRAY);

    draw_button (btn_yes, mouse.mouse_pos, mouse.is_down, 40);
    draw_button (btn_no,  mouse.mouse_pos, mouse.is_down, 40);
}