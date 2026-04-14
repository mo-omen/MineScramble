# MineScramble - Project Context for Gemini

## Project Overview

**MineScramble** is a graphical word unscrambling game written in C using X11 for rendering and `miniaudio` for music and sound effects. The interface is hand-drawn with primitive graphics functions and follows a Minecraft-inspired visual style with blocky text, panel framing, and game-like menu flows.

## Current Feature Set

- Survival mode with three puzzle-set difficulty choices
- Creative mode with Marathon, Zen, and Sudden Death variants
- Persistent leaderboard categories stored locally
- Hint system with mode-specific rules
- Background music controls and sound-effect toggles
- Preloaded short SFX for low-latency UI and gameplay feedback

## Directory Structure and Key Files

- `game.c`: Main game flow, menu logic, gameplay loop, puzzle management, hints, and leaderboard handling
- `gfx.c` / `gfx.h`: X11 drawing and input wrapper
- `audio_sys.c` / `audio_sys.h`: Audio initialization, BGM switching, SFX playback, and popup drawing
- `src/puzzle/`: Word lists for the three puzzle groups
- `src/sounds/`: Background tracks and effect files
- `src/savedata/`: Location of the generated `scores.dat` leaderboard file
- `Documentation.md`: Human-oriented project documentation and gameplay summary

## Building and Running

Compile:

```bash
gcc game.c gfx.c audio_sys.c -o game.o -lX11 -lm -lpthread -ldl
```

Run:

```bash
./game.o
```

## Runtime Notes

- The game requires an X11-capable Linux environment
- BGM and SFX both start enabled for each launch
- Audio toggles are session-only
- Background tracks are streamed
- SFX assets are preloaded during `audio_init()` to reduce click delay

## Architecture Notes

- Most gameplay logic is centralized in `game.c`
- UI rendering is custom and built from low-level primitives rather than image assets
- Shared data is bounded by fixed macros such as `MAX_WORDS`, `MAX_WORD_LEN`, `MAX_LETTERS`, and `MAX_SCORES`
- Input supports mouse-driven interaction plus several keyboard shortcuts during gameplay and audio control
- The codebase is procedural C with a small module split: gameplay, graphics wrapper, and audio wrapper

## Key Gameplay Concepts

- Survival uses one selected puzzle set and timed rounds with lives
- Creative Marathon uses a single global timer
- Creative Zen removes timer pressure
- Creative Sudden Death ends on the first failure
- Hints reveal one correct slot and follow per-mode cost rules

## Maintenance Guidance

- Prefer updating the explicit `gcc` compile line when adding source files
- Keep puzzle and sound asset paths in sync with the hardcoded runtime references
- `patch1.sh` is obsolete and should not be reintroduced; `game.c` already includes `audio_sys.h`
