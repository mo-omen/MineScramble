# MineScramble

MineScramble is a Minecraft-flavored word unscrambling game written in C with X11 graphics and `miniaudio` audio playback. It mixes a hand-drawn pixel UI, rotating background music, responsive sound effects, persistent leaderboards, and multiple game modes built around the same core word puzzle loop.

## Highlights

- Survival and Creative mode families with different pacing rules
- Hand-drawn X11 interface with blocky text, panels, and Minecraft-inspired styling
- Background music with previous/next track controls and a "Now Playing" popup
- Preloaded sound effects for click, letter placement, tile removal, and hints
- Persistent leaderboard storage in `src/savedata/scores.dat`
- Puzzle content loaded from external text files under `src/puzzle/`

## Game Modes

### Survival

Choose a puzzle set and play with a per-word timer plus a lives system.

- `Peaceful`: 10 seconds per word
- `Survival`: 15 seconds per word
- `Hardcore`: 20 seconds per word

Correct answers award points and can restore lives up to the configured cap.

### Creative

Creative mode builds one merged pool from all puzzle files and offers three rule sets:

- `Marathon`: one shared 120-second run timer
- `Zen`: no timer, no life pressure
- `Sudden Death`: one life and a 25-second per-word limit

## Controls

### Menu / General

- Mouse click: interact with buttons and tiles
- `[` / `]`: switch to previous or next background track

### During Gameplay

- Click scrambled tiles to place letters
- Click answer slots to remove letters
- `Enter`: submit the current answer
- `Backspace`: remove the last placed letter
- `\\`: use a hint when the current mode allows it

## Project Layout

- `game.c`: main game loop, menus, puzzle flow, hints, scoring, and leaderboard logic
- `gfx.c`, `gfx.h`: X11 rendering and input wrapper
- `audio_sys.c`, `audio_sys.h`: music, sound effects, toggles, and track popup handling
- `src/puzzle/`: puzzle word lists
- `src/sounds/`: music and sound assets
- `src/savedata/scores.dat`: saved leaderboard file generated at runtime

## Build

Compile directly with `gcc`:

```bash
gcc game.c gfx.c audio_sys.c -o game.o -lX11 -lm -lpthread -ldl
```

Run it with:

```bash
./game.o
```

## Requirements

- Linux or Linux-compatible environment
- X11 display support
- `gcc`
- X11 development/runtime libraries
- pthread and dynamic loader support used by `miniaudio`

## Audio Notes

- BGM starts enabled on each launch
- SFX starts enabled on each launch
- Audio toggles are session-only and are not persisted
- Short sound effects are preloaded during startup so UI clicks play immediately instead of loading from disk per interaction

## Puzzle Content

Puzzle files are plain text word lists:

- `src/puzzle/puzzle1.txt`
- `src/puzzle/puzzle2.txt`
- `src/puzzle/puzzle3.txt`

These feed Survival directly and are also merged into the Creative pool.

## Leaderboards

Scores are saved locally in `src/savedata/scores.dat`. The game stores separate leaderboard categories for:

- Survival
- Creative Marathon
- Creative Zen
- Creative Sudden Death

