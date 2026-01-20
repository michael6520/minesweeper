#pragma once

#include <algorithm>
#include <cstdint>
#include <intrin.h>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "BoardParser.h"

// Left click or right click. Left click reveals a cell, right click flags it.
enum ActionType { LCLICK, RCLICK };

// Represents a single click, left or right, at a single cell's coordinates.
struct GridAction {
	ActionType type;
	size_t x, y;
};

// Represents a single Minesweeper constraint: a set of frontier cells 
// and the exact number of mines among them.
struct Constraint {
	std::vector<int> vars;
	int mines;
	int mineCounter = 0;
};

// Applies basic deterministic Minesweeper logic to find guaranteed moves,
// and saves those moves in a list. Does not guess.
class Solver {
public:
	// Constructor; initializes 'progress' to true.
	Solver();

	// Updates the internal parsed board with a new one.
	void update(std::vector<std::vector<Cell>> pBoard);

	// Returns a list of grid actions to be applied.
	const std::vector<GridAction>& returnActions() const;

	// Finds every guaranteed mine and safe cell in the current parsed board,
	// then updates 'gridActions' accordingly
	void solveStep();

	bool progress;	// Represents whether or not the solver made any progress in a turn.

	void CSPTurn();

private:	
	std::vector<std::vector<Cell>> parsedBoard;	// Parsed grid of cell data
	std::vector<GridAction> gridActions;		// List of grid actions to be applied

	// Helper function to locate every guaranteed mine in the current parsed board,
	// with no duplicates.
	std::unordered_set<Coord, CoordHash> findMines();

	// Helper function to locate every guaranteed safe cell in the current parsed board,
	// with no duplicates.
	std::unordered_set<Coord, CoordHash> findSafeCells();

	std::vector<Coord> frontierCells;		// List of coordinates of every unknown cell adjacent to a number cell.
	std::vector<Constraint> constraints;	// Stores all constraints extracted from the current board.
	std::vector<int> sects;					// Stores sect IDs for the frontier. sect[i] is the sect ID for frontierCell[i].
	std::vector<int> mines;
	std::vector<int> safeCells;

	// Finds all unknown cells that are adjacent to a number
	// cell and stores them in the 'frontierCells' vector.
	void collectFrontier();

	// Finds all constraints in the current board and stores
	// them in the 'constraints' vector.
	void collectConstraints();

	// DFS to turn the frontier into independent sections saved as sect IDs in 'sect'
	void findSections();

	bool checkAssignment(const std::vector<int>& assignment, const std::vector<int>& vars, std::vector<Constraint> cons);

	void solveSections();

	void CSPGridActions();
};