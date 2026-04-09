## 1. Project Summary

**Project name:** MineScramble

**Type of program:** A graphical word unscrambling game written in C using the X11 graphics library.

**Theme:** Minecraft-inspired user interface.

**Main idea of the game:**
- The program loads words from text files.
- It scrambles one word at a time.
- The player must rearrange the letters into the correct word before time runs out.
- The player gains points and lives for correct answers.
- The player loses a life for a wrong answer or if the timer reaches zero.
- The game ends when the player has no lives left, or when all words in the chosen set have been used.

## 2. What Files Exist and What They Do

### Main source files

`game.c`
- This is the main application file.
- It contains almost all game logic, screen drawing, input handling, scoring, puzzle loading, and the game flow.

`gfx.c`
- This is a provided graphics library built on top of X11.
- It handles low-level graphics tasks such as opening the window, drawing points and lines, reading mouse clicks, and reading keyboard input.

`gfx.h`
- Header file for `gfx.c`.
- It contains the function declarations for the graphics library.

### Puzzle files

`src/puzzle/puzzle1.txt`
- Easy words.
- Mostly 5 to 6 letters.

`src/puzzle/puzzle2.txt`
- Medium words.
- Mostly 6 to 7 letters.

`src/puzzle/puzzle3.txt`
- Hard words.
- Mostly 8 to 15 letters.

### Save-data folder

`src/savedata/scores.dat`
- This file is created and used to save leaderboard data.
- If it does not exist yet, the game simply starts with no saved scores.

## 3. How to Compile and Run

### Compile command

```bash
gcc game.c gfx.c audio_sys.c -o game.o -lX11 -lm -lpthread -ldl
```

### Why these flags are used

- `game.c gfx.c audio_sys.c`: compiles all source files including the new audio system.
- `-o game.o`: names the final executable `game.o`.
- `-lX11`: links the X11 graphics library.
- `-lm`: links the math library.
- `-lpthread -ldl`: links libraries required by miniaudio.

### Run command

```bash
./game.o
```

### Environment requirement

The program needs:
- Linux or a Linux-like environment
- X11 graphical support

If there is no graphical display, the game window cannot open.

## 4. Overall Program Flow

The flow of the program is:

1. `main()` starts the program.
2. The random number generator is seeded using the current time.
3. Puzzle files are loaded into memory.
4. Saved scores are loaded from `src/savedata/scores.dat`.
5. A graphics window is opened.
6. If no puzzle files are found, an error screen is shown.
7. Otherwise, the program repeatedly shows the main menu.
8. From the main menu, the user can:
- Play the game
- Read the help screen
- View the leaderboard
- Quit
9. If the player chooses to play:
- The player enters a name
- The player chooses a difficulty
- The game starts
10. When the game ends, the score is saved and the game-over screen is shown.

## 5. Important Constants and What They Mean

These constants are declared near the top of `game.c`.

- `WIN_W 1024`
  Window width in pixels.

- `WIN_H 720`
  Window height in pixels.

- `MAX_WORDS 40`
  Maximum number of words loaded from one puzzle file.

- `MAX_WORD_LEN 20`
  Maximum word length stored in the arrays.

- `MAX_PUZZLES 3`
  There are three difficulty sets.

- `MAX_LETTERS 16`
  Maximum letters handled visually in a word round.

- `TILE_SIZE 52`
  Size of each letter tile.

- `TILE_GAP 6`
  Spacing between tiles.

- `TIMER_MAX 10`
  Each round gives the player 10 seconds.

- `STARTING_LIVES 3`
  The player starts with 3 lives.

- `MAX_LIVES 6`
  The maximum lives a player can have.

- `POINTS_PER_CORRECT 10`
  Each correct word adds 10 points.

- `MAX_SCORES 10`
  The leaderboard stores only the top 10 scores.

## 6. Data Structures

### `PuzzleSet`

```c
typedef struct {
    char words[MAX_WORDS][MAX_WORD_LEN];
    int word_count;
} PuzzleSet;
```

Purpose:
- Stores all words for one difficulty level.

Fields:
- `words`: 2D character array storing many words.
- `word_count`: how many valid words were loaded.

### `ScoreRecord`

```c
typedef struct {
    char name[30];
    int score;
    int set_used;
} ScoreRecord;
```

Purpose:
- Stores one leaderboard entry.

Fields:
- `name`: player name.
- `score`: final score.
- `set_used`: which difficulty was used.

## 7. Global Variables

### Puzzle storage

```c
static PuzzleSet puzzles[MAX_PUZZLES];
```

This holds the three puzzle sets loaded from the text files.

### High-score storage

```c
static ScoreRecord high_scores[MAX_SCORES];
static int num_scores = 0;
```

These hold the leaderboard data currently in memory.

### Puzzle file paths

