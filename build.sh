#!/bin/sh

[ -f nuget ] && rm nuget; rm obj/*

gcc -g -Iinclude -c src/curses_helpers.c -o obj/curses_helpers.o -lm -lcurses
gcc -g -Iinclude -c src/ncsv.c -o obj/ncsv.o -lm -lcurses
gcc -g -Iinclude -c src/settings.c -o obj/settings.o -lm -lcurses
gcc -g -Iinclude -c src/table.c -o obj/table.o -lm -lcurses
gcc -g -Iinclude -c src/common.c -o obj/common.o -lm -lcurses
gcc -g -Iinclude -c src/main.c -o obj/main.o -lm -lcurses

gcc -g obj/common.o obj/table.o obj/curses_helpers.o obj/ncsv.o obj/settings.o obj/main.o -o nuget -lm -lcurses
