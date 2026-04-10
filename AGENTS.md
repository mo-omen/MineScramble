# Repository Guidelines

## Project Structure & Module Organization
This is a small C/X11 game with most code at the repository root. `game.c` contains the main game loop, UI flow, puzzle loading, and scoring. `gfx.c`/`gfx.h` provide the X11 drawing wrapper, and `audio_sys.c`/`audio_sys.h` handle music and sound effects through `miniaudio.h`. Runtime content lives under `src/`: puzzle word lists are in `src/puzzle/`, audio assets are in `src/sounds/`, and save data is written to `src/savedata/scores.dat` at runtime.

## Build, Test, and Development Commands
There is no `Makefile` yet; build directly with `gcc`:

```bash
gcc game.c gfx.c audio_sys.c -o game.o -lX11 -lm -lpthread -ldl
./game.o
```

Use the first command to compile the full game and the second to launch it in an X11-capable Linux environment. If you add new source files, include them explicitly in the compile command. Generated binaries and object files are ignored by `.gitignore`.

## Coding Style & Naming Conventions
Follow the existing C style: 4-space indentation, opening braces on the same line, and sectioned comments for major blocks. Keep macros uppercase (`MAX_WORDS`), use `snake_case` for functions and variables (`load_all_puzzles`, `current_bgm`), and prefer short, file-local `static` helpers when logic does not need external linkage. Keep asset and puzzle filenames lowercase with hyphens or numeric suffixes, for example `bg-sweden.wav` or `puzzle3.txt`.

## Testing Guidelines
This repository does not currently include an automated test suite. Validate changes by rebuilding cleanly and manually exercising the affected flow in the running game: menu navigation, puzzle loading, scoring, leaderboard persistence, and audio playback. When fixing bugs, document the manual test case in your PR description.

## Commit & Pull Request Guidelines
Recent history uses short, imperative commits, sometimes with prefixes such as `feat:` or `Fix:`. Prefer consistent lowercase prefixes, for example `feat: add new puzzle set` or `fix: prevent empty score entries`. Pull requests should include a brief summary, manual test notes, and screenshots or short recordings for UI changes. Link the related task or issue when one exists.
