# SFML Game Simulation

This project is a **2D game built using the SFML (Simple and Fast Multimedia Library)**, showcasing fundamentals of **game design**, **C++ programming**, and **graphics programming**.

The game demonstrates:

-   Object-oriented design principles

-   Real-time event handling and rendering

-   Game loop structure

-   Collision detection and resource management

## Concept

**Game Title:** Protect the Base! 
**Genre:** Real Time Strategy, Tower Defense  
**Objective:** Protect the base as long as possible, gathering resources and collecting coins to gain the advantage over your enemies.

**Core Features:**

-   Smooth 2D graphics powered by SFML
    
-   Mouse input
    
-   Dynamic objects and entities
    
-   Basic UI (health, menus, resources)
    
-   Sound effects and background music


## How It Works

The project is structured around a **main game loop** that updates and renders all objects each frame.  It uses the following modules:


|         Modules       |Description |
|----------------|-------------------------------|
|`main.cpp` |Entry point that initializes game and starts main loop 
|`Game.h/.cpp`       |Manages game states, updates, rendering, and event handling.     
|`Player.h/.cpp`           | Defines player logic, input control, and animations.
|`Enemy.h/.cpp`     |Handles enemy movement and interactions.
|`ResourceManager.h/.cpp`|Manages textures, fonts, and sounds.
|`UI.h/.cpp`|Handles user interface elements like score or HUD

## Installation & Setup

### Prerequisites

-   **C++17 or newer**
    
-   **SFML 2.6+** (or compatible)
    
-   **CMake** _(optional, if building with CMake)_
    

### Build Instructions (Example for Windows/Mac/Linux)

#### Using g++ (manual build)
 ```
 g++ -std=c++17 -I path/to/SFML/include -o game main.cpp Game.cpp Player.cpp Enemy.cpp \
    -L path/to/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

#### Using CMake
```
mkdir build
cd build
cmake ..
make
./game
```

## How to Play

|         Action       |Key |
|----------------|-------------------------------|
|LMB | Select soldier/building, harvest wood
|RMB       |Move soldier to selected tile     
|Esc      | Exit game

## References

- SFML Official Documentation
- C++ Reference
