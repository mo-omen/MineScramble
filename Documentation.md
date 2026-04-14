## 1. Project Summary

**Project name:** MineScramble

**Type of program:** A Minecraft-themed word unscrambling game written in C with X11 graphics and `miniaudio` audio playback.

**Main idea of the game:**
- Load words from external puzzle files
- Scramble one word at a time
- Let the player reconstruct the original word from clickable tiles
- Award points based on successful answers
- Store local leaderboard entries across multiple game categories

## 2. Main Files

`game.c`
- Main gameplay logic, menus, puzzle loading, input handling, hints, score flow, and leaderboard UI

`gfx.c` / `gfx.h`
- Minimal X11 rendering and input wrapper used by the game UI

`audio_sys.c` / `audio_sys.h`
- Audio engine setup, music switching, sound-effect playback, audio toggles, and the "NOW PLAYING" popup
- Short sound effects are preloaded at startup so clicks and tile actions do not incur per-use file load delay

`src/puzzle/puzzle1.txt`, `puzzle2.txt`, `puzzle3.txt`
- Source word sets used by Survival and combined into the Creative pool

`src/sounds/`
- Background tracks and effect assets

`src/savedata/scores.dat`
- Local persistent leaderboard data

## 3. Build and Run

```bash
gcc game.c gfx.c audio_sys.c -o game.o -lX11 -lm -lpthread -ldl
./game.o
```

Notes:
- `-lX11` links the graphics backend
- `-lpthread` and `-ldl` are needed by `miniaudio`
- The game requires an X11-capable Linux environment

## 4. Startup Flow

1. `main()` seeds randomness with `srand(time(NULL))`
2. `audio_init()` starts the audio engine, preloads SFX, and starts the first BGM track
3. `load_all_puzzles()` loads puzzle files into memory
4. `build_creative_pool()` merges unique words into the Creative pool
5. `load_scores()` restores saved leaderboard entries
6. `gfx_open()` initializes the X11 window
7. The main menu loop runs until the player quits

Main menu branches:
- `PLAY GAME`
- `HOW TO PLAY`
- `OPTIONS`
- `LEADERBOARD`
- `QUIT GAME`

## 5. Main Data Structures

`PuzzleSet`
- Stores words loaded from one puzzle file

`WordPool`
- Shared gameplay pool abstraction used by Survival and Creative modes

`ScoreRecord`
- Stores player name, score, selected set, and leaderboard category

`RunMode`
- `RUN_MODE_SURVIVAL`
- `RUN_MODE_CREATIVE_MARATHON`
- `RUN_MODE_CREATIVE_ZEN`
- `RUN_MODE_CREATIVE_SUDDEN_DEATH`

`ScoreCategory`
- Separate leaderboard group for Survival and each Creative mode

## 6. Gameplay Rules

### Survival

- Uses one selected puzzle set
- Uses a per-word timer
- Uses the lives system
- Correct answers award `+10` points
- Correct answers can grant a life up to the mode limit

Difficulty timing:
- Peaceful: `10` seconds
- Survival: `15` seconds
- Hardcore: `20` seconds

### Creative Marathon

- Uses the merged Creative word pool
- Uses one shared `120` second timer for the whole run
- Shows `6` lives as a static display and does not consume them
- Correct answers award `+10` points

### Creative Zen

- Uses the merged Creative word pool
- Has no timer
- Wrong answers reveal the correct word and continue
- Does not use life loss

### Creative Sudden Death

- Uses the merged Creative word pool
- Uses `25` seconds per word
- Starts with one life
- The first wrong answer or timeout ends the session

## 7. Hint System

Hints reveal one correct answer slot and consume the corresponding scrambled tile.

Rules:
- Hints are disabled in Sudden Death
- Maximum hints for a word is half its length
- Survival and Marathon hints cost `5` score
- Zen hints are free
- Hint activation plays `src/sounds/sfx-hint.wav`

Examples:
- 5-letter word -> maximum `2` hints
- 6-letter word -> maximum `3` hints
- 8-letter word -> maximum `4` hints

Key functions:
- `apply_hint()`
- `try_use_hint()`

## 8. Controls and UI Notes

General:
- Mouse clicks are the primary interaction method
- `[` and `]` switch the previous or next BGM track

Gameplay:
- Click scrambled tiles to place letters
- Click answer slots to remove placed letters
- `Enter` submits an answer
- `Backspace` removes the most recent placed letter
- `\` triggers a hint when allowed

UI notes:
- The help screen uses paging controls
- The options menu exposes session-only BGM and SFX toggles
- The game-over replay action is labeled `RESPAWN`
- Player names are normalized to uppercase before leaderboard storage

## 9. Audio Behavior

The audio system provides:
- Rotating background music
- Click, letter, remove, and hint sound effects
- BGM on/off toggle
- SFX on/off toggle
- A popup showing the current active track
- Keyboard track skipping with `[` and `]`

Important behavior:
- BGM starts enabled on each launch
- SFX starts enabled on each launch
- Audio settings are not persisted
- Background tracks are streamed from disk
- Short SFX are preloaded at startup and replayed from memory for lower latency

## 10. Leaderboard Saving

Scores are stored in category order using a `V2` save format.

Saved data includes:
- A version line
- Score counts for each category
- Serialized `ScoreRecord` entries

`load_scores()` remains backward compatible with the older format and imports those records into the Survival leaderboard.

## 11. Important Functions

`main()`
- Startup, initialization, and top-level menu flow

`load_all_puzzles()`
- Reads puzzle files into `puzzles[]`

`build_creative_pool()`
- Builds the combined Creative word source

`scramble_word()`
- Shuffles a word until it differs from the original

`pick_word()`
- Chooses an unused word index

`apply_hint()`
- Inserts a correct hinted letter into the answer

`try_use_hint()`
- Validates hint rules and updates score or hint count

`gui_play_game()`
- Runs a gameplay session

`gui_options_menu()`
- Shows main-menu audio settings

`add_score()`, `save_scores()`, `load_scores()`
- Manage leaderboard data and persistence

## 12. Maintenance Notes

- The project is currently built directly with `gcc`; there is no `Makefile`
- `patch1.sh` is no longer required and has been removed from the repository
- Manual testing remains the primary validation method