```c
static const char *puzzle_files[MAX_PUZZLES]
```

These are the paths to the three word-list files.

### Difficulty names

There are two name arrays:

- `set_names`
  Describes difficulty by word length.

- `mc_set_names`
  Gives the Minecraft-style difficulty names:
  `Peaceful`, `Survival`, `Hardcore`

## 8. Main Function Explained

`main()` is the entry point of the whole program.

### What it does

- Seeds randomness with `srand(time(NULL))`
- Loads all puzzle files using `load_all_puzzles()`
- Loads previous scores using `load_scores()`
- Opens the graphics window with `gfx_open()`
- Sets the background color
- If no puzzle files are found, shows an error screen
- Enters a loop showing the main menu

### Main menu choices

- Choice `1`: Start playing
- Choice `2`: Show help
- Choice `3`: Show leaderboard
- Choice `4`: Quit

### When Play is chosen

The code:
- asks for the player name
- asks for difficulty
- checks whether the chosen puzzle set contains words
- starts gameplay using `gui_play_game()`

## 9. Puzzle Loading Functions

### `load_all_puzzles(PuzzleSet psets[])`

Purpose:
- Opens each puzzle text file.
- Reads lines from the file.
- Removes newline characters.
- Stores valid words into memory.

How it works:
- Loops through all 3 puzzle files.
- Uses `fopen()` to open each file.
- Uses `fgets()` to read line by line.
- Uses `strcspn(line, "\r\n")` to remove line breaks.
- Uses `strncpy()` to safely copy each word.
- Counts how many words were loaded.

Important note:
- The function returns how many puzzle files were successfully opened, not how many total words were loaded.

Supervisor-style explanation:
"This function loads the word database from external text files into memory before the game starts, so gameplay does not need to read files every round."

## 10. Graphics and Drawing Functions

The game does not use images. Instead, it draws everything manually using basic shapes.

### Simple drawing

`draw_filled_rect()`
- Draws a filled rectangle by drawing many horizontal lines.

`draw_rect()`
- Draws only the outline of a rectangle.

These are basic helper functions used by many other UI functions.

### Pixel font functions

The code contains a custom 5x7 pixel font array called `mc_font`.

`draw_pixel_char()`
- Draws one character using the bitmap font.

`draw_pixel_string()`
- Draws a full string one character at a time.

`pixel_string_width()`
- Calculates how wide the text will be in pixels.

`draw_pixel_centered()`
- Draws a string centered around a given x-position.

`draw_pixel_shadow()`
- Draws text twice to make a shadow effect.

Why this exists:
- It gives the game a retro Minecraft-style appearance instead of normal system text.

### Pixel art functions

The code contains small arrays for shapes:
- `creeper_face`
- `heart_shape`
- `diamond_shape`

These are drawn with functions:

`draw_creeper()`
- Draws an 8x8 creeper face.

`draw_mc_heart()`
- Draws one heart, either filled or empty.

`draw_mc_hearts()`
- Draws multiple hearts to represent player lives.

`draw_diamond()`
- Draws a Minecraft-style diamond icon.

### Minecraft UI functions

`draw_mc_background()`
- Clears the screen and draws a block-grid background.

`draw_grass_strip()`
- Draws a grassy decorative strip.

`draw_dirt_strip()`
- Draws a dirt decorative strip.

`draw_mc_panel()`
- Draws a gray Minecraft-style panel with highlights and shadows.

`draw_mc_button()`
- Draws a default button.

`draw_mc_button_color()`
- Draws a colored themed button with shading and centered text.

`draw_mc_slot_empty()`
- Draws an empty answer slot.

`draw_mc_slot_letter()`
- Draws a slot containing a letter tile.

`draw_mc_slot_dark()`
- Draws a disabled or already-used scrambled tile.

`draw_xp_bar()`
- Draws the timer bar.
- Green means safe time.
- Yellow means medium urgency.
- Red means low time.

## 11. Utility Functions

### `draw_shadow_text()`
- Draws normal text twice to create a shadow effect.

### `draw_centered_text()`
- Centers text horizontally based on its string length.

### `is_click_in_rect()`
- Returns true if a mouse click happened inside a rectangle.
- This is used for buttons and tile-click detection.

## 12. Scramble and Guess Logic

### `scramble_word(const char *original, char *scrambled)`

Purpose:
- Takes a correct word and creates a scrambled version.

How it works:
- Copies the original word into `scrambled`
- Uses a shuffle process similar to Fisher-Yates
- Repeats up to 20 times if the shuffled result accidentally matches the original
- If it still matches, it swaps the first two letters as a final fallback

Why this matters:
- Prevents the word from staying unchanged after shuffling.

### `pick_word(int used[], int word_count)`

Purpose:
- Picks a random unused word index.

