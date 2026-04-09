# MineScramble - Project Context for Gemini

## Project Overview
**MineScramble** is a graphical word unscrambling game written in C, built on top of the X11 graphics library. It features a Minecraft-inspired user interface (with elements like blocky pixel fonts, creeper faces, hearts, and dirt/grass themes) created manually using primitive graphics drawing functions rather than loading images.

**Key Features:**
- Players unscramble words within a time limit.
- Points and lives are awarded for correct answers.
- Words are loaded from external text files grouped by difficulty (`Peaceful`, `Survival`, `Hardcore`).
- Leaderboards are persistent and saved locally to a file.
- Custom rendering system for the UI and text.

## Directory Structure and Key Files
- `game.c`: The core logic file handling the game loop, UI rendering, file loading, scrambling, input, and scoring.
- `gfx.c` / `gfx.h`: A simple, provided graphics library built on X11 that handles opening the window, drawing primitives, and reading events.
- `src/puzzle/`: Contains text files (`puzzle1.txt`, `puzzle2.txt`, `puzzle3.txt`) defining the words for the three difficulty tiers.
- `src/savedata/`: Directory where the `scores.dat` leaderboard file is created and stored.
- `src/sounds/`: Contains background music and sound effects (`.wav`, `.mp3`). 
- `Documentation.md`: Extensive manual documenting program flow, constants, global variables, and game limitations.

## Building and Running

**Dependencies:**
- Linux or a Linux-like environment.
- X11 graphical support (an active display is required).
- `gcc` compiler and `libm` (math library).

**Compile Command:**
```bash
gcc game.c gfx.c audio_sys.c -o game.o -lX11 -lm -lpthread -ldl
```

**Run Command:**
```bash
./game.o
```

## Development Conventions & Architecture
- **Monolithic Game Logic:** The majority of the game mechanics (input processing, game loop, UI layout) are contained within `game.c`.
- **Custom UI Rendering:** The game draws UI elements using raw graphics primitives (`gfx_line`, `gfx_point`, `gfx_color`) to simulate a Minecraft aesthetic, including custom pixel font rendering routines.
- **Fixed Limits:** Uses C preprocessor macros for constraints (e.g., `MAX_WORDS 40`, `MAX_WORD_LEN 20`, `MAX_LETTERS 16`, `MAX_SCORES 10`).
- **Input Handling:** Supports both mouse clicks (via geometric bounds checking `is_click_in_rect`) and keyboard inputs.
- **Timing:** Uses standard C time functions (`time(NULL)`) for round limits and elapsed time calculations. No complex physics or animation engines are used.
- **Style:** Standard procedural C coding conventions, utilizing basic C libraries (`stdio.h`, `stdlib.h`, `string.h`, `time.h`, `ctype.h`).
