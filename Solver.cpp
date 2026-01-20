#include <iostream>
#include "Solver.h"

Solver::Solver() {
	progress = true;
}

void Solver::update(std::vector<std::vector<Cell>> pBoard) { parsedBoard = pBoard; }

const std::vector<GridAction>& Solver::returnActions() const { return gridActions; }

std::unordered_set<Coord, CoordHash> Solver::findMines() {
	std::unordered_set<Coord, CoordHash> toClick;
	for (const auto& row : parsedBoard) {
		for (const auto& cell : row) {
			if (cell.state == FLAG || cell.state == UNKNOWN)
				continue;
			if (cell.adjacentUnknowns == 0)
				continue;

			if (cell.adjacentUnknowns + cell.adjacentFlags == cell.state) {
				progress = true;
				for (const auto& neighbor : cell.neighbors) {
					toClick.insert({ neighbor.x, neighbor.y });
				}
			}
		}
	}
	return toClick;
}

std::unordered_set<Coord, CoordHash> Solver::findSafeCells() {
	std::unordered_set<Coord, CoordHash> toClick;
	for (const auto& row : parsedBoard) {
		for (const auto& cell : row) {
			if (cell.state == FLAG || cell.state == UNKNOWN || cell.state == ZERO)
				continue;
			if (cell.adjacentUnknowns == 0)
				continue;

			if (cell.adjacentFlags == cell.state) {
				progress = true;
				for (const auto& neighbor : cell.neighbors) {
					toClick.insert({ neighbor.x, neighbor.y });
				}
			}
		}
	}
	return toClick;
}

void Solver::solveStep() {
	progress = false;
	gridActions.clear();
	
	const auto& mines = findMines();
	for (const auto& mine : mines)
		gridActions.push_back({ RCLICK, mine.x, mine.y });
	const auto& safeCells = findSafeCells();
	for (const auto& cell : safeCells)
		gridActions.push_back({ LCLICK, cell.x, cell.y });
}

void Solver::collectFrontier() {
	frontierCells.clear();
	for (const auto& row : parsedBoard) {
		for (const auto& cell : row) {
			if (cell.frontier)
				frontierCells.push_back({ cell.x, cell.y });
		}
	}
}

void Solver::collectConstraints() {
	std::unordered_map<Coord, int, CoordHash> coordToIndex;
	for (int i = 0; i < frontierCells.size(); ++i)
		coordToIndex[frontierCells[i]] = i;

	constraints.clear();

	for (const auto& row : parsedBoard) {
		for (const auto& cell : row) {
			if (cell.state == ZERO || cell.state == FLAG || cell.state == UNKNOWN)
				continue;

			Constraint c;
			c.mines = cell.state - cell.adjacentFlags;

			for (const auto& nbr : cell.neighbors) {
				auto it = coordToIndex.find(nbr);
				if (it != coordToIndex.end()) {
					c.vars.push_back(it->second);
				}
			}

			if (!c.vars.empty())
				constraints.push_back(c);
		}
	}
}

void Solver::findSections() {
	sects.clear();
	std::unordered_map<int, std::vector<int>> adj;
	for (const auto& c : constraints) {
		for (int vi : c.vars)
			for (int vj : c.vars)
				if (vi != vj)
					adj[vi].push_back(vj);
	}

	sects.resize(frontierCells.size(), -1);
	int sectId = 0;

	for (int i = 0; i < frontierCells.size(); ++i) {
		if (sects[i] != -1)
			continue;

		std::stack<int> stack;
		stack.push(i);
		sects[i] = sectId;

		while (!stack.empty()) {
			int u = stack.top();
			stack.pop();
			
			for (int v : adj[u]) {
				if (sects[v] == -1) {
					sects[v] = sectId;
					stack.push(v);
				}
			}
		}

		sectId++;
	}
}

void Solver::solveSections() {
	mines.clear();
	safeCells.clear();
	progress = false;

	int maxSectId = *std::max_element(sects.begin(), sects.end());
	int numSects = maxSectId + 1;

	for (int sid = 0; sid < numSects; ++sid) {
		std::vector<int> vars;
		std::vector<Constraint> cons;

		for (int i = 0; i < frontierCells.size(); ++i) {
			if (sects[i] == sid)
				vars.push_back(i);
		}

		int N = vars.size();
		if (N > 30) {
			std::cout << "too many variables:" << N << '\n';
			continue;
		}

		std::unordered_set<int> varsSet(vars.begin(), vars.end());

		for (const auto& c : constraints) {
			if (std::all_of(c.vars.begin(), c.vars.end(),
				[&](int v) { return varsSet.count(v); })) {
				cons.push_back(c);
			}
		}

		int C = cons.size();

		std::unordered_map<int, int> globalToLocal;
		for (int i = 0; i < N; ++i)
			globalToLocal[vars[i]] = i;

		std::vector<std::vector<int>> varToCons(N);
		for (int ci = 0; ci < C; ++ci) {
			for (int v : cons[ci].vars) {
				int local = globalToLocal[v];
				varToCons[local].push_back(ci);
			}
		}

		std::vector<int> assignment(N, 0);
		std::vector<int> constraintCount(C, 0);
		std::vector<int> mineCount(N, 0);
		int numValidAssignments = 0;

		size_t totalMasks = 1 << N;
		int prevGray = 0;

		for (size_t mask = 0; mask < totalMasks; ++mask) {
			int gray = mask ^ (mask >> 1);

			if (mask > 0) {
				int diff = gray ^ prevGray;
				unsigned long index;
				_BitScanForward(&index, diff);
				int flipped = static_cast<int>(index);

				int newValue = (gray >> flipped) & 1;
				int delta = newValue ? 1 : -1;

				assignment[flipped] = newValue;

				for (int ci : varToCons[flipped])
					constraintCount[ci] += delta;
			}

			prevGray = gray;

			bool valid = true;
			for (int ci = 0; ci < C; ++ci) {
				if (constraintCount[ci] != cons[ci].mines) {
					valid = false;
					break;
				}
			}
			if (!valid) continue;

			numValidAssignments++;
			for (int i = 0; i < N; ++i)
				mineCount[i] += assignment[i];
		}

		for (int i = 0; i < N; ++i) {
			if (numValidAssignments == 0) {
				std::cout << "no valid assignments\n";
				break;
			}
			if (mineCount[i] == numValidAssignments) {
				progress = true;
				mines.push_back(vars[i]);
			}
			else if (mineCount[i] == 0) {
				progress = true;
				safeCells.push_back(vars[i]);
			}
		}

		std::cout << "Section " << sid << " has " << N << " vars, " << cons.size() << " constraints\n";
	}
}

void Solver::CSPGridActions() {
	gridActions.clear();
	for (int mine : mines) {
		Coord coord = frontierCells[mine];
		gridActions.push_back({ RCLICK, coord.x, coord.y });
	}
	for (int safeCell : safeCells) {
		Coord coord = frontierCells[safeCell];
		gridActions.push_back({ LCLICK, coord.x, coord.y });
	}
}

void Solver::CSPTurn() {
	collectFrontier();
	collectConstraints();

	findSections();
	solveSections();

	CSPGridActions();
}