/********************************************************************
    Group Name   :       0day
    1st Member Name      :       Momen Mohamedelhassan Mohamed Elhag
    1st Matric Number    :       25014926
    2nd Member Name      :       Ammar Hafiz bin Kamalul
    2nd Matric Number    :       26001569
    Semester             :       January 2026

-Command to compile the project:
gcc game.c gfx.c -o game.o -lX11 -lm

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include "gfx.h"

/* ======================== CONSTANTS ======================== */

#define WIN_W 1024
#define WIN_H 720
#define MAX_WORDS 40
#define MAX_WORD_LEN 20
#define MAX_PUZZLES 3
#define MAX_LETTERS 16
#define TILE_SIZE 52
#define TILE_GAP 6
#define TIMER_MAX 10
#define STARTING_LIVES 3
#define POINTS_PER_CORRECT 10
#define MAX_SCORES 10

/* ======================== DATA STRUCTURES ======================== */

typedef struct {
    char words[MAX_WORDS][MAX_WORD_LEN];
    int word_count;
} PuzzleSet;

typedef struct {
    char name[30];
    int score;
    int set_used;
} ScoreRecord;

/* ======================== GLOBAL STATE ======================== */

static PuzzleSet puzzles[MAX_PUZZLES];
static ScoreRecord high_scores[MAX_SCORES];
static int num_scores = 0;

static const char *puzzle_files[MAX_PUZZLES] = {
    "src/puzzle/puzzle1.txt",
    "src/puzzle/puzzle2.txt",
    "src/puzzle/puzzle3.txt"
};

static const char *set_names[MAX_PUZZLES] = {
    "Easy (5-6 letters)",
    "Medium (6-7 letters)",
    "Hard (8-15 letters)"
};

static const char *mc_set_names[MAX_PUZZLES] = {
    "Peaceful",
    "Survival",
    "Hardcore"
};

/* ======================== PIXEL FONT DATA ======================== */
/* 5x7 bitmap font for ASCII 32-95 (space through underscore)       */
/* Each row: bit4=leftmost column, bit0=rightmost. 7 rows per char. */
/* Lowercase maps to uppercase in the renderer.                      */

