#pragma once

#include <cstdlib>
#include <vector>
#include <windows.h>

// Represents the color of a pixel in RGB
struct Pixel {
	int r, g, b;
};

// Encapsulates the pixel colors and dimensions of a captured Minesweeper board.
// Stores raw pixel bytes, board width/height, and the width of a single cell in pixels.
// Provides helper functions to get a pixel from the raw pixel bytes and compare two pixel colors.
class BoardImage {
public:
	std::vector<BYTE> pixels;	// Raw pixel bytes in BGRA format.
	int width;					// Width of the captured board in pixels.
	int	height;					// Height of the captured board in pixels.
	int	cellWidth;				// Width of a single Minesweeper cell in pixels.

	// Returns a Pixel struct in RGB of the pixel at the given index in the pixels vector.
	Pixel getPixel(int id) const;

	// Compares a pixel to a target color. Returns true if each
	// pixel value is within tolerance of the target's cooresponding value.
	bool matchColor(const Pixel& target, const Pixel& pixel, int tolerance) const;
};