How it works:
- Builds a temporary list of all unused indices
- Randomly selects one of them
- Returns `-1` if no words are left

Why this is good:
- Ensures words are not repeated in a single playthrough

Important limitation:
- It prevents repeated **indices**, not repeated **text values**.
- Since `puzzle2.txt` contains `forest` twice, that word can still appear twice in one game because it exists twice in the source file.

### `check_guess(const char *guess, const char *answer)`

Purpose:
- Compares the player's guess to the real answer.

How it works:
- First checks length
- Then compares each character using `tolower()`
- So uppercase and lowercase are treated as the same

## 13. Screen Functions

The code uses many `gui_...` functions to display screens.

### `gui_main_menu()`

Purpose:
- Shows the first screen of the program.

What it displays:
- Title
- Creeper art
- Four buttons:
  - Play Game
  - How To Play
  - Leaderboard
  - Quit Game

What it returns:
- `1`, `2`, `3`, or `4` depending on which button was clicked.

### `gui_help_screen()`

Purpose:
- Explains the rules to the player.

It includes:
- how to select letters
- keyboard controls
- scoring rules
- lives system
- difficulty information

The function waits until the Back button is clicked.

### `gui_scoreboard()`

Purpose:
- Shows the saved top scores.

What it displays:
- rank
- player name
- score
- difficulty name

If no scores exist:
- It shows a message telling the user to play first.

### `gui_difficulty_select()`

Purpose:
- Lets the player choose a difficulty.

Available options:
- `0` = Peaceful
- `1` = Survival
- `2` = Hardcore
- random choice
- back to previous menu

What makes this screen useful:
- It also shows how many words each set currently has loaded.

### `gui_get_player_name(char *name, int maxlen)`

Purpose:
- Lets the player type their name before starting.

How it works:
- Reads keyboard input using `gfx_wait()`
- Accepts printable characters only
- Supports Backspace
- Finishes when Enter is pressed and the name is not empty

### `gui_play_game(PuzzleSet *ps, int set_index, const char *player_name)`

This is the most important function in the entire program.

Purpose:
- Runs the actual gameplay loop.

Main responsibilities:
- Initializes score and lives
- Tracks which words were already used
- Picks a new word each round
- Scrambles it
- Draws the game screen
- Handles input
- Checks answers
- Updates lives and score
- Ends the game when lives reach zero or all words are done

## 14. Detailed Breakdown of `gui_play_game()`

### Part A: Setup

At the start of the function:
- `lives` is set to `STARTING_LIVES`
- `score` starts at `0`
- `used[]` marks which words were already seen

### Part B: Round selection

Each round:
- a new word index is chosen using `pick_word()`
- if no word is left, the player has completed the set
- the selected word is copied into `word`
- the word is scrambled into `scrambled`

### Part C: Tile position calculation

The code calculates x/y coordinates for:
- the scrambled tiles
- the answer slots

This allows the word to be centered on screen regardless of length.

### Part D: Timer setup

The function stores:
- `round_start = time(NULL)`

Then each loop calculates:
- `elapsed`
- `seconds_left = TIMER_MAX - elapsed`

If `seconds_left <= 0`, the result becomes timeout.

### Part E: Drawing the gameplay screen

Every frame, the function draws:
- background
- top HUD panel
- player name
- hearts
- score
- round number
- difficulty name
- instruction text
- timer bar
- scrambled tiles
- answer slots
- menu button

### Part F: Auto-check behavior

When the player fills every slot:
- `pending_auto_check` becomes `1`
- On the next loop, the code automatically checks the answer

This means:
- the player does not always need to press Enter
- Enter is optional if all slots are already filled

### Part G: Input handling

The game supports both mouse and keyboard.

#### Mouse input

If the player clicks:

- `MENU`
  - the function returns immediately to the main menu

- a scrambled tile
  - the tile is marked selected
  - its letter is added to the answer

- an answer slot
  - that letter is removed
  - the matching scrambled tile is re-enabled

#### Keyboard input

- letter key
  - selects the first unused scrambled tile matching that letter

- Backspace
  - removes the last placed letter

- Enter
  - manually submits the answer if all slots are filled

### Part H: Handling repeated letters

This is one of the more difficult parts of the code.

Problem:
- If the word has the same letter more than once, removing one letter must restore the correct tile, not the wrong duplicate tile.

How the code solves it:
- It counts how many matching letters appear before a given tile
- It compares that count with how many matching letters were already placed in answer slots
- This allows it to map duplicate letters correctly

This logic appears in:
- click-to-remove handling
- Backspace handling

Supervisor-style explanation:
"The duplicate-letter handling prevents ambiguity when a word contains repeated letters, so the correct tile instance is restored instead of restoring a random matching letter."

### Part I: Round result

Possible results:

- `1` = correct
- `-1` = wrong
- `-2` = timeout

