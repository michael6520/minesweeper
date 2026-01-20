# Minesweeper

# Overview

This software solves the google minesweeper by using the C++ Win32 API to look at the screen, find certain pixel RGB values, convert the screen capture into useable data, perform some solving logic, and then use Win32 to click on the screen. It repeats these steps over and over again until the board is solved. Since it looks for specific RGB values, the google minesweeper is the only one it can solve.

I made this because I thought it would be a fun introduction to C++. I like logic and I'd never done anything with looking at the screen before, so I thought I'd give it a try.

# Development Environment

I wrote it in C++ using Visual Studio. I used the Win32 API to handle getting the board information and clicking the cells.

# Future Work

I need to optimize it. If it can't figure it out using simple logic, it instead uses constraint satisfaction by splitting the border into independent sections and then calculating every possible permutation of mines in that section. Because of that, it runs in O(2^n) time, which means if there are more than 30 cells in every section it would start taking far too long.
