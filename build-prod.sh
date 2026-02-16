#!/bin/bash

x86_64-w64-mingw32-g++ src/main.cpp\
 src/game.cpp\
 src/player.cpp\
 src/collisionSystem.cpp\
 src/enemySystem.cpp\
 src/particleSystem.cpp\
 src/projectileSystem.cpp\
 src/statSystem.cpp\
 src/timerSystem.cpp\
 src/xpSystem.cpp\
 src/modifierSystem.cpp\
 src/messageSystem.cpp\
 src/assetManager.cpp\
 src/powerupMenu.cpp\
 src/gameOverMenu.cpp\
 src/pauseMenu.cpp\
 src/gameDrawSystem.cpp\
 src/gameHelpers.cpp\
 src/gameInputSystem.cpp\
 src/mainMenu.cpp\
 src/globalDataWrapper.cpp\
 src/settingsManager.cpp\
 -o /mnt/tmpfs/REAliens.exe\
 -Iinclude/\
 -Isrc/headers/\
 -Llib/\
 -lraylib\
 -lopengl32\
 -lgdi32\
 -lwinmm\
 -static\
 -flto=auto\
 -O3\
 -std=c++20\
 -march=sandybridge\
 -mtune=haswell\
 -mwindows



