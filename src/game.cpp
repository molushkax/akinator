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

    bool answer_sound_played = false;

    while (!WindowShouldClose ()) {
        UpdateMusicStream (dinastiya);

        mouse.mouse_pos = GetMousePosition ();
        mouse.is_down = IsMouseButtonDown (MOUSE_BUTTON_LEFT);
        mouse.is_released = IsMouseButtonReleased (MOUSE_BUTTON_LEFT);
    
        BeginDrawing ();
        ClearBackground (RAYWHITE);
        DrawTexture (background, 0, 0, WHITE);
        
        node_t* current = tree->current_node;
        bool is_leaf = (current->positive == NULL && current->negative == NULL);

        draw_button (&btn_restart, mouse.mouse_pos, mouse.is_down, 15);
        draw_button (&btn_exit,    mouse.mouse_pos, mouse.is_down, 15);

        float volume = update_and_draw_slider (&volume_slider, mouse.mouse_pos, mouse.is_down, mouse.is_released);
        int volume_in_procents = (int)(volume * 100);
        const char* text_pro = TextFormat ("SOUND: %d %%", volume_in_procents);
        DrawText (text_pro, 551, 87, 14, BLACK);

        if (update_button (&btn_restart, mouse.mouse_pos, mouse.is_released)) {
            tree->current_node = tree->root;
            current  = tree->current_node;
            StopSound (answer);
            answer_sound_played = true;
        }

        if (update_button (&btn_exit, mouse.mouse_pos, mouse.is_released)) {
            CloseWindow ();
        }

        if (!is_leaf) {
            const char* text = TextFormat ("Is it %s?", current->text); //TODO озвучка звук
            int text_width = MeasureText (text, 70);

            DrawText (text, (800 - text_width) / 2, 330, 50, DARKGRAY);

            draw_button (&btn_yes, mouse.mouse_pos, mouse.is_down, 40);
            draw_button (&btn_no,  mouse.mouse_pos, mouse.is_down, 40);

            if (update_button (&btn_yes, mouse.mouse_pos, mouse.is_released)) {
                tree->current_node = current->positive;
            }

            if (update_button (&btn_no, mouse.mouse_pos, mouse.is_released)) {
                tree->current_node = current->negative;
            } 
        }

        else {
            const char* text = TextFormat ("I think it's %s", current->text);
            int text_width = MeasureText (text, 55);
            DrawText (text, (800 - text_width) / 2, 370, 40, DARKGRAY);

            if (!answer_sound_played) {
                PlaySound (answer);
                answer_sound_played = true;
            }
        }

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