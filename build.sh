#!/bin/bash

# Copyright (C) 2026 Soo0707
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

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
 src/gameInputSystem.cpp\
 src/mainMenu.cpp\
 src/stringCache.cpp\
 src/settingsManager.cpp\
 src/stateManager.cpp\
 src/systemsResetState.cpp\
 -o /mnt/tmpfs/out.exe\
 -std=c++20\
 -Iinclude/\
 -Isrc/headers/\
 -Llib/\
 -lraylib\
 -lopengl32\
 -lgdi32\
 -lwinmm\
 -static\
 -march=sandybridge\
 -mtune=haswell\
 -flto=auto\
 -O3

#-g\
#-fno-omit-frame-pointer



