# projet-algo-s2
# 🚀 Orbital Breaker: Claude's Last Mission

> A shoot'em up game developed in C with Allegro 4.4 — ECE Paris, Semester 2 Algorithmique & Programmation project.

---

## 📖 Overview

**Orbital Breaker: Claude's Last Mission** is a 2D space shooter where the player pilots a spaceship through three asteroid-filled levels before facing a final boss. The game features voiced dialogue sequences, cinematic transitions between levels, a power-up system, and a complete save/load system.

---

## 🎮 Gameplay

- Destroy all asteroids to complete each level and advance to the next
- Survive 3 levels then defeat the final boss
- Collect power-ups dropped by destroyed asteroids:
  - **Triple Laser** (blue) — fires three missiles simultaneously
  - **Invincibility** (yellow) — temporary shield
- You have 3 hearts — lose them all and it's game over

### Controls

| Key | Action |
|-----|--------|
| ← / → | Move ship left / right |
| Space | Fire laser |
| P | Pause |
| S | Save game |

---

## 🗂️ Project Structure

```
projet-algo-s2/
├── src/
│   ├── main.c          # Entry point & main game loop
│   ├── game.c          # Game logic, collisions, level management
│   ├── player.c        # Player movement, lasers, stats
│   ├── asteroids.c     # Asteroid spawning, physics, splitting
│   ├── boss.c          # Boss AI, lightning attack sequences
│   ├── display.c       # Rendering, HUD, cinematics, transitions
│   ├── ihm.c           # Menus, screens (main, pause, game over, victory)
│   ├── audio.c         # Sound effects, music, voiced dialogue
│   ├── assets.c        # Asset loading/freeing
│   └── save.c          # Save/load system (plaintext)
├── headers/            # Corresponding .h files
├── assets/
│   ├── images/         # BMP sprites (ships, asteroids, boss, UI)
│   └── sfx/            # WAV audio files (music, SFX, speech)
├── data/
│   ├── levels.txt      # Level configuration
│   └── save.txt        # Save file (auto-generated)
└── CMakeLists.txt
```

---

## ⚙️ Levels Configuration

Levels are defined in `data/levels.txt`:

```
4              ← total number of levels
0 0 200 5 40   ← level 0: normal, gravity=200, 5 asteroids, max size 40
1 0 300 8 60
2 0 500 12 50
3 1 0 0 0      ← level 3: boss level
```

Format per line: `index  is_boss  gravity  max_asteroids  max_size`

---

## 🔊 Audio

- Background music loops throughout the game (`bo.wav`)
- Sound effects for lasers, hits, explosions, and boss lightning
- 8 voiced dialogue clips (`speech1–8.wav`) used across intro, game over, and victory cinematics

---

## 🛠️ Build Instructions

### Prerequisites

- [MinGW/GCC](https://www.mingw-w64.org/) (Windows)
- [Allegro 4.4](https://liballeg.org/) (`-lalleg44`)
- [CMake](https://cmake.org/) ≥ 3.10
- CLion (recommended IDE) or any CMake-compatible environment

### Build

```bash
git clone https://github.com/rurv/projet-algo-s2.git
cd projet-algo-s2
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

The executable will be generated in the `build/` directory. Assets are automatically copied there by CMake.

> **Note:** The game looks for assets relative to the working directory. CLion may run from the build subdirectory — the code handles both `assets/` and `../assets/` paths automatically.

---

## 💾 Save System

The save file (`data/save.txt`) is human-readable plaintext:

```
ORBS          ← magic key to validate the file
2             ← level index
3             ← remaining lives
1             ← ship skin ID
YourName      ← player pseudo
```

Use **S** in-game or the **Save** button in the pause menu to save. The save is automatically deleted upon completing or restarting the game.

---

## 📄 License

Academic project — ECE Paris. Not for commercial use.
