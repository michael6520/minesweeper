#pragma once

#include <windows.h>
#include "BoardImage.h"
#include "Solver.h"

// Handles all interaction with the Minesweeper board on screen, including:
// - Finding the board's position and size on screen.
// - Capturing the image of the board.
// - Simulating mouse input to reveal or flag cells.
class CaptureBoard {
public:
	// Constructs a CaptureBoard; initializes the 'board dimension' values
	// to the dimensions of the user's monitors, so the first time
	// captureScreen() is called it captures the entire screen.
	CaptureBoard();

	// Returns the board image.
	const BoardImage& returnImg() const;

	// Captures a rectangle of the screen into a vector of pixel bytes.
	// Uses dimensions from BoardImage img, which are initialized to be 
	// the screen dimensions.
	void captureScreen();

	// Finds the minesweeper board in a pixel vector containing the entire screen.
	// Changes the dimensions of BoardImage img so that whenever captureScreen()
	// is called, it captures just the Minesweeper board.
	void findBoard();

	// Clicks the center of the Minesweeper board to start the game.
	void startGame() const;

	// Applies a list of queued actions (clicks or flags) to the board.
	void applyActions(const std::vector<GridAction>& actions) const;

private:
	BoardImage img;		// Data about the board capture, including vector of raw pixel bytes, width, height, and cell width.
	int left;			// X coordinate of the top-leftmost cell of the Minesweeper board.
	int top;			// Y coordinate of the top-leftmost cell of the Minesweeper board.
	int	screenWidth;	// Width of the capture in pixels.
	
	// Sets the cursor position to (x, y)
	void moveMouse(int x, int y) const;

	// Mouse click inputs at the current cursor position
	void leftClick() const;
	void rightClick() const;

	// Moves the cursor and left clicks or right clicks depending on action param
	void clickCell(int screenX, int screenY, ActionType action) const;
};