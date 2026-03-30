#!/bin/bash

# Copyright (C) 2026 Soo0707
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

g++ src/main.cpp\
 src/game.cpp\
 src/player.cpp\
 src/collisionSystem.cpp\
 src/enemySystem.cpp\
 src/particleSystem.cpp\
 src/projectileSystem.cpp\
 src/statSystem.cpp\
 src/timerSystem.cpp\
 src/itemSystem.cpp\
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
 src/cameraSystem.cpp\
 -o /mnt/tmpfs/out\
 -std=c++20\
 -Iinclude/\
 -Isrc/headers/\
 -Llib/linux/\
 -lraylib\
 -lGL\
 -lX11\
 -O0\
 -g\
 -Wno-changes-meaning\
 -fno-omit-frame-pointer\
 -fsanitize=address\
 -fsanitize=undefined


