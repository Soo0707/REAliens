#!/bin/bash

x86_64-w64-mingw32-g++ src/main.cpp src/game.cpp src/player.cpp src/gameObjects.cpp src/assetManager.cpp\
 -o /mnt/tmpfs/out.exe\
 -Iinclude/\
 -Isrc/headers/\
 -Llib/\
 -lraylib\
 -lopengl32\
 -lgdi32\
 -lwinmm\
 -static

#-mwindows\
