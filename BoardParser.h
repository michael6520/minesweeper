#pragma once

#include <vector>
#include "BoardImage.h"

// Every different type of cell on the Minesweeper grid.
enum State {
	ZERO,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	FLAG,
	UNKNOWN,
	NOTFOUND
};

// Coordinates of a cell
struct Coord {
	size_t x, y;

	// For use in unordered maps.
	bool operator==(const Coord& other) const {
		return x == other.x && y == other.y;
	}
};

// For coord use in unordered maps.
struct CoordHash {
	std::size_t operator()(const Coord& c) const {
		return std::hash<int>()(c.x) ^ (std::hash<int>()(c.y) << 1);
	}
};

// Represents a single Minesweeper cell in its parsed state.
// Stores the state, position, and information about adjacent cells
// used by the solver to make decisions.
struct Cell {
	State state;
	int adjacentFlags;
	int adjacentUnknowns;
	size_t x, y;
	std::vector<Coord> neighbors;	// Only contains adjacent unknowns cells; no flags or number cells
	bool frontier = false;
};

// Parses a board image into a grid of cells useable by the solver.
class BoardParser {
public:
	// Constructor. Doesn't initialize anything.
	BoardParser();

	// Updates the board image with a newly captured one.
	void update(const BoardImage& img);

	// Returns the parsed grid of cells.
	const std::vector<std::vector<Cell>>& returnBoard() const;

	// Reads the board image into a grid of cells with states and coordinates,
	// leaving adjacency and neighbor data as zero or empty.
	void parseCells();

	// Fills in adjacency and neighbor data for each cell in a newly parsed grid.
	void initParsedBoard();

	bool gameOver = false;

private:
	BoardImage img;								// Current captured board image.
	std::vector<std::vector<Cell>> parsedBoard;	// Parsed grid of cell data.
	int boardWidth;								// Number of cells horizontally.
	int boardHeight;							// Number of cells vertically.

	// Helper function to convert a pixel color into a state;
	// each state has a unique color.
	State findState(const Pixel& pixel);

	// Creates a cell object by iterating through the pixel data for
	// a single board cell in the board image.
	Cell makeCell(size_t start, size_t x, size_t y);
};