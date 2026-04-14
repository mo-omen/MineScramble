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
static ma_sound sfx_click_sound;
static ma_sound sfx_letter_sound;
static ma_sound sfx_remove_sound;
static ma_sound sfx_hint_sound;
static int audio_initialized = 0;
static int bgm_loaded = 0;
static int sfx_click_loaded = 0;
static int sfx_letter_loaded = 0;
static int sfx_remove_loaded = 0;
static int sfx_hint_loaded = 0;
static int bgm_enabled = 1;
static int sfx_enabled = 1;

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
extern void draw_pixel_shadow(int cx, int y, const char *str, int scale,
                              int r, int g, int b);
extern void draw_filled_rect(int x, int y, int w, int h);

static void update_popup_info(void)
{
    const char* f = bgm_files[current_bgm];
    const char* name = strrchr(f, '/');
    if(name) name++; else name = f;

    snprintf(track_popup_buf, sizeof(track_popup_buf), "NOW PLAYING: %s", name);
    track_popup_w = (int)strlen(track_popup_buf) * 12 + 24;
    if(track_popup_w < 320) track_popup_w = 320;
    track_popup_x = 1024 - track_popup_w - 20;
    track_popup_y = 720 - 48 - 20;
}

static void audio_apply_bgm_state(void)
{
    if(!audio_initialized || !bgm_loaded) return;

    ma_sound_set_volume(&bgm_sound, 1.0f);
    if(bgm_enabled) {
        if(!ma_sound_is_playing(&bgm_sound)) {
            ma_sound_start(&bgm_sound);
        }
    } else if(ma_sound_is_playing(&bgm_sound)) {
        ma_sound_stop(&bgm_sound);
    }
}

static void audio_play_current_bgm(void)
{
    if(!audio_initialized) return;

    if(bgm_loaded && ma_sound_is_playing(&bgm_sound)) {
        ma_sound_stop(&bgm_sound);
    }
    if(bgm_loaded) {
        ma_sound_uninit(&bgm_sound);
    }
    if(ma_sound_init_from_file(&engine, bgm_files[current_bgm],
                               MA_SOUND_FLAG_STREAM, NULL, NULL,
                               &bgm_sound) != MA_SUCCESS) {
        bgm_loaded = 0;
        return;
    }

    bgm_loaded = 1;
    audio_apply_bgm_state();
    popup_show_time = time(NULL);
    update_popup_info();
}

static int audio_load_sfx(ma_sound *sound, const char *path)
{
    return ma_sound_init_from_file(&engine, path, 0, NULL, NULL, sound) == MA_SUCCESS;
}

static void audio_play_sfx(ma_sound *sound, int loaded)
{
    if(!audio_initialized || !sfx_enabled || !loaded) return;

    ma_sound_stop(sound);
    ma_sound_seek_to_pcm_frame(sound, 0);
    ma_sound_start(sound);
}

void audio_init(void)
{
    if(ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        printf("Failed to initialize audio engine.\n");
        return;
    }
    bgm_enabled = 1;
    sfx_enabled = 1;
    current_bgm = 0;
    bgm_loaded = 0;
    sfx_click_loaded = audio_load_sfx(&sfx_click_sound, "src/sounds/sfx-click.wav");
    sfx_letter_loaded = audio_load_sfx(&sfx_letter_sound, "src/sounds/sfx-letter.wav");
    sfx_remove_loaded = audio_load_sfx(&sfx_remove_sound, "src/sounds/sfx-remove.wav");
    sfx_hint_loaded = audio_load_sfx(&sfx_hint_sound, "src/sounds/sfx-hint.wav");
    audio_initialized = 1;
    audio_play_current_bgm();
}

void audio_cleanup(void)
{
    if(!audio_initialized) return;
    if(bgm_loaded) {
        ma_sound_uninit(&bgm_sound);
    }
    if(sfx_click_loaded) {
        ma_sound_uninit(&sfx_click_sound);
    }
    if(sfx_letter_loaded) {
        ma_sound_uninit(&sfx_letter_sound);
    }
    if(sfx_remove_loaded) {
        ma_sound_uninit(&sfx_remove_sound);
    }
    if(sfx_hint_loaded) {
        ma_sound_uninit(&sfx_hint_sound);
    }
    ma_engine_uninit(&engine);
    audio_initialized = 0;
}

void audio_tick(void)
{
    if(!audio_initialized || !bgm_loaded || !bgm_enabled) return;
    if(!ma_sound_is_playing(&bgm_sound) && ma_sound_at_end(&bgm_sound)) {
        current_bgm = (current_bgm + 1) % num_bgm;
        audio_play_current_bgm();
    }
}

void audio_play_sfx_click(void)
{
    audio_play_sfx(&sfx_click_sound, sfx_click_loaded);
}

void audio_play_sfx_letter(void)
{
    audio_play_sfx(&sfx_letter_sound, sfx_letter_loaded);
}

void audio_play_sfx_remove(void)
{
    audio_play_sfx(&sfx_remove_sound, sfx_remove_loaded);
}

void audio_play_sfx_hint(void)
{
    audio_play_sfx(&sfx_hint_sound, sfx_hint_loaded);
}

void audio_toggle_bgm(void)
{
    bgm_enabled = !bgm_enabled;
    audio_apply_bgm_state();
}

int audio_bgm_enabled(void)
{
    return bgm_enabled;
}

void audio_toggle_sfx(void)
{
    sfx_enabled = !sfx_enabled;
}

int audio_sfx_enabled(void)
{
    return sfx_enabled;
}

void audio_next_bgm(void)
{
    if(!audio_initialized) return;
    current_bgm = (current_bgm + 1) % num_bgm;
    audio_play_current_bgm();
}

void audio_prev_bgm(void)
{
    if(!audio_initialized) return;
    current_bgm = (current_bgm - 1 + num_bgm) % num_bgm;
    audio_play_current_bgm();
}

int audio_popup_active(void)
{
    return (time(NULL) - popup_show_time < 3);
}

void audio_draw_popup(void)
{
    if(audio_popup_active()) {
        int h = 48;

        gfx_color(198, 198, 198);
        draw_filled_rect(track_popup_x, track_popup_y, track_popup_w, h);
        gfx_color(255, 255, 255);
        draw_filled_rect(track_popup_x, track_popup_y, track_popup_w, 3);
        draw_filled_rect(track_popup_x, track_popup_y, 3, h);
        gfx_color(85, 85, 85);
        draw_filled_rect(track_popup_x, track_popup_y + h - 3, track_popup_w, 3);
        draw_filled_rect(track_popup_x + track_popup_w - 3, track_popup_y, 3, h);

        draw_pixel_shadow(track_popup_x + track_popup_w / 2,
                          track_popup_y + (h - 14) / 2,
                          track_popup_buf, 2, 255, 255, 0);
    }
}
