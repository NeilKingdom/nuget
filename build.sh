#!/bin/sh

[ -f nuget ] && rm nuget; rm obj/*

gcc -g -Iinclude -c src/table.c -o obj/table.o -lm -lcurses
gcc -g -Iinclude -c src/curses_helpers.c -o obj/curses_helpers.o -lm -lcurses
gcc -g -Iinclude -c src/csv_parser.c -o obj/csv_parser.o -lm -lcurses
gcc -g -Iinclude -c src/json_parser.c -o obj/json_parser.o -lm -lcurses
gcc -g -Iinclude -c src/main.c -o obj/main.o -lm -lcurses

gcc -g obj/table.o obj/curses_helpers.o obj/csv_parser.o obj/json_parser.o obj/main.o -o nuget -lm -lcurses
