#!/bin/bash

x86_64-w64-mingw32-g++ $PWD/src/main.cpp\
 -o /mnt/tmpfs/out.exe\
 -Iinclude/\
 -Llib/\
 -lraylib\
 -lopengl32\
 -lgdi32\
 -lwinmm\
 -mwindows\

