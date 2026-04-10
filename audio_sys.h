#ifndef AUDIO_SYS_H
#define AUDIO_SYS_H

void audio_init(void);
void audio_cleanup(void);
void audio_tick(void);
void audio_play_sfx_click(void);
void audio_play_sfx_letter(void);
void audio_play_sfx_remove(void);
void audio_toggle_bgm(void);
int audio_bgm_enabled(void);
void audio_toggle_sfx(void);
int audio_sfx_enabled(void);

// For drawing the popup
void audio_draw_popup(void);
int audio_popup_active(void);

#endif
