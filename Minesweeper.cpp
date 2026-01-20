#include <iostream>
#include <windows.h>
#include "BoardParser.h"
#include "CaptureBoard.h"
#include "Solver.h"

// Moves the cursor to the top left of the main monitor
// and waits for particle effects to dissipate or for
// the win screen to pop up. Ensures the next capture is clean.
static void wait(int time) {
	SetCursorPos(0, 0);
	Sleep(time);
}

// Captures the entire screen, finds the Minesweeper board,
// then clicks the center to initialize the game.
// After this, 'capture' knows the board's location and size.
static void initializeGame(CaptureBoard& capture) {
	capture.captureScreen();
	capture.findBoard();
	capture.startGame();
}

// Performs a single game turn:
// 1. Captures the current board image.
// 2. Parses the image into useable cell states.
// 3. Finds guaranteed mines and safe cells using deterministic logic.
// 4. Clicks the board according to the data found in step 3.
static void processTurn(CaptureBoard& capture, BoardParser& parser, Solver& solver) {
	capture.captureScreen();

	parser.update(capture.returnImg());
	parser.parseCells();
	
	if (parser.gameOver) {
		solver.progress = false;
		return;
	}

	parser.initParsedBoard();

	solver.update(parser.returnBoard());
	solver.solveStep();

	capture.applyActions(solver.returnActions());
}

// Performs a single game turn:
// 1. Captures the current board image.
// 2. Parses the image into useable cell states.
// 3. Finds guaranteed mines and safe cells using constraint satisfaction.
// 4. Clicks the board according to the data found in step 3.
static void processCSPTurn(CaptureBoard& capture, BoardParser& parser, Solver& solver) {
	capture.captureScreen();

	parser.update(capture.returnImg());
	parser.parseCells();

	if (parser.gameOver) {
		solver.progress = false;
		return;
	}

	parser.initParsedBoard();

	solver.update(parser.returnBoard());
	solver.CSPTurn();

	capture.applyActions(solver.returnActions());
}

int main()
{
	CaptureBoard capture;
	BoardParser parser;
	Solver solver;

	initializeGame(capture);
	wait(750);

	while (solver.progress) {
		while (solver.progress) {
			processTurn(capture, parser, solver);
			wait(100);
		}
		processCSPTurn(capture, parser, solver);
		wait(100);
	}

	return 0;
}