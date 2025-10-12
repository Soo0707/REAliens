#!/bin/bash

x86_64-w64-mingw32-g++ src/main.cpp\
 src/game.cpp\
 src/player.cpp\
 src/assetManager.cpp\
 src/projectiles.cpp\
 src/collisions.cpp\
 src/enemy.cpp\
 src/powerupMenu.cpp\
 src/xp.cpp\
 src/gameOverMenu.cpp\
 src/pauseMenu.cpp\
 -o /mnt/tmpfs/out.exe\
 -O3\
 -std=c++20\
 -Iinclude/\
 -Isrc/headers/\
 -Llib/\
 -lraylib\
 -lopengl32\
 -lgdi32\
 -lwinmm\
 -static

#-mwindows\

