#!/bin/bash

x86_64-w64-mingw32-g++ src/main.cpp\
 src/game.cpp\
 src/player.cpp\
 src/collisionSystem.cpp\
 src/enemySystem.cpp\
 src/gameTextSystem.cpp\
 src/particleSystem.cpp\
 src/projectileSystem.cpp\
 src/statSystem.cpp\
 src/timerSystem.cpp\
 src/xpSystem.cpp\
 src/modifierSystem.cpp\
 src/messageSystem.cpp\
 src/assetManager.cpp\
 src/projectiles.cpp\
 src/enemy.cpp\
 src/powerupMenu.cpp\
 src/xp.cpp\
 src/gameOverMenu.cpp\
 src/pauseMenu.cpp\
 src/gameText.cpp\
 src/gameDrawSystem.cpp\
 src/gameHelpers.cpp\
 src/gameInputSystem.cpp\
 src/mainMenu.cpp\
 src/globalDataWrapper.cpp\
 src/settingsManager.cpp\
 src/particle.cpp\
 -o /mnt/tmpfs/out.exe\
 -flto=auto\
 -O3\
 -std=c++20\
 -Iinclude/\
 -Isrc/headers/\
 -Llib/\
 -lraylib\
 -lopengl32\
 -lgdi32\
 -lwinmm\
 -static\
 -march=haswell
 #-Wall
#-fno-exceptions
#-Wextra\
#-Wpedantic

#-mwindows\