Effects:

Correct:
- player gains 1 life
- player gains 10 points
- tiles flash green

Wrong:
- player loses 1 life
- correct answer is shown
- tiles flash red

Timeout:
- player loses 1 life
- correct answer is shown
- tiles flash orange

### Part J: End of game

When the game loop ends:
- the score is added to the leaderboard with `add_score()`
- scores are saved with `save_scores()`
- the game-over screen is shown

If the player chooses Replay:
- the code starts another game using a random difficulty

## 15. Game Over and Error Screens

### `gui_game_over(int score, int *choice)`

Purpose:
- Shows the final score and offers two buttons:
  - Replay
  - Main Menu

Output:
- stores the player's choice in the variable pointed to by `choice`

### `gui_error_screen(const char *msg)`

Purpose:
- Displays an error message if puzzle files are missing or unavailable.

Example use:
- when no puzzle files are found at startup
- when a selected puzzle set has zero words

## 16. Score Management

### `add_score(const char *name, int score, int set_index)`

Purpose:
- Inserts a new score into the correct sorted position.

How it works:
- Builds a new `ScoreRecord`
- Finds where the new score should go
- Shifts lower scores downward
- Keeps only the top 10 entries

Important note:
- Scores are sorted from highest to lowest.

### `save_scores(void)`

Purpose:
- Writes leaderboard data into `src/savedata/scores.dat`

File format:
- first line = number of saved scores
- for each score:
  - player name
  - score
  - difficulty index

### `load_scores(void)`

Purpose:
- Reads leaderboard data back into memory at program startup.

Safety features:
- If the file does not exist, it simply returns.
- It limits loaded scores to `MAX_SCORES`.

## 17. About `gfx.c` and `gfx.h`

This library handles the low-level graphics and events.

### Important functions from the library

`gfx_open(width, height, title)`
- Opens the application window.

`gfx_point(x, y)`
- Draws one point.

`gfx_line(x1, y1, x2, y2)`
- Draws a line.

`gfx_color(r, g, b)`
- Sets the current drawing color.

`gfx_clear()`
- Clears the window to the chosen background color.

`gfx_clear_color(r, g, b)`
- Sets the background color used by `gfx_clear()`.

`gfx_wait()`
- Waits until the user presses a key or mouse button.

`gfx_xpos()` and `gfx_ypos()`
- Return the coordinates of the last event.

`gfx_event_waiting()`
- Checks whether any input event is waiting.

`gfx_flush()`
- Sends all drawing commands to the window.

`gfx_text(x, y, text)`
- Draws normal text using X11 text drawing.

### Important design note

`gfx.c` was provided externally and is not the team's original logic file. The project mainly builds game features on top of that library.

## 18. How the UI Style Is Built

The game uses a Minecraft-inspired style through:

- blocky pixel font
- creeper face art
- heart icons for lives
- diamond icon for score decoration
- gray inventory-style panels
- 3D-style buttons and slots
- grass and dirt strips
- timer shown as an XP bar

So, the "theme" is not from imported images. It is drawn manually using rectangles, lines, and pixel patterns.

## 19. Strengths of the Project

- Clear separation between low-level graphics (`gfx.c`) and game logic (`game.c`)
- Puzzle words stored externally in text files, so content is easy to change
- Leaderboard persistence through save file
- Supports mouse and keyboard controls
- Uses custom drawing instead of depending on image assets
- Handles repeated letters more carefully than a naive implementation
- Uses reusable helper functions for UI drawing

## 20. Limitations and Technical Notes

These are useful to know if the supervisor asks about weaknesses or future improvement.

### 1. Full project logic is concentrated in one file

Almost all game logic is in `game.c`.

This makes the project simpler for a small assignment, but for a larger project it would be better to split into modules such as:
- gameplay logic
- UI rendering
- score handling
- puzzle loading

### 2. Replay uses recursive function calls

If the player presses Replay on the game-over screen, `gui_play_game()` calls itself again.

This works for normal use, but from a software design perspective, a loop would be cleaner than recursion for replaying many times.

### 3. Exiting to menu during a round does not save a partial score

If the player clicks the `MENU` button while a round is running, `gui_play_game()` returns immediately.

That means:
- the current score is not added to the leaderboard
- the current run is abandoned

This is a design choice, but it is worth knowing.

### 4. No animation framework

The program uses manual redraws and `usleep()` delays instead of a more advanced game engine or timing system.

### 5. Duplicate words inside puzzle files are not filtered

If the same word exists twice in a puzzle file, both entries may still appear in one session.

### 6. Fixed-size arrays

The code uses static array limits like `MAX_WORDS`, `MAX_WORD_LEN`, and `MAX_LETTERS`.

This is simple and safe for a small project, but less flexible than dynamic allocation.

