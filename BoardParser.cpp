#include "BoardParser.h"

BoardParser::BoardParser() {}

void BoardParser::update(const BoardImage& image) {
	img = image;

	boardWidth = img.width / img.cellWidth;
	boardHeight = img.height / img.cellWidth;
}

const std::vector<std::vector<Cell>>& BoardParser::returnBoard() const { return parsedBoard; }

State BoardParser::findState(const Pixel& pixel) {
	static const std::vector<std::pair<Pixel, State>> lookup = {
		{{ 222, 189, 156 }, ZERO},
		{{ 25, 118, 210 }, ONE},
		{{ 56, 142, 60 }, TWO},
		{{ 211, 47, 47 }, THREE},
		{{ 123, 31, 162 }, FOUR},
		{{ 255, 143, 0 }, FIVE},
		{{ 0, 151, 167 }, SIX},
		{{ 66, 66, 66 }, SEVEN},
		{{ 230, 51, 7 }, FLAG},
		{{ 166, 212, 77 }, UNKNOWN},
	};

	for (const auto& [p, state] : lookup) {
		if (img.matchColor(p, pixel, 10))
			return state;
	}
	return NOTFOUND;
}

Cell BoardParser::makeCell(size_t start, size_t x, size_t y) {
	bool zero = false;
	bool unknown = false;

	for (size_t id = 0; id < img.cellWidth; ++id) {
		size_t scaledid = (id + start) * 4;

		Pixel pixel = img.getPixel(scaledid);
		State state = findState(pixel);

		if (state == ZERO)
			zero = true;
		else if (state == UNKNOWN)
			unknown = true;
		else if (!(state == NOTFOUND))
			return { state, 0, 0, x, y, {} };
	}
	if (zero)
		return { ZERO, 0, 0, x, y, {} };
	else if (unknown)
		return { UNKNOWN, 0, 0, x, y, {} };
	else
		return { NOTFOUND, 0, 0, x, y, {} };
}

void BoardParser::parseCells() {	
	static bool firstTime = true;
	if (firstTime) {
		firstTime = false;
		parsedBoard.resize(boardHeight);
		for (auto& row : parsedBoard)
			row.resize(boardWidth);
	}

	for (size_t y = 0; y < boardHeight; ++y) {
		for (size_t x = 0; x < boardWidth; ++x) {
			size_t id = img.cellWidth * (y * img.width + img.width / 2 + x);
			Cell cell = makeCell(id, x, y);
			if (cell.state == NOTFOUND) {
				gameOver = true;
				return;
			}
			parsedBoard[y][x] = cell;
		}
	}
}

void BoardParser::initParsedBoard() {
	for (auto& row : parsedBoard) {
		for (auto& cell : row) {
			if (cell.state == FLAG || cell.state == UNKNOWN || cell.state == ZERO)
				continue;

			for (int x = -1; x < 2; ++x) {
				for (int y = -1; y < 2; ++y) {
					size_t nx = cell.x + x;
					size_t ny = cell.y + y;
					if (nx < 0 || nx >= boardWidth ||
						ny < 0 || ny >= boardHeight)
						continue;

					const auto& neighbor = parsedBoard[ny][nx];

					if (neighbor.state == UNKNOWN) {
						cell.adjacentUnknowns += 1;
						cell.neighbors.push_back({ nx, ny });
						parsedBoard[ny][nx].frontier = true;
					}
					if (neighbor.state == FLAG) 
						cell.adjacentFlags += 1;
				}
			}
		}
	}
}