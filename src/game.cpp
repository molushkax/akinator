#include "raylib.h"
#include "../include/tree.h"
#include "../include/game.h"

tree_t* load_game_tree (const char* filename) {
    assert (filename != NULL);

    FILE* in = fopen (filename, "r");
    if (in == NULL) return NULL;

    tree_t* tree = (tree_t*) calloc (1, sizeof (tree_t));
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

    bool end_game = false;

    while (!WindowShouldClose ()) {
        UpdateMusicStream (dinastiya);

        Vector2 mouse = GetMousePosition ();
        node_t* current = tree->current_node;

        bool is_node = (current->positive == NULL && current->negative == NULL);

        BeginDrawing ();
        ClearBackground (RAYWHITE);
        DrawTexture (background, 0, 0, WHITE);

        if (!end_game) {
            if (!is_node) {
                const char* text = TextFormat ("Is it %s?", current->text); //TODO озвучка звук
                int text_width = MeasureText (text, 70);

                DrawText (text, (800 - text_width) / 2, 330, 50, DARKGRAY);

                Rectangle if_yes_rec = { 100, 410, 200, 60 };
                if (CheckCollisionPointRec (mouse, if_yes_rec)) {
                    DrawRectangleRec (if_yes_rec, GREEN);
                    if (IsMouseButtonDown (MOUSE_LEFT_BUTTON)) {
                        DrawRectangleRec (if_yes_rec, DARKGREEN);
                        if (IsMouseButtonPressed (MOUSE_LEFT_BUTTON)) {
                            tree->current_node = current->positive;
                        }
                    }
                }

                else DrawRectangleRec (if_yes_rec, GRAY);

                DrawText ("YES", (int)if_yes_rec.x + 70, (int)if_yes_rec.y + 15, 30, WHITE);

                Rectangle if_no_rec = { 350, 410, 200, 60 };
                if (CheckCollisionPointRec (mouse, if_no_rec)) {
                    DrawRectangleRec (if_no_rec, RED);
                    if (IsMouseButtonDown (MOUSE_LEFT_BUTTON)) {
                        DrawRectangleRec (if_no_rec, MAROON);
                        if (IsMouseButtonPressed (MOUSE_LEFT_BUTTON)) {
                            tree->current_node = current->negative;
                        }
                    }
                }

                else DrawRectangleRec (if_no_rec, GRAY);

                DrawText ("NO", (int)if_no_rec.x + 75, (int)if_no_rec.y + 15, 30, WHITE);
            }

            else {
                PlaySound (answer);
                end_game = true;
            }
        }

        if (end_game) {
            const char* text = TextFormat ("I think it's %s", current->text);
            int text_width = MeasureText (text, 55);

            DrawText (text, (800 - text_width) / 2, 330, 40, DARKGRAY);

            Rectangle if_restart_rec = { 220, 410, 200, 60 };
            if (CheckCollisionPointRec (mouse, if_restart_rec)) {
                DrawRectangleRec (if_restart_rec, SKYBLUE);
                if (IsMouseButtonDown (MOUSE_LEFT_BUTTON)) {
                        DrawRectangleRec (if_restart_rec, DARKBLUE);
                        if (IsMouseButtonPressed (MOUSE_LEFT_BUTTON)) {
                            tree->current_node = tree->root;
                            end_game = false;
                        }
                    }
            }

            else DrawRectangleRec (if_restart_rec, GRAY);

            DrawText ("RESTART", (int)if_restart_rec.x + 30, (int)if_restart_rec.y + 15, 30, WHITE);
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

bool update_button (button_t* btn, Vector2 mouse_pos, bool is_mouse_released) { // проверяет, кликнули ли по кнопке
    assert (btn != NULL);

    bool is_hovered = CheckCollisionPointRec (mouse_pos, btn->rect);
    bool is_pressed = false;

    if (is_hovered && is_mouse_released) is_pressed = true;

    return is_pressed;
}

void draw_button (button_t* btn, Vector2 mouse_pos, bool is_mouse_down) {
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

    int text_width = MeasureText (btn->text, 30);
    DrawText (btn->text, (int)(btn->rect.x + ((float)btn->rect.width - (float)text_width) / 2), (int)btn->rect.y + 15, 30, WHITE);
}

