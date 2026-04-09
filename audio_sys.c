#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "audio_sys.h"
#include "gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static ma_engine engine;
static ma_sound bgm_sound;
static int audio_initialized = 0;

static const char* bgm_files[] = {
    "src/sounds/bg-haggstrom.wav",
    "src/sounds/bg-mice-on-venus.mp3",
    "src/sounds/bg-minecraft.wav",
    "src/sounds/bg-subwoofer-lullaby.mp3",
    "src/sounds/bg-sweden.wav"
};
static const int num_bgm = 5;
static int current_bgm = 0;

static time_t popup_show_time = 0;

static char track_popup_buf[256];
static int track_popup_w = 300;
static int track_popup_x = 0;
static int track_popup_y = 0;

extern void draw_pixel_string(int x, int y, const char *text, int scale);
extern void draw_pixel_shadow(int cx, int y, const char *str, int scale, int r, int g, int b);
extern void draw_filled_rect(int x, int y, int w, int h, int r, int g, int b);
extern void draw_rect(int x, int y, int w, int h, int r, int g, int b);

static void update_popup_info(void) {
    const char* f = bgm_files[current_bgm];
    const char* name = strrchr(f, '/');
    if (name) name++; else name = f;
    
    snprintf(track_popup_buf, sizeof(track_popup_buf), "NOW PLAYING: %s", name);
    track_popup_w = strlen(track_popup_buf) * 12 + 24;
    if (track_popup_w < 320) track_popup_w = 320;
    track_popup_x = 1024 - track_popup_w - 20;
    track_popup_y = 720 - 48 - 20;
}

static void audio_play_current_bgm(void) {
    if (!audio_initialized) return;
    
    if (ma_sound_is_playing(&bgm_sound)) {
        ma_sound_stop(&bgm_sound);
    }
    ma_sound_uninit(&bgm_sound);
    ma_sound_init_from_file(&engine, bgm_files[current_bgm], MA_SOUND_FLAG_STREAM, NULL, NULL, &bgm_sound);
    ma_sound_start(&bgm_sound);
    popup_show_time = time(NULL);
    update_popup_info();
}

void audio_init(void) {
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        printf("Failed to initialize audio engine.\n");
        return;
    }
    audio_initialized = 1;
    ma_sound_init_from_file(&engine, bgm_files[current_bgm], MA_SOUND_FLAG_STREAM, NULL, NULL, &bgm_sound);
    ma_sound_start(&bgm_sound);
    popup_show_time = time(NULL);
    update_popup_info();
}

void audio_cleanup(void) {
    if (!audio_initialized) return;
    ma_sound_uninit(&bgm_sound);
    ma_engine_uninit(&engine);
}

void audio_tick(void) {
    if (!audio_initialized) return;
    if (!ma_sound_is_playing(&bgm_sound) && ma_sound_at_end(&bgm_sound)) {
        audio_next_track();
    }
}

void audio_play_sfx_click(void) {
    if (!audio_initialized) return;
    ma_engine_play_sound(&engine, "src/sounds/sfx-click.wav", NULL);
}

void audio_play_sfx_letter(void) {
    if (!audio_initialized) return;
    ma_engine_play_sound(&engine, "src/sounds/sfx-letter.wav", NULL);
}

void audio_play_sfx_remove(void) {
    if (!audio_initialized) return;
    ma_engine_play_sound(&engine, "src/sounds/sfx-remove.wav", NULL);
}

void audio_next_track(void) {
    current_bgm = (current_bgm + 1) % num_bgm;
    audio_play_current_bgm();
}

void audio_prev_track(void) {
    current_bgm = (current_bgm - 1 + num_bgm) % num_bgm;
    audio_play_current_bgm();
}

int audio_popup_active(void) {
    return (time(NULL) - popup_show_time < 3);
}

void audio_draw_popup(void) {
    if (audio_popup_active()) {
        int h = 48;
        
        // Draw Minecraft-style panel manually
        draw_filled_rect(track_popup_x, track_popup_y, track_popup_w, h, 198, 198, 198);
        draw_filled_rect(track_popup_x, track_popup_y, track_popup_w, 3, 255, 255, 255);
        draw_filled_rect(track_popup_x, track_popup_y, 3, h, 255, 255, 255);
        draw_filled_rect(track_popup_x, track_popup_y + h - 3, track_popup_w, 3, 85, 85, 85);
        draw_filled_rect(track_popup_x + track_popup_w - 3, track_popup_y, 3, h, 85, 85, 85);

        // Draw text with shadow
        draw_pixel_shadow(track_popup_x + track_popup_w / 2, track_popup_y + (h - 14) / 2, 
                          track_popup_buf, 2, 255, 255, 0);
    }
}
