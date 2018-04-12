# Game Of Life

## Introduction
This program is written in C and uses the Simple DirectMedia Layer library source code available
from [libsdl](https://www.libsdl.org/) and therefore distributed under the terms of the [zlib license](http://www.zlib.net/zlib_license.html).

This program displays the Game Of Life based on 4 simple fundamental rules described by Conway
1. Any live cell with fewer than two live neighbors dies, as if caused by underpopulation.
2. Any live cell with more than three live neighbors dies, as if by overcrowding.
3. Any live cell with two or three live neighbors lives on to the next generation.
4. Any dead cell with exactly three live neighbors becomes a live cell.

where each living cell is represented by a red pixel.

The main application is found in the directory GameOfLife/build/main and the source code in GameOfLife/build/main.c.
For displaying the intro and start sequence external source code snippets were included from https://gist.github.com/niw/5963798 written
by Yoshimasa Niwa based on Guillame Cottenceau's work and may be distributed under the terms of the [X11 license](https://mit-license.org/).

## How it works
After downloading this repository go to the folder GameOfLife/build/ and double click main (works for ubuntu 16.04 but no idea if it works on other operating systems as I did not test for it). When launching the main executable icon you will see the title of the program in the newly
launched window and after a few seconds a second description will be displayed. At this stage you can simply press a key on your keyboard and the show starts. After 300 iterations it automatically pauses, which resumes its activity after clicking a key on your keyboard (after every 300 iterations). The application may be quit by clicking on the  quit icon on the window or from the terminal by CTRL + C, as you wish.

## Reason for this application
This application was written as a project assignment in order to fulfill the requirements of the C Programing Course at the Free University in Berlin in April 2018. Moreover to get more confident with C.