static const unsigned char mc_font[64][7] = {
    /* 32 ' ' */ {0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    /* 33 '!' */ {0x04,0x04,0x04,0x04,0x04,0x00,0x04},
    /* 34 '"' */ {0x0A,0x0A,0x0A,0x00,0x00,0x00,0x00},
    /* 35 '#' */ {0x0A,0x1F,0x0A,0x0A,0x1F,0x0A,0x00},
    /* 36 '$' */ {0x04,0x0F,0x14,0x0E,0x05,0x1E,0x04},
    /* 37 '%' */ {0x18,0x19,0x02,0x04,0x08,0x13,0x03},
    /* 38 '&' */ {0x08,0x14,0x14,0x08,0x15,0x12,0x0D},
    /* 39 ''' */ {0x04,0x04,0x08,0x00,0x00,0x00,0x00},
    /* 40 '(' */ {0x02,0x04,0x08,0x08,0x08,0x04,0x02},
    /* 41 ')' */ {0x08,0x04,0x02,0x02,0x02,0x04,0x08},
    /* 42 '*' */ {0x00,0x04,0x15,0x0E,0x15,0x04,0x00},
    /* 43 '+' */ {0x00,0x04,0x04,0x1F,0x04,0x04,0x00},
    /* 44 ',' */ {0x00,0x00,0x00,0x00,0x00,0x04,0x08},
    /* 45 '-' */ {0x00,0x00,0x00,0x1F,0x00,0x00,0x00},
    /* 46 '.' */ {0x00,0x00,0x00,0x00,0x00,0x00,0x04},
    /* 47 '/' */ {0x00,0x01,0x02,0x04,0x08,0x10,0x00},
    /* 48 '0' */ {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E},
    /* 49 '1' */ {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E},
    /* 50 '2' */ {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F},
    /* 51 '3' */ {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E},
    /* 52 '4' */ {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02},
    /* 53 '5' */ {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E},
    /* 54 '6' */ {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E},
    /* 55 '7' */ {0x1F,0x01,0x02,0x04,0x08,0x08,0x08},
    /* 56 '8' */ {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E},
    /* 57 '9' */ {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C},
    /* 58 ':' */ {0x00,0x00,0x04,0x00,0x04,0x00,0x00},
    /* 59 ';' */ {0x00,0x00,0x04,0x00,0x04,0x04,0x08},
    /* 60 '<' */ {0x02,0x04,0x08,0x10,0x08,0x04,0x02},
    /* 61 '=' */ {0x00,0x00,0x1F,0x00,0x1F,0x00,0x00},
    /* 62 '>' */ {0x08,0x04,0x02,0x01,0x02,0x04,0x08},
    /* 63 '?' */ {0x0E,0x11,0x01,0x02,0x04,0x00,0x04},
    /* 64 '@' */ {0x0E,0x11,0x17,0x15,0x17,0x10,0x0E},
    /* 65 'A' */ {0x0E,0x11,0x11,0x11,0x1F,0x11,0x11},
    /* 66 'B' */ {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E},
    /* 67 'C' */ {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E},
    /* 68 'D' */ {0x1C,0x12,0x11,0x11,0x11,0x12,0x1C},
    /* 69 'E' */ {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F},
    /* 70 'F' */ {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10},
    /* 71 'G' */ {0x0E,0x11,0x10,0x17,0x11,0x11,0x0E},
    /* 72 'H' */ {0x11,0x11,0x11,0x1F,0x11,0x11,0x11},
    /* 73 'I' */ {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E},
    /* 74 'J' */ {0x0F,0x02,0x02,0x02,0x02,0x12,0x0C},
    /* 75 'K' */ {0x11,0x12,0x14,0x18,0x14,0x12,0x11},
    /* 76 'L' */ {0x10,0x10,0x10,0x10,0x10,0x10,0x1F},
    /* 77 'M' */ {0x11,0x1B,0x15,0x15,0x11,0x11,0x11},
    /* 78 'N' */ {0x11,0x11,0x19,0x15,0x13,0x11,0x11},
    /* 79 'O' */ {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E},
    /* 80 'P' */ {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10},
    /* 81 'Q' */ {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D},
    /* 82 'R' */ {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11},
    /* 83 'S' */ {0x0E,0x11,0x10,0x0E,0x01,0x11,0x0E},
    /* 84 'T' */ {0x1F,0x04,0x04,0x04,0x04,0x04,0x04},
    /* 85 'U' */ {0x11,0x11,0x11,0x11,0x11,0x11,0x0E},
    /* 86 'V' */ {0x11,0x11,0x11,0x11,0x11,0x0A,0x04},
    /* 87 'W' */ {0x11,0x11,0x11,0x15,0x15,0x15,0x0A},
    /* 88 'X' */ {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11},
    /* 89 'Y' */ {0x11,0x11,0x0A,0x04,0x04,0x04,0x04},
    /* 90 'Z' */ {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F},
    /* 91 '[' */ {0x0E,0x08,0x08,0x08,0x08,0x08,0x0E},
    /* 92 '\' */ {0x00,0x10,0x08,0x04,0x02,0x01,0x00},
    /* 93 ']' */ {0x0E,0x02,0x02,0x02,0x02,0x02,0x0E},
    /* 94 '^' */ {0x04,0x0A,0x11,0x00,0x00,0x00,0x00},
    /* 95 '_' */ {0x00,0x00,0x00,0x00,0x00,0x00,0x1F}
};

/* ======================== PIXEL ART DATA ======================== */

/* Creeper face: 8x8 grid */
static const int creeper_face[8][8] = {
    {0,1,1,0,0,1,1,0},
    {0,1,1,0,0,1,1,0},
    {0,0,0,1,1,0,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,1,0,0,1,0,0},
    {0,0,1,0,0,1,0,0},
    {0,0,0,0,0,0,0,0}
};

/* Minecraft heart: 7 wide x 6 tall */
static const int heart_shape[6][7] = {
    {0,1,1,0,1,1,0},
    {1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1},
    {0,1,1,1,1,1,0},
    {0,0,1,1,1,0,0},
    {0,0,0,1,0,0,0}
};

/* Diamond icon: 5 wide x 8 tall */
static const int diamond_shape[8][5] = {
    {0,0,1,0,0},
    {0,1,1,1,0},
    {1,1,1,1,1},
    {1,1,1,1,1},
    {1,1,1,1,1},
    {0,1,1,1,0},
    {0,1,1,1,0},
    {0,0,1,0,0}
};

/* ======================== FUNCTION PROTOTYPES ======================== */

/* File I/O */
int load_all_puzzles(PuzzleSet psets[]);

/* Drawing primitives */
void draw_filled_rect(int x, int y, int w, int h);
void draw_rect(int x, int y, int w, int h);

/* Pixel font rendering */
void draw_pixel_char(int x, int y, char c, int scale);
void draw_pixel_string(int x, int y, const char *str, int scale);
int pixel_string_width(const char *str, int scale);
void draw_pixel_centered(int cx, int y, const char *str, int scale);
void draw_pixel_shadow(int cx, int y, const char *str, int scale, int r, int g, int b);

/* Pixel art */
void draw_creeper(int x, int y, int scale);
void draw_mc_heart(int x, int y, int scale, int filled);
void draw_mc_hearts(int x, int y, int lives, int scale);
void draw_diamond(int x, int y, int scale);

/* Minecraft UI components */
void draw_mc_background(void);
void draw_grass_strip(int y);
void draw_dirt_strip(int y);
void draw_mc_panel(int x, int y, int w, int h);
void draw_mc_button(int x, int y, int w, int h, const char *label);
void draw_mc_button_color(int x, int y, int w, int h, const char *label, int r, int g, int b);
void draw_mc_slot_empty(int x, int y, int size);
void draw_mc_slot_letter(int x, int y, int size, char letter, int bg_r, int bg_g, int bg_b);
void draw_mc_slot_dark(int x, int y, int size);
void draw_xp_bar(int x, int y, int w, int h, int seconds_left, int max_seconds);

/* Utility */
void draw_shadow_text(int x, int y, const char *text, int r, int g, int b);
void draw_centered_text(int cx, int y, const char *text);
int is_click_in_rect(int cx, int cy, int rx, int ry, int rw, int rh);

/* Scramble logic */
void scramble_word(const char *original, char *scrambled);
int pick_word(int used[], int word_count);
int check_guess(const char *guess, const char *answer);

/* GUI screens */
int gui_main_menu(void);
void gui_help_screen(void);
void gui_scoreboard(void);
int gui_difficulty_select(void);
void gui_get_player_name(char *name, int maxlen);
void gui_play_game(PuzzleSet *ps, int set_index, const char *player_name);
void gui_game_over(int score, int *choice);
void gui_error_screen(const char *msg);

/* Score management */
void add_score(const char *name, int score, int set_index);
void save_scores(void);
void load_scores(void);

/* Tile flash */
void flash_tiles_color(int slots_x[], int slots_y[], int count, int r, int g, int b, int flashes);

/* ======================== MAIN FUNCTION ======================== */

int main()
{
    int loaded;
    int choice;

    srand((unsigned int)time(NULL));

    loaded = load_all_puzzles(puzzles);
    load_scores();

    gfx_open(WIN_W, WIN_H, "Mine Scramble - Minecraft Word Game");
    gfx_clear_color(32, 32, 32);

    if(loaded == 0) {
        gui_error_screen("No puzzle files found! Check src/puzzle/ folder.");
        return 1;
    }

    while(1) {
        choice = gui_main_menu();

        if(choice == 1) {
            char player_name[30];
            int set_index;

            gui_get_player_name(player_name, 29);
            set_index = gui_difficulty_select();
            if(set_index >= 0) {
                if(puzzles[set_index].word_count == 0) {
                    char err[80];
                    sprintf(err, "Puzzle file not found: %s", puzzle_files[set_index]);
                    gui_error_screen(err);
                } else {
                    gui_play_game(&puzzles[set_index], set_index, player_name);
                }
            }
        } else if(choice == 2) {
            gui_help_screen();
        } else if(choice == 3) {
            gui_scoreboard();
        } else if(choice == 4) {
            break;
        }
    }

    return 0;
}

/* ======================== FILE I/O ======================== */

int load_all_puzzles(PuzzleSet psets[])
{
    int loaded = 0;

    for(int i = 0; i < MAX_PUZZLES; i++) {
        FILE *fp = fopen(puzzle_files[i], "r");
        psets[i].word_count = 0;

        if(fp == NULL) {
            fprintf(stderr, "Warning: Could not open %s\n", puzzle_files[i]);
        } else {
            char line[MAX_WORD_LEN + 10];
            while(fgets(line, sizeof(line), fp) != NULL && psets[i].word_count < MAX_WORDS) {
                line[strcspn(line, "\r\n")] = '\0';
                if(strlen(line) > 0) {
                    strncpy(psets[i].words[psets[i].word_count], line, MAX_WORD_LEN - 1);
                    psets[i].words[psets[i].word_count][MAX_WORD_LEN - 1] = '\0';
                    psets[i].word_count++;
                }
            }
            fclose(fp);
            loaded++;
        }
    }
    return loaded;
}

/* ======================== DRAWING PRIMITIVES ======================== */

void draw_filled_rect(int x, int y, int w, int h)
{
    for(int i = 0; i < h; i++) {
        gfx_line(x, y + i, x + w - 1, y + i);
    }
}

void draw_rect(int x, int y, int w, int h)
{
    gfx_line(x, y, x + w - 1, y);
    gfx_line(x + w - 1, y, x + w - 1, y + h - 1);
    gfx_line(x, y + h - 1, x + w - 1, y + h - 1);
    gfx_line(x, y, x, y + h - 1);
}

/* ======================== PIXEL FONT RENDERING ======================== */

void draw_pixel_char(int x, int y, char c, int scale)
{
    int idx;
    if(c >= 'a' && c <= 'z') c -= 32;
    idx = c - 32;
    if(idx < 0 || idx >= 64) return;
    for(int row = 0; row < 7; row++) {
        unsigned char bits = mc_font[idx][row];
        for(int col = 0; col < 5; col++) {
            if(bits & (0x10 >> col)) {
                draw_filled_rect(x + col * scale, y + row * scale, scale, scale);
            }
        }
    }
}

void draw_pixel_string(int x, int y, const char *str, int scale)
{
    for(int i = 0; str[i] != '\0'; i++) {
        draw_pixel_char(x + i * 6 * scale, y, str[i], scale);
    }
}

int pixel_string_width(const char *str, int scale)
{
    int len = (int)strlen(str);
    if(len == 0) return 0;
    return len * 6 * scale - scale;
}

void draw_pixel_centered(int cx, int y, const char *str, int scale)
{
    int w = pixel_string_width(str, scale);
    draw_pixel_string(cx - w / 2, y, str, scale);
}

void draw_pixel_shadow(int cx, int y, const char *str, int scale, int r, int g, int b)
{
    int w = pixel_string_width(str, scale);
    int x = cx - w / 2;
    int off = scale > 2 ? 2 : 1;
    gfx_color(r / 4, g / 4, b / 4);
    draw_pixel_string(x + off, y + off, str, scale);
    gfx_color(r, g, b);
    draw_pixel_string(x, y, str, scale);
}

/* ======================== PIXEL ART RENDERING ======================== */

void draw_creeper(int x, int y, int scale)
{
    /* Green base face */
    gfx_color(82, 148, 54);
    draw_filled_rect(x, y, 8 * scale, 8 * scale);
    /* Darker green features */
    gfx_color(30, 65, 18);
    for(int r = 0; r < 8; r++) {
        for(int c = 0; c < 8; c++) {
            if(creeper_face[r][c]) {
                draw_filled_rect(x + c * scale, y + r * scale, scale, scale);
            }
        }
    }
    /* Subtle highlight on top-left (only non-eye pixel) */
    gfx_color(105, 175, 72);
    draw_filled_rect(x, y, scale, scale);
}

void draw_mc_heart(int x, int y, int scale, int filled)
{
    for(int r = 0; r < 6; r++) {
        for(int c = 0; c < 7; c++) {
            if(heart_shape[r][c]) {
                if(filled) {
                    if(r == 0) {
                        gfx_color(255, 90, 90);
                    } else if(r == 1 && c <= 2) {
                        gfx_color(230, 60, 60);
                    } else {
                        gfx_color(190, 25, 25);
                    }
                } else {
                    gfx_color(50, 15, 15);
                }
                draw_filled_rect(x + c * scale, y + r * scale, scale, scale);
            }
        }
    }
    /* Dark outline for definition */
    if(filled) {
        gfx_color(100, 10, 10);
    } else {
        gfx_color(30, 8, 8);
    }
    for(int r = 0; r < 6; r++) {
        for(int c = 0; c < 7; c++) {
            if(heart_shape[r][c]) {
                /* Check if edge pixel (adjacent to empty) */
                int is_edge = 0;
                if(r == 0 || (r > 0 && !heart_shape[r-1][c])) is_edge = 1;
                if(r == 5 || (r < 5 && !heart_shape[r+1][c])) is_edge = 1;
                if(c == 0 || (c > 0 && !heart_shape[r][c-1])) is_edge = 1;
                if(c == 6 || (c < 6 && !heart_shape[r][c+1])) is_edge = 1;
                if(is_edge) {
                    draw_rect(x + c * scale, y + r * scale, scale, scale);
                }
            }
        }
    }
}

void draw_mc_hearts(int x, int y, int lives, int scale)
{
    int spacing = 7 * scale + scale * 2;
    int max_display = 10;
    for(int i = 0; i < max_display && i < 10; i++) {
        draw_mc_heart(x + i * spacing, y, scale, i < lives);
    }
}

void draw_diamond(int x, int y, int scale)
{
    for(int r = 0; r < 8; r++) {
        for(int c = 0; c < 5; c++) {
            if(diamond_shape[r][c]) {
                if(r <= 1) {
                    gfx_color(160, 240, 240);
                } else if(r <= 3) {
                    gfx_color(100, 220, 220);
                } else if(r <= 5) {
                    gfx_color(60, 190, 200);
                } else {
                    gfx_color(40, 160, 180);
                }
                draw_filled_rect(x + c * scale, y + r * scale, scale, scale);
            }
        }
    }
}

/* ======================== MINECRAFT UI COMPONENTS ======================== */

void draw_mc_background(void)
{
    gfx_clear();
    /* Subtle block grid pattern */
    gfx_color(38, 38, 38);
    for(int x = 0; x < WIN_W; x += 32) {
        gfx_line(x, 0, x, WIN_H);
    }
    for(int y = 0; y < WIN_H; y += 32) {
        gfx_line(0, y, WIN_W, y);
    }
}

void draw_grass_strip(int y)
{
    /* Grass green top layer */
    gfx_color(90, 168, 48);
    draw_filled_rect(0, y, WIN_W, 4);
    /* Dirt brown layer below */
    gfx_color(134, 96, 67);
    draw_filled_rect(0, y + 4, WIN_W, 5);
    /* Darker dirt accent */
    gfx_color(110, 78, 54);
    draw_filled_rect(0, y + 9, WIN_W, 2);
}

void draw_dirt_strip(int y)
{
    gfx_color(110, 78, 54);
    draw_filled_rect(0, y, WIN_W, 2);
    gfx_color(134, 96, 67);
    draw_filled_rect(0, y + 2, WIN_W, 5);
    gfx_color(90, 168, 48);
    draw_filled_rect(0, y + 7, WIN_W, 4);
}

void draw_mc_panel(int x, int y, int w, int h)
{
    /* Fill: Minecraft inventory gray */
    gfx_color(198, 198, 198);
    draw_filled_rect(x, y, w, h);
    /* Top-left highlight (white) */
    gfx_color(255, 255, 255);
    draw_filled_rect(x, y, w, 3);
    draw_filled_rect(x, y, 3, h);
    /* Bottom-right shadow */
    gfx_color(85, 85, 85);
    draw_filled_rect(x, y + h - 3, w, 3);
    draw_filled_rect(x + w - 3, y, 3, h);
    /* Corner cleanup */
    gfx_color(198, 198, 198);
    draw_filled_rect(x + w - 3, y, 3, 3);
    draw_filled_rect(x, y + h - 3, 3, 3);
    /* Black outline */
    gfx_color(0, 0, 0);
    draw_rect(x - 1, y - 1, w + 2, h + 2);
}

void draw_mc_button(int x, int y, int w, int h, const char *label)
{
    draw_mc_button_color(x, y, w, h, label, 150, 150, 150);
}

void draw_mc_button_color(int x, int y, int w, int h, const char *label,
                          int r, int g, int b)
{
    int lw, lh, tx, ty;
    int hr, hg, hb, sr, sg, sb;

    /* Button face */
    gfx_color(r, g, b);
    draw_filled_rect(x, y, w, h);

    /* Top-left highlight */
    hr = r + 55; if(hr > 255) hr = 255;
    hg = g + 55; if(hg > 255) hg = 255;
    hb = b + 55; if(hb > 255) hb = 255;
    gfx_color(hr, hg, hb);
    draw_filled_rect(x, y, w, 2);
    draw_filled_rect(x, y, 2, h);

    /* Bottom-right shadow */
    sr = r * 2 / 3; sg = g * 2 / 3; sb = b * 2 / 3;
    gfx_color(sr, sg, sb);
    draw_filled_rect(x, y + h - 2, w, 2);
    draw_filled_rect(x + w - 2, y, 2, h);

    /* Black outline */
    gfx_color(0, 0, 0);
    draw_rect(x - 1, y - 1, w + 2, h + 2);

    /* Label centered using pixel font scale 2 */
    lw = pixel_string_width(label, 2);
    lh = 7 * 2;
    tx = x + (w - lw) / 2;
    ty = y + (h - lh) / 2;
    gfx_color(sr / 2, sg / 2, sb / 2);
    draw_pixel_string(tx + 1, ty + 1, label, 2);
    gfx_color(255, 255, 255);
    draw_pixel_string(tx, ty, label, 2);
}

void draw_mc_slot_empty(int x, int y, int size)
{
    /* Dark fill */
    gfx_color(40, 40, 40);
    draw_filled_rect(x, y, size, size);
    /* Inset border: top-left dark, bottom-right lighter */
    gfx_color(22, 22, 22);
    gfx_line(x, y, x + size - 1, y);
    gfx_line(x, y, x, y + size - 1);
    gfx_color(90, 90, 90);
    gfx_line(x + size - 1, y, x + size - 1, y + size - 1);
    gfx_line(x, y + size - 1, x + size - 1, y + size - 1);
}

void draw_mc_slot_letter(int x, int y, int size, char letter,
                         int bg_r, int bg_g, int bg_b)
{
    int char_w, char_h, tx, ty;
    int hr, hg, hb, sr, sg, sb;
    char uc;

    /* Background fill */
    gfx_color(bg_r, bg_g, bg_b);
    draw_filled_rect(x, y, size, size);

    /* 3D raised border */
    hr = bg_r + 45; if(hr > 255) hr = 255;
    hg = bg_g + 45; if(hg > 255) hg = 255;
    hb = bg_b + 45; if(hb > 255) hb = 255;
    gfx_color(hr, hg, hb);
    gfx_line(x, y, x + size - 1, y);
    gfx_line(x, y, x, y + size - 1);

    sr = bg_r * 2 / 3; sg = bg_g * 2 / 3; sb = bg_b * 2 / 3;
    gfx_color(sr, sg, sb);
    gfx_line(x + size - 1, y, x + size - 1, y + size - 1);
    gfx_line(x, y + size - 1, x + size - 1, y + size - 1);

    /* Letter centered with shadow */
    uc = toupper((unsigned char)letter);
    char_w = 5 * 3;
    char_h = 7 * 3;
    tx = x + (size - char_w) / 2;
    ty = y + (size - char_h) / 2;
    gfx_color(20, 20, 20);
    draw_pixel_char(tx + 1, ty + 1, uc, 3);
    gfx_color(255, 255, 255);
    draw_pixel_char(tx, ty, uc, 3);
}

void draw_mc_slot_dark(int x, int y, int size)
{
    /* Very dark grayed-out slot */
    gfx_color(28, 28, 32);
    draw_filled_rect(x, y, size, size);
    gfx_color(20, 20, 22);
    gfx_line(x, y, x + size - 1, y);
    gfx_line(x, y, x, y + size - 1);
    gfx_color(42, 42, 46);
    gfx_line(x + size - 1, y, x + size - 1, y + size - 1);
    gfx_line(x, y + size - 1, x + size - 1, y + size - 1);
}

void draw_xp_bar(int x, int y, int w, int h, int seconds_left, int max_seconds)
{
    int fill_w;
    int r, g, b;
    char buf[16];

    /* Background */
    gfx_color(20, 20, 20);
    draw_filled_rect(x, y, w, h);

    /* Fill amount */
    fill_w = (seconds_left * w) / max_seconds;
    if(fill_w < 0) fill_w = 0;
    if(fill_w > w) fill_w = w;

    /* Color based on time */
    if(seconds_left > 6) {
        r = 80; g = 220; b = 50;
    } else if(seconds_left > 3) {
        r = 230; g = 200; b = 40;
    } else {
        r = 220; g = 50; b = 40;
    }

    if(fill_w > 0) {
        gfx_color(r, g, b);
        draw_filled_rect(x, y, fill_w, h);
        /* Highlight on fill top edge */
        gfx_color(r + 40 > 255 ? 255 : r + 40, g + 40 > 255 ? 255 : g + 40, b + 40 > 255 ? 255 : b + 40);
        gfx_line(x, y, x + fill_w - 1, y);
    }

    /* Inset border */
    gfx_color(10, 10, 10);
    gfx_line(x, y, x + w - 1, y);
    gfx_line(x, y, x, y + h - 1);
    gfx_color(70, 70, 70);
    gfx_line(x + w - 1, y, x + w - 1, y + h - 1);
    gfx_line(x, y + h - 1, x + w - 1, y + h - 1);

    /* Timer text to the right */
    sprintf(buf, "%ds", seconds_left);
    gfx_color(255, 255, 255);
    gfx_text(x + w + 10, y + h - 3, buf);
}

/* ======================== UTILITY ======================== */

void draw_shadow_text(int x, int y, const char *text, int r, int g, int b)
{
    gfx_color(r / 4, g / 4, b / 4);
    gfx_text(x + 1, y + 1, text);
    gfx_color(r, g, b);
    gfx_text(x, y, text);
}

void draw_centered_text(int cx, int y, const char *text)
{
    int tx = cx - (int)strlen(text) * 3;
    gfx_text(tx, y, text);
}

int is_click_in_rect(int cx, int cy, int rx, int ry, int rw, int rh)
{
    return (cx >= rx && cx < rx + rw && cy >= ry && cy < ry + rh);
}

/* ======================== SCRAMBLE LOGIC ======================== */

void scramble_word(const char *original, char *scrambled)
{
    int len = (int)strlen(original);
    int j;
    char temp;

    strcpy(scrambled, original);

    for(int attempts = 0; attempts < 20; attempts++) {
        for(int i = len - 1; i > 0; i--) {
            j = rand() % (i + 1);
            temp = scrambled[i];
            scrambled[i] = scrambled[j];
            scrambled[j] = temp;
        }
        if(strcmp(scrambled, original) != 0) {
            return;
        }
    }
    if(len >= 2) {
        temp = scrambled[0];
        scrambled[0] = scrambled[1];
        scrambled[1] = temp;
    }
}

int pick_word(int used[], int word_count)
{
    int available[MAX_WORDS];
    int num_available = 0;

    for(int i = 0; i < word_count; i++) {
        if(!used[i]) {
            available[num_available] = i;
            num_available++;
        }
    }

    if(num_available == 0) return -1;
    return available[rand() % num_available];
}

int check_guess(const char *guess, const char *answer)
{
    int len_g = (int)strlen(guess);
    int len_a = (int)strlen(answer);

    if(len_g != len_a) return 0;

    for(int i = 0; i < len_a; i++) {
        if(tolower((unsigned char)guess[i]) != tolower((unsigned char)answer[i])) {
            return 0;
        }
    }
    return 1;
}

/* ======================== GUI: MAIN MENU ======================== */

int gui_main_menu(void)
{
    int btn_w = 300;
    int btn_h = 44;
    int cx = WIN_W / 2 - btn_w / 2;
    int mx, my;
    int creeper_size;
    int title_y, creeper_y, sub_y;
    int btn1_y, btn2_y, btn3_y, btn4_y;

    /* Layout positions */
    title_y = 65;
    creeper_size = 8;
    creeper_y = 130;
    sub_y = 210;
    btn1_y = 260;
    btn2_y = 318;
    btn3_y = 376;
    btn4_y = 434;

    while(1) {
        draw_mc_background();
        draw_grass_strip(0);
        draw_dirt_strip(WIN_H - 11);

        /* Title with shadow */
        draw_pixel_shadow(WIN_W / 2, title_y, "MINE SCRAMBLE", 5,
                          255, 255, 85);

        /* Creeper face decoration */
        draw_creeper(WIN_W / 2 - 4 * creeper_size, creeper_y, creeper_size);

        /* Subtitle */
        draw_pixel_shadow(WIN_W / 2, sub_y, "UNSCRAMBLE THE WORD!", 2,
                          180, 180, 200);

        /* Buttons */
        draw_mc_button_color(cx, btn1_y, btn_w, btn_h, "PLAY GAME",
                             80, 145, 55);
        draw_mc_button_color(cx, btn2_y, btn_w, btn_h, "HOW TO PLAY",
                             70, 100, 155);
        draw_mc_button_color(cx, btn3_y, btn_w, btn_h, "LEADERBOARD",
                             165, 140, 45);
        draw_mc_button_color(cx, btn4_y, btn_w, btn_h, "QUIT GAME",
                             155, 60, 55);

        /* Footer */
        gfx_color(80, 80, 80);
        draw_centered_text(WIN_W / 2, WIN_H - 22,
                           "SPI Project - January 2026");

        gfx_flush();

        gfx_wait();
        mx = gfx_xpos();
        my = gfx_ypos();

        if(is_click_in_rect(mx, my, cx, btn1_y, btn_w, btn_h)) return 1;
        if(is_click_in_rect(mx, my, cx, btn2_y, btn_w, btn_h)) return 2;
        if(is_click_in_rect(mx, my, cx, btn3_y, btn_w, btn_h)) return 3;
        if(is_click_in_rect(mx, my, cx, btn4_y, btn_w, btn_h)) return 4;
    }
}

/* ======================== GUI: HELP SCREEN ======================== */

void gui_help_screen(void)
{
    int mx, my;
    int back_w = 160;
    int back_h = 40;
    int back_x = WIN_W / 2 - back_w / 2;
    int back_y = 620;
    int panel_x = 80;
    int panel_y = 80;
    int panel_w = WIN_W - 160;
    int panel_h = 520;
    int lx, ly;

    while(1) {
        draw_mc_background();
        draw_grass_strip(0);
        draw_dirt_strip(WIN_H - 11);

        /* Title */
        draw_pixel_shadow(WIN_W / 2, 30, "HOW TO PLAY", 3,
                          100, 200, 255);

        /* Content panel */
        draw_mc_panel(panel_x, panel_y, panel_w, panel_h);

        lx = panel_x + 30;
        ly = panel_y + 25;

        gfx_color(50, 50, 50);
        gfx_text(lx, ly, "1. A scrambled word will appear on screen as letter tiles.");
        ly += 32;
        gfx_text(lx, ly, "2. Click the scrambled letters to place them in the answer slots.");
        ly += 32;
        gfx_text(lx, ly, "   You can also type letters on the keyboard to place them.");
        ly += 32;
        gfx_text(lx, ly, "3. Click a placed letter or press Backspace to remove it.");
        ly += 32;
        gfx_text(lx, ly, "4. Once all slots are filled, your guess is checked automatically.");
        ly += 32;
        gfx_text(lx, ly, "   You can also press Enter to submit your answer.");
        ly += 48;

        gfx_color(30, 30, 30);
        gfx_text(lx, ly, "SCORING:");
        ly += 28;

        /* Correct - with heart decoration */
        draw_mc_heart(lx, ly - 10, 2, 1);
        gfx_color(30, 120, 30);
        gfx_text(lx + 20, ly, "Correct guess within 10 seconds:  +1 life,  +10 points");
        ly += 28;

        /* Wrong */
        draw_mc_heart(lx, ly - 10, 2, 0);
        gfx_color(160, 30, 30);
        gfx_text(lx + 20, ly, "Wrong guess or time runs out:  -1 life,  0 points");
        ly += 40;

        gfx_color(50, 50, 50);
        gfx_text(lx, ly, "You start with 3 lives. Game ends when lives reach 0.");
        ly += 28;
        gfx_text(lx, ly, "The correct answer is shown after a wrong guess or timeout.");
        ly += 28;
        gfx_text(lx, ly, "No repeated words in a single game session.");
        ly += 40;

        /* Difficulty info with creeper */
        draw_creeper(lx, ly - 6, 3);
        gfx_color(30, 30, 30);
        gfx_text(lx + 32, ly, "Choose your difficulty: Peaceful, Survival, or Hardcore!");

        /* Back button */
        draw_mc_button(back_x, back_y, back_w, back_h, "BACK");

        gfx_flush();

        gfx_wait();
        mx = gfx_xpos();
        my = gfx_ypos();

        if(is_click_in_rect(mx, my, back_x, back_y, back_w, back_h)) {
            return;
        }
    }
}

/* ======================== GUI: SCOREBOARD ======================== */

void gui_scoreboard(void)
{
    int mx, my;
    int back_w = 160;
    int back_h = 40;
    int back_x = WIN_W / 2 - back_w / 2;
    int back_y = 620;
    int panel_x = 100;
    int panel_y = 90;
    int panel_w = WIN_W - 200;
    int panel_h = 500;

    while(1) {
        draw_mc_background();
        draw_grass_strip(0);
        draw_dirt_strip(WIN_H - 11);

        /* Title with diamond decoration */
        {
            int title_y = 28;
            int title_h = 7 * 3;
            int dia_h = 8 * 3;
            int dia_y = title_y + title_h / 2 - dia_h / 2;
            draw_diamond(WIN_W / 2 - 120, dia_y, 3);
            draw_pixel_shadow(WIN_W / 2, title_y, "LEADERBOARD", 3,
                              255, 215, 0);
            draw_diamond(WIN_W / 2 + 106, dia_y, 3);
        }

        /* Content panel */
        draw_mc_panel(panel_x, panel_y, panel_w, panel_h);

        if(num_scores == 0) {
            gfx_color(100, 100, 100);
            draw_centered_text(WIN_W / 2, 340,
                               "No scores recorded yet. Play a game!");
        } else {
            /* Table header */
            gfx_color(60, 60, 60);
            gfx_text(panel_x + 40, panel_y + 28, "RANK");
            gfx_text(panel_x + 120, panel_y + 28, "PLAYER");
            gfx_text(panel_x + 370, panel_y + 28, "SCORE");
            gfx_text(panel_x + 500, panel_y + 28, "DIFFICULTY");

            /* Separator line */
            gfx_color(140, 140, 140);
            gfx_line(panel_x + 20, panel_y + 38,
                     panel_x + panel_w - 20, panel_y + 38);

            for(int i = 0; i < num_scores && i < MAX_SCORES; i++) {
                char buf[80];
                int row_y = panel_y + 60 + i * 38;

                /* Medal colors for top 3 */
                if(i == 0) {
                    gfx_color(200, 170, 0);
                } else if(i == 1) {
                    gfx_color(160, 160, 170);
                } else if(i == 2) {
                    gfx_color(180, 115, 40);
                } else {
                    gfx_color(80, 80, 80);
                }

                sprintf(buf, "#%d", i + 1);
                gfx_text(panel_x + 50, row_y, buf);
                gfx_text(panel_x + 120, row_y, high_scores[i].name);
                sprintf(buf, "%d", high_scores[i].score);
                gfx_text(panel_x + 390, row_y, buf);
                if(high_scores[i].set_used >= 0 &&
                    high_scores[i].set_used < MAX_PUZZLES) {
                    gfx_text(panel_x + 500, row_y,
                             mc_set_names[high_scores[i].set_used]);
                }
            }
        }

        draw_mc_button(back_x, back_y, back_w, back_h, "BACK");

        gfx_flush();

        gfx_wait();
        mx = gfx_xpos();
        my = gfx_ypos();

        if(is_click_in_rect(mx, my, back_x, back_y, back_w, back_h)) {
            return;
        }
    }
}

/* ======================== GUI: DIFFICULTY SELECT ======================== */

int gui_difficulty_select(void)
{
    int btn_w = 340;
    int btn_h = 50;
    int cx = WIN_W / 2 - btn_w / 2;
    int mx, my;
    int back_w = 160;
    int back_h = 40;
    int back_x = WIN_W / 2 - back_w / 2;
    char label[80];

    int btn1_y = 165;
    int btn2_y = 260;
    int btn3_y = 355;
    int btn4_y = 450;
    int back_y = 550;

    while(1) {
        draw_mc_background();
        draw_grass_strip(0);
        draw_dirt_strip(WIN_H - 11);

        /* Title */
        draw_pixel_shadow(WIN_W / 2, 40, "SELECT DIFFICULTY", 3,
                          100, 200, 255);

        gfx_color(160, 160, 180);
        draw_centered_text(WIN_W / 2, 100,
                           "Choose your word challenge level:");

        /* Peaceful (Easy) */
        draw_mc_button_color(cx, btn1_y, btn_w, btn_h, "PEACEFUL",
                             80, 160, 55);
        gfx_color(120, 190, 100);
        sprintf(label, "%d words, 5-6 letters - Overworld", puzzles[0].word_count);
        draw_centered_text(WIN_W / 2, btn1_y + btn_h + 18, label);

        /* Survival (Medium) */
        draw_mc_button_color(cx, btn2_y, btn_w, btn_h, "SURVIVAL",
                             190, 160, 40);
        gfx_color(210, 190, 80);
        sprintf(label, "%d words, 6-7 letters - The Nether", puzzles[1].word_count);
        draw_centered_text(WIN_W / 2, btn2_y + btn_h + 18, label);

        /* Hardcore (Hard) */
        draw_mc_button_color(cx, btn3_y, btn_w, btn_h, "HARDCORE",
                             170, 50, 50);
        gfx_color(210, 100, 100);
        sprintf(label, "%d words, 8-15 letters - The End", puzzles[2].word_count);
        draw_centered_text(WIN_W / 2, btn3_y + btn_h + 18, label);

        /* Random */
        draw_mc_button_color(cx, btn4_y, btn_w, btn_h, "RANDOM",
                             120, 70, 170);
        gfx_color(160, 130, 210);
        draw_centered_text(WIN_W / 2, btn4_y + btn_h + 18,
                           "Random difficulty - Adventure mode");

        /* Back */
        draw_mc_button(back_x, back_y, back_w, back_h, "BACK");

        gfx_flush();

        gfx_wait();
        mx = gfx_xpos();
        my = gfx_ypos();

        if(is_click_in_rect(mx, my, cx, btn1_y, btn_w, btn_h)) return 0;
        if(is_click_in_rect(mx, my, cx, btn2_y, btn_w, btn_h)) return 1;
        if(is_click_in_rect(mx, my, cx, btn3_y, btn_w, btn_h)) return 2;
        if(is_click_in_rect(mx, my, cx, btn4_y, btn_w, btn_h))
            return rand() % MAX_PUZZLES;
        if(is_click_in_rect(mx, my, back_x, back_y, back_w, back_h))
            return -1;
    }
}

/* ======================== GUI: PLAYER NAME INPUT ======================== */

void gui_get_player_name(char *name, int maxlen)
{
    int len = 0;
    char c;
    int input_x = WIN_W / 2 - 160;
    int input_y = 290;
    int input_w = 320;
    int input_h = 48;

    name[0] = '\0';

    while(1) {
        draw_mc_background();
        draw_grass_strip(0);
        draw_dirt_strip(WIN_H - 11);

        /* Title */
        draw_pixel_shadow(WIN_W / 2, 120, "ENTER YOUR NAME", 3,
                          100, 200, 255);

        /* Creeper decoration */
        draw_creeper(WIN_W / 2 - 24, 190, 6);

        /* Instruction */
        gfx_color(160, 160, 180);
        draw_centered_text(WIN_W / 2, 262,
                           "Type your name and press Enter to begin");

        /* Input box - inventory slot style */
        gfx_color(20, 20, 20);
        draw_filled_rect(input_x, input_y, input_w, input_h);
        /* Inset border */
        gfx_color(10, 10, 10);
        gfx_line(input_x, input_y, input_x + input_w - 1, input_y);
        gfx_line(input_x, input_y, input_x, input_y + input_h - 1);
        gfx_color(80, 80, 80);
        gfx_line(input_x + input_w - 1, input_y,
                 input_x + input_w - 1, input_y + input_h - 1);
        gfx_line(input_x, input_y + input_h - 1,
                 input_x + input_w - 1, input_y + input_h - 1);

        /* Name text with blinking cursor */
        {
            char display[60];
            int text_w, tx, ty;
            sprintf(display, "%s_", name);
            text_w = pixel_string_width(display, 2);
            tx = input_x + (input_w - text_w) / 2;
            ty = input_y + (input_h - 14) / 2;
            gfx_color(255, 255, 255);
            draw_pixel_string(tx, ty, display, 2);
        }

        gfx_color(90, 90, 110);
        draw_centered_text(WIN_W / 2, 370,
                           "(Use keyboard to type, Enter to confirm)");

        gfx_flush();

        c = gfx_wait();

        if(c == '\r' || c == '\n' || c == 13) {
            if(len > 0) return;
        } else if(c == 8 || c == 127) {
            if(len > 0) {
                len--;
                name[len] = '\0';
            }
        } else if(c >= 32 && c <= 126 && len < maxlen) {
            name[len] = c;
            len++;
            name[len] = '\0';
        }
    }
}

/* ======================== GUI: GAME PLAY ======================== */

void flash_tiles_color(int slots_x[], int slots_y[], int count,
                       int r, int g, int b, int flashes)
{
    for(int f = 0; f < flashes; f++) {
        gfx_color(r, g, b);
        for(int i = 0; i < count; i++) {
            draw_filled_rect(slots_x[i], slots_y[i], TILE_SIZE, TILE_SIZE);
        }
        gfx_flush();
        usleep(100000);

        gfx_color(32, 32, 32);
        for(int i = 0; i < count; i++) {
            draw_filled_rect(slots_x[i], slots_y[i], TILE_SIZE, TILE_SIZE);
        }
        gfx_flush();
        usleep(70000);
    }
}

void gui_play_game(PuzzleSet *ps, int set_index, const char *player_name)
{
    int lives = STARTING_LIVES;
    int score = 0;
    int used[MAX_WORDS];
    int word_idx;
    int round_num = 0;

    /* HUD layout constants */
    int hud_panel_x = 8;
    int hud_panel_y = 14;
    int hud_panel_w = WIN_W - 16;
    int hud_panel_h = 52;

    memset(used, 0, sizeof(used));

    while(lives > 0) {
        char word[MAX_WORD_LEN];
        char scrambled[MAX_WORD_LEN];
        int word_len;

        /* Tile state */
        int scramble_x[MAX_LETTERS], scramble_y[MAX_LETTERS];
        int slot_x[MAX_LETTERS], slot_y[MAX_LETTERS];
        int tile_selected[MAX_LETTERS];
        char answer_slots[MAX_LETTERS];
        int answer_count;
        int tiles_start_x;

        /* Timer */
        time_t round_start;
        int seconds_left;

        /* Result: 0=pending, 1=correct, -1=wrong, -2=timeout */
        int result;
        int pending_auto_check;

        /* Menu button */
        int menu_btn_x = 20;
        int menu_btn_y = WIN_H - 52;
        int menu_btn_w = 120;
        int menu_btn_h = 38;

        word_idx = pick_word(used, ps->word_count);
        if(word_idx < 0) {
            /* All words completed */
            draw_mc_background();
            draw_grass_strip(0);
            draw_pixel_shadow(WIN_W / 2, 260, "ALL WORDS DONE!", 4,
                              80, 255, 80);
            {
                char buf[60];
                int scale_d = 3;
                int text_y = 340;
                int sw, diamond_w, text_h, diamond_h, gap, half_text, diamond_y;
                sprintf(buf, "SCORE: %d", score);
                sw = pixel_string_width(buf, scale_d);
                diamond_w = 5 * scale_d;
                text_h = 7 * scale_d;
                diamond_h = 8 * scale_d;
                gap = 10;
                half_text = sw / 2;
                diamond_y = text_y + text_h / 2 - diamond_h / 2;
                draw_diamond(WIN_W / 2 - half_text - gap - diamond_w, diamond_y, scale_d);
                draw_pixel_shadow(WIN_W / 2, text_y, buf, scale_d,
                                  255, 215, 0);
                draw_diamond(WIN_W / 2 + half_text + gap, diamond_y, scale_d);
            }
            draw_creeper(WIN_W / 2 - 28, 400, 7);
            gfx_flush();
            usleep(2500000);
            break;
        }
        used[word_idx] = 1;
        round_num++;

        strcpy(word, ps->words[word_idx]);
        word_len = (int)strlen(word);
        scramble_word(word, scrambled);

        /* Calculate tile positions (centered) */
        tiles_start_x = WIN_W / 2 - (word_len * (TILE_SIZE + TILE_GAP)) / 2;

        for(int i = 0; i < word_len; i++) {
            scramble_x[i] = tiles_start_x + i * (TILE_SIZE + TILE_GAP);
            scramble_y[i] = 190;
            slot_x[i] = tiles_start_x + i * (TILE_SIZE + TILE_GAP);
            slot_y[i] = 320;
            tile_selected[i] = 0;
        }

        answer_count = 0;
        memset(answer_slots, 0, sizeof(answer_slots));
        result = 0;
        pending_auto_check = 0;
        round_start = time(NULL);

        /* === Round loop === */
        while(result == 0) {
            int elapsed;
            int mx, my;
            char buf[80];

            elapsed = (int)difftime(time(NULL), round_start);
            seconds_left = TIMER_MAX - elapsed;
            if(seconds_left < 0) seconds_left = 0;

            if(seconds_left <= 0) {
                result = -2;
                break;
            }

            /* === DRAW FRAME === */
            draw_mc_background();
            draw_grass_strip(0);

            /* HUD panel */
            draw_mc_panel(hud_panel_x, hud_panel_y, hud_panel_w, hud_panel_h);

            /* Player name in HUD */
            gfx_color(50, 50, 50);
            sprintf(buf, "Player: %s", player_name);
            gfx_text(hud_panel_x + 18, hud_panel_y + 20, buf);

            /* Hearts below player name */
            draw_mc_hearts(hud_panel_x + 18, hud_panel_y + 30, lives, 2);

            /* Score with diamond icon (right side) */
            draw_diamond(hud_panel_w - 160, hud_panel_y + 8, 2);
            gfx_color(50, 50, 50);
            sprintf(buf, "Score: %d", score);
            gfx_text(hud_panel_w - 140, hud_panel_y + 20, buf);

            sprintf(buf, "Round: %d", round_num);
            gfx_text(hud_panel_w - 140, hud_panel_y + 38, buf);

            /* Difficulty tag centered */
            gfx_color(80, 80, 100);
            sprintf(buf, "[ %s - %s ]", mc_set_names[set_index],
                    set_names[set_index]);
            draw_centered_text(WIN_W / 2, hud_panel_y + 30, buf);

            /* Instruction text */
            gfx_color(160, 160, 180);
            draw_centered_text(WIN_W / 2, 90,
                               "Unscramble the word! Click or type letters. Backspace to undo.");

            /* XP Timer bar */
            draw_xp_bar(WIN_W / 2 - 200, 105, 400, 16,
                        seconds_left, TIMER_MAX);

            /* "SCRAMBLED" label */
            gfx_color(130, 130, 160);
            draw_centered_text(WIN_W / 2, 160, "Scrambled Letters:");

            /* Scrambled letter tiles */
            for(int i = 0; i < word_len; i++) {
                if(tile_selected[i]) {
                    draw_mc_slot_dark(scramble_x[i], scramble_y[i], TILE_SIZE);
                } else {
                    draw_mc_slot_letter(scramble_x[i], scramble_y[i], TILE_SIZE,
                                        scrambled[i], 65, 65, 110);
                }
            }

            /* Down arrow indicator */
            {
                int ax = WIN_W / 2;
                int ay = 260;
                gfx_color(100, 100, 130);
                gfx_line(ax, ay, ax, ay + 20);
                gfx_line(ax, ay + 20, ax - 8, ay + 12);
                gfx_line(ax, ay + 20, ax + 8, ay + 12);
            }

            /* "YOUR ANSWER" label */
            gfx_color(130, 130, 160);
            draw_centered_text(WIN_W / 2, 298, "Your Answer:");

            /* Answer slots */
            for(int i = 0; i < word_len; i++) {
                if(i < answer_count) {
                    draw_mc_slot_letter(slot_x[i], slot_y[i], TILE_SIZE,
                                        answer_slots[i], 45, 100, 55);
                } else {
                    draw_mc_slot_empty(slot_x[i], slot_y[i], TILE_SIZE);
                }
            }

            /* Menu button */
            draw_mc_button(menu_btn_x, menu_btn_y, menu_btn_w, menu_btn_h,
                           "MENU");

            gfx_flush();

            if(pending_auto_check && answer_count == word_len && result == 0) {
                char guess[MAX_WORD_LEN];
                strncpy(guess, answer_slots, word_len);
                guess[word_len] = '\0';

                if(check_guess(guess, word)) {
                    result = 1;
                } else {
                    result = -1;
                }
                pending_auto_check = 0;
                usleep(30000);
                continue;
            }

            /* === HANDLE INPUT === */
            if(gfx_event_waiting()) {
                char key = gfx_wait();
                mx = gfx_xpos();
                my = gfx_ypos();

                if(key == 1) {
                    /* --- Mouse click handling --- */

                    /* Menu button */
                    if(is_click_in_rect(mx, my, menu_btn_x, menu_btn_y,
                                         menu_btn_w, menu_btn_h)) {
                        return;
                    }

                    /* Click on scrambled tiles (to select) */
                    for(int i = 0; i < word_len; i++) {
                        if(!tile_selected[i] &&
                            is_click_in_rect(mx, my, scramble_x[i], scramble_y[i],
                                             TILE_SIZE, TILE_SIZE)) {
                            tile_selected[i] = 1;
                            answer_slots[answer_count] = scrambled[i];
                            answer_count++;
                            if(answer_count == word_len) pending_auto_check = 1;
                            break;
                        }
                    }

                    /* Click on answer slots (to deselect/return) */
                    for(int i = 0; i < answer_count; i++) {
                        if(is_click_in_rect(mx, my, slot_x[i], slot_y[i],
                                             TILE_SIZE, TILE_SIZE)) {
                            char removed_letter = answer_slots[i];
                            for(int j = 0; j < word_len; j++) {
                                if(tile_selected[j] &&
                                    scrambled[j] == removed_letter) {
                                    int count_before = 0;
                                    int slot_count = 0;
                                    for(int k = 0; k < j; k++) {
                                        if(tile_selected[k] &&
                                            scrambled[k] == removed_letter) {
                                            count_before++;
                                        }
                                    }
                                    for(int k = 0; k < i; k++) {
                                        if(answer_slots[k] == removed_letter) {
                                            slot_count++;
                                        }
                                    }
                                    if(count_before == slot_count) {
                                        tile_selected[j] = 0;
                                        break;
                                    }
                                }
                            }
                            for(int k = i; k < answer_count - 1; k++) {
                                answer_slots[k] = answer_slots[k + 1];
                            }
                            answer_count--;
                            answer_slots[answer_count] = '\0';
                            break;
                        }
                    }
                } else if(key == 8 || key == 127) {
                    /* --- Backspace: remove last placed letter --- */
                    if(answer_count > 0) {
                        char removed_letter = answer_slots[answer_count - 1];
                        /* Find the corresponding scrambled tile to deselect */
                        int found = 0;
                        for(int j = word_len - 1; j >= 0; j--) {
                            if(tile_selected[j] &&
                                scrambled[j] == removed_letter && !found) {
                                int match_count = 0;
                                int slot_count = 0;
                                for(int k = 0; k <= j; k++) {
                                    if(tile_selected[k] &&
                                        scrambled[k] == removed_letter) {
                                        match_count++;
                                    }
                                }
                                for(int k = 0; k < answer_count - 1; k++) {
                                    if(answer_slots[k] == removed_letter) {
                                        slot_count++;
                                    }
                                }
                                if(match_count > slot_count) {
                                    tile_selected[j] = 0;
                                    found = 1;
                                }
                            }
                        }
                        answer_count--;
                        answer_slots[answer_count] = '\0';
                    }
                } else if(key == 13 || key == 10) {
                    /* --- Enter: submit answer if all slots filled --- */
                    if(answer_count == word_len) {
                        char guess[MAX_WORD_LEN];
                        strncpy(guess, answer_slots, word_len);
                        guess[word_len] = '\0';
                        if(check_guess(guess, word)) {
                            result = 1;
                        } else {
                            result = -1;
                        }
                    }
                } else if(isalpha(key)) {
                    /* --- Keyboard letter: select matching scrambled tile --- */
                    char upper = toupper(key);
                    for(int i = 0; i < word_len; i++) {
                        if(!tile_selected[i] &&
                            toupper(scrambled[i]) == upper) {
                            tile_selected[i] = 1;
                            answer_slots[answer_count] = scrambled[i];
                            answer_count++;
                            if(answer_count == word_len) pending_auto_check = 1;
                            break;
                        }
                    }
                }
            }

            usleep(30000); /* ~33 FPS */
        }

        /* === SHOW ROUND RESULT === */
        {
            char msg1[80] = "";
            char msg2[80] = "";
            int msg_r, msg_g, msg_b;

            if(result == 1) {
                sprintf(msg1, "CORRECT!");
                msg_r = 80; msg_g = 230; msg_b = 60;
                lives++;
                score += POINTS_PER_CORRECT;
                flash_tiles_color(slot_x, slot_y, word_len,
                                  80, 230, 60, 3);
            } else if(result == -1) {
                sprintf(msg1, "WRONG!");
                sprintf(msg2, "Answer: %s", word);
                msg_r = 220; msg_g = 55; msg_b = 55;
                lives--;
                flash_tiles_color(slot_x, slot_y, word_len,
                                  220, 55, 55, 3);
            } else {
                sprintf(msg1, "TIMES UP!");
                sprintf(msg2, "Answer: %s", word);
                msg_r = 230; msg_g = 150; msg_b = 35;
                lives--;
            }

            /* Result display */
            draw_mc_background();
            draw_grass_strip(0);
            draw_dirt_strip(WIN_H - 11);

            draw_pixel_shadow(WIN_W / 2, 220, msg1, 5,
                              msg_r, msg_g, msg_b);

            if(strlen(msg2) > 0) {
                draw_pixel_shadow(WIN_W / 2, 300, msg2, 2,
                                  200, 200, 220);
            }

            {
                char buf[60];
                sprintf(buf, "LIVES: %d   SCORE: %d", lives, score);
                draw_pixel_shadow(WIN_W / 2, 360, buf, 2,
                                  180, 180, 200);
            }

            /* Decorative creeper for wrong answer */
            if(result != 1) {
                draw_creeper(WIN_W / 2 - 24, 410, 6);
            } else {
                draw_diamond(WIN_W / 2 - 8, 410, 3);
            }

            gfx_flush();
            usleep(1800000);
        }
    }

    /* === GAME OVER === */
    add_score(player_name, score, set_index);
    save_scores();

    {
        int choice = 0;
        gui_game_over(score, &choice);

        if(choice == 1) {
            int new_set = rand() % MAX_PUZZLES;
            gui_play_game(&puzzles[new_set], new_set, player_name);
        }
    }
}

/* ======================== GUI: GAME OVER ======================== */

void gui_game_over(int score, int *choice)
{
    int btn_w = 220;
    int btn_h = 46;
    int mx, my;
    char buf[60];
    int replay_x = WIN_W / 2 - btn_w - 15;
    int menu_x = WIN_W / 2 + 15;
    int btn_y = 420;

    while(1) {
        draw_mc_background();
        draw_grass_strip(0);
        draw_dirt_strip(WIN_H - 11);

        /* GAME OVER title */
        draw_pixel_shadow(WIN_W / 2, 120, "GAME OVER", 5,
                          220, 55, 55);

        /* Creeper face */
        draw_creeper(WIN_W / 2 - 32, 200, 8);

        /* Final score with diamonds */
        sprintf(buf, "SCORE: %d", score);
        {
            int text_y = 296;
            int scale_d = 3;
            int sw = pixel_string_width(buf, scale_d);
            int diamond_w = 5 * scale_d;
            int text_h = 7 * scale_d;
            int diamond_h = 8 * scale_d;
            int gap = 10;
            int half_text = sw / 2;
            int diamond_y = text_y + text_h / 2 - diamond_h / 2;
            draw_diamond(WIN_W / 2 - half_text - gap - diamond_w, diamond_y, scale_d);
            draw_pixel_shadow(WIN_W / 2, text_y, buf, scale_d,
                              255, 215, 0);
            draw_diamond(WIN_W / 2 + half_text + gap, diamond_y, scale_d);
        }

        /* Buttons */
        draw_mc_button_color(replay_x, btn_y, btn_w, btn_h,
                             "REPLAY", 80, 145, 55);
        draw_mc_button_color(menu_x, btn_y, btn_w, btn_h,
                             "MAIN MENU", 100, 100, 140);

        gfx_flush();

        gfx_wait();
        mx = gfx_xpos();
        my = gfx_ypos();

        if(is_click_in_rect(mx, my, replay_x, btn_y, btn_w, btn_h)) {
            *choice = 1;
            return;
        }
        if(is_click_in_rect(mx, my, menu_x, btn_y, btn_w, btn_h)) {
            *choice = 2;
            return;
        }
    }
}

/* ======================== GUI: ERROR SCREEN ======================== */

void gui_error_screen(const char *msg)
{
    int btn_w = 160;
    int btn_h = 46;
    int btn_x = WIN_W / 2 - btn_w / 2;
    int btn_y = 440;
    int mx, my;

    while(1) {
        draw_mc_background();
        draw_grass_strip(0);
        draw_dirt_strip(WIN_H - 11);

        /* Error title */
        draw_pixel_shadow(WIN_W / 2, 120, "ERROR", 5,
                          220, 55, 55);

        /* Creeper face */
        draw_creeper(WIN_W / 2 - 32, 200, 8);

        /* Error message */
        gfx_color(200, 200, 220);
        draw_centered_text(WIN_W / 2, 330, msg);

        gfx_color(140, 140, 160);
        draw_centered_text(WIN_W / 2, 360,
                           "Make sure puzzle .txt files are in src/puzzle/");

        /* Back button */
        draw_mc_button_color(btn_x, btn_y, btn_w, btn_h,
                             "BACK", 100, 100, 140);

        gfx_flush();

        gfx_wait();
        mx = gfx_xpos();
        my = gfx_ypos();

        if(is_click_in_rect(mx, my, btn_x, btn_y, btn_w, btn_h)) {
            return;
        }
    }
}

/* ======================== SCORE MANAGEMENT ======================== */

void add_score(const char *name, int score, int set_index)
{
    int i, j;
    ScoreRecord new_rec;

    strncpy(new_rec.name, name, 29);
    new_rec.name[29] = '\0';
    new_rec.score = score;
    new_rec.set_used = set_index;

    for(i = 0; i < num_scores; i++) {
        if(score > high_scores[i].score) {
            break;
        }
    }

    if(num_scores < MAX_SCORES) {
        num_scores++;
    }
    for(j = num_scores - 1; j > i; j--) {
        high_scores[j] = high_scores[j - 1];
    }

    if(i < MAX_SCORES) {
        high_scores[i] = new_rec;
    }
}

void save_scores(void)
{
    FILE *fp;
    fp = fopen("src/savedata/scores.dat", "w");
    if(fp == NULL) return;
    fprintf(fp, "%d\n", num_scores);
    for(int i = 0; i < num_scores; i++) {
        fprintf(fp, "%s\n%d\n%d\n",
                high_scores[i].name,
                high_scores[i].score,
                high_scores[i].set_used);
    }
    fclose(fp);
}

void load_scores(void)
{
    FILE *fp;
    int n;
    char line[64];
    fp = fopen("src/savedata/scores.dat", "r");
    if(fp == NULL) return;
    if(fgets(line, sizeof(line), fp) == NULL) { fclose(fp); return; }
    n = atoi(line);
    if(n < 0) n = 0;
    if(n > MAX_SCORES) n = MAX_SCORES;
    num_scores = 0;
    for(int i = 0; i < n; i++) {
        if(fgets(line, sizeof(line), fp) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(high_scores[i].name, line, 29);
        high_scores[i].name[29] = '\0';
        if(fgets(line, sizeof(line), fp) == NULL) break;
        high_scores[i].score = atoi(line);
        if(fgets(line, sizeof(line), fp) == NULL) break;
        high_scores[i].set_used = atoi(line);
        num_scores++;
    }
    fclose(fp);
}
