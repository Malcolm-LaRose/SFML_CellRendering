#include "Grid.h"

Grid::Grid() : grid(gols.rows, std::vector<Cell>(gols.cols)) {
    // Initialization code if necessary
}

const std::vector<std::vector<Cell>>& Grid::getGrid() const {
    return grid;
}

const CellType& Grid::getCellTypeAt(const int& row, const int& col) const {
    return grid[row][col].getCellType();
}

void Grid::updateCellTypeAt(const int& row, const int& col, const CellType& ty) {
    if (col < 0 || col >= gols.cols) return;
    if (row < 0 || row >= gols.rows) return;

    grid[row][col].changeCellType(ty);
}

void Grid::resetGrid() {
    for (int row = 0; row < gols.rows; row++) {
        for (int col = 0; col < gols.cols; col++) {
            updateCellTypeAt(row, col, CellType::AIR);
        }
    }
    iterNum = 0;
}

void Grid::randomizeGrid() {
    for (int row = 0; row < gols.rows; row++) {
        for (int col = 0; col < gols.cols; col++) {
            const CellType& randCellType = getRandCellType();
            updateCellTypeAt(row, col, randCellType);
        }
    }
    iterNum = 0;
}

void Grid::incIterNum() {
    ++iterNum;
}

void Grid::addHighlightedCell(const sf::Vector2i& cell) {
    highlightedCells.insert(cell);
}

const bool Grid::isCellHighlighted(const sf::Vector2i& cell) const {
    return highlightedCells.find(cell) != highlightedCells.end();
}

void Grid::clearHighlightedCells() {
    highlightedCells.clear();
}

const std::unordered_set<sf::Vector2i>& Grid::getHighlightedCells() const {
    return highlightedCells;
}

const CellType& Grid::getRandCellType() {
    const int randInt = genRandomInt(0, CellType::_CELLTYPES - 1);
    return CellType::Type(randInt);
}