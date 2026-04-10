## 1. Project Summary

**Project name:** MineScramble

**Type of program:** A Minecraft-themed word-unscrambling game written in C with X11 graphics and `miniaudio` audio playback.

**Main idea of the game:**
- Load words from puzzle text files.
- Scramble one word at a time.
- Let the player rebuild the original word using letter tiles.
- Award points for correct answers.
- Track separate leaderboards for Survival, Marathon, Zen Mode, and Sudden Death.

## 2. Main Files

`game.c`
- Main game logic.
- Menus, gameplay loop, hint system, score saving, leaderboard rendering, and custom UI drawing.

`gfx.c` / `gfx.h`
- Lightweight X11 wrapper.
- Opens the window, draws lines/text, and reads keyboard or mouse input.

`audio_sys.c` / `audio_sys.h`
- Background music, sound effects, runtime BGM/SFX toggles, and the "NOW PLAYING" popup.
- Audio always starts with BGM on and SFX on by default for each launch.

`src/puzzle/puzzle1.txt`, `puzzle2.txt`, `puzzle3.txt`
- Source word lists for the three Survival difficulty sets.

`src/savedata/scores.dat`
- Leaderboard save file.

## 3. Build and Run

```bash
gcc game.c gfx.c audio_sys.c -o game.o -lX11 -lm -lpthread -ldl
./game.o
```

Notes:
- `-lX11` links the graphics backend.
- `-lpthread -ldl` are required by `miniaudio`.
- The game needs an X11-capable Linux environment.

## 4. Current Program Flow

1. `main()` seeds randomness with `srand(time(NULL))`.
2. `audio_init()` starts the audio engine and first BGM track.
3. `load_all_puzzles()` loads puzzle words into memory.
4. `build_creative_pool()` merges all loaded words into one Creative pool without duplicates.
5. `load_scores()` loads saved leaderboard data.
6. The graphics window opens.
7. The main menu loop runs until the player quits.

If the player chooses Play:
- enter player name
- choose `SURVIVAL` or `CREATIVE`
- Survival opens difficulty select
- Creative opens mode select
- gameplay begins through `gui_play_game(...)`

Other main-menu paths:
- `HOW TO PLAY` opens the paged help screen
- `OPTIONS` opens session-only BGM/SFX controls
- `LEADERBOARD` opens the score tabs

## 5. Main Data Structures

`PuzzleSet`
- Stores words for one puzzle file.

`WordPool`
- Generic gameplay word source.
- Used so Survival and Creative can both feed words into the same game loop.

`ScoreRecord`
- Stores player name, score, selected set, and leaderboard category.

`RunMode`
- `RUN_MODE_SURVIVAL`
- `RUN_MODE_CREATIVE_MARATHON`
- `RUN_MODE_CREATIVE_ZEN`
- `RUN_MODE_CREATIVE_SUDDEN_DEATH`

`ScoreCategory`
- Separate leaderboard buckets for Survival and each Creative mode.

## 6. Gameplay Rules

### Survival
- Uses one selected puzzle set.
- Timer enabled.
- Lives enabled.
- Peaceful uses `10` seconds per word.
- Survival uses `15` seconds per word.
- Hardcore uses `20` seconds per word.
- Correct answers give `+10` score and may award a life.

### Timed Marathon
- Uses the mixed Creative pool.
- Uses one shared `120` second timer for the whole run.
- Always shows `6` lives and never changes them.
- Correct answers still give `+10` score.

### Zen Mode
- Uses the mixed Creative pool.
- No timer.
- Wrong answers reveal the correct word and continue.
- No life loss.

### Sudden Death
- Uses the mixed Creative pool.
- Uses `25` seconds per word.
- Starts with one life.
- First wrong answer or timeout ends the run.

## 7. Hint System

Hints reveal one correct answer slot by consuming the matching scrambled tile.

Rules:
- hints are disabled in Sudden Death
- the maximum hints per round is **half of the word length**
- paid modes require at least `5` score before each hint use
- Survival and Marathon hints cost `5` each
- Zen hints are free

Examples:
- 5-letter word -> max `2` hints
- 6-letter word -> max `3` hints
- 8-letter word -> max `4` hints

Implementation notes:
- `apply_hint()` inserts the correct letter into the correct slot
- `try_use_hint()` checks availability and cost, then plays `sfx-hint.wav`
- keyboard hint hotkey is `\` so it does not conflict with answer letters

## 8. Menu and UI Notes

- The main menu now contains `PLAY GAME`, `HOW TO PLAY`, `OPTIONS`, `LEADERBOARD`, and `QUIT GAME`.
- `OPTIONS` reuses the pause-menu style but only shows BGM and SFX toggles.
- The help screen is paged with left/right arrow buttons so text stays inside the panel.
- The game-over replay action is labeled `RESPAWN`.
- Player names are uppercased before saving so the leaderboard matches the game font.
- `\` triggers a hint from the keyboard when hints are allowed.
- `[` and `]` skip to the previous or next BGM track.

## 9. Audio Behavior

The audio system provides:
- looping background music
- click, letter, remove, and hint sound effects
- BGM on/off toggle
- SFX on/off toggle
- a popup showing the current music track
- keyboard BGM skipping with `[` and `]`

Important behavior:
- BGM starts ON by default every launch
- SFX starts ON by default every launch
- audio settings are session-only and are not saved to disk
- the hint SFX path is `src/sounds/sfx-hint.wav`

## 10. Leaderboard Saving

Scores are stored in category order using a `V2` save format.

The save file stores:
- a version line
- number of scores in each category
- each `ScoreRecord`

`load_scores()` is backward compatible with the older format and loads old records into the Survival leaderboard.
Loaded and newly entered names are normalized to uppercase for visual consistency.

## 11. Important Functions

`main()`
- top-level startup and menu flow

`load_all_puzzles()`
- reads puzzle files into `puzzles[]`

`build_creative_pool()`
- creates the mixed Creative word pool

`scramble_word()`
- shuffles a word until it differs from the original

`pick_word()`
- chooses one unused word index

`apply_hint()`
- inserts one correct hinted letter into the answer

`try_use_hint()`
- enforces hint rules and updates score or hint count

`gui_play_game()`
- runs the main gameplay session

`gui_options_menu()`
- shows main-menu audio toggles

`add_score()`, `save_scores()`, `load_scores()`
- handle leaderboard management and persistence
