#pragma once

#ifndef GRID_H
#define GRID_H

#include <vector>
#include <unordered_set>
#include "Cell.h"
#include "Settings.h"
#include <SFML/Graphics.hpp>
#include "vecHash.h"

class Grid {
public:
    Grid();

    const std::vector<std::vector<Cell>>& getGrid() const;
    const CellType& getCellTypeAt(const int& row, const int& col) const;
    void updateCellTypeAt(const int& row, const int& col, const CellType& ty);
    void resetGrid();
    void randomizeGrid();
    void incIterNum();
    void addHighlightedCell(const sf::Vector2i& cell);
    const bool isCellHighlighted(const sf::Vector2i& cell) const;
    void clearHighlightedCells();
    const std::unordered_set<sf::Vector2i>& getHighlightedCells() const;

private:
    std::vector<std::vector<Cell>> grid; // Can be 1d vector instead
    std::unordered_set<sf::Vector2i> highlightedCells;
    uint32_t iterNum = 0;

    GoL_Settings& gols = GoL_Settings::getSettings();

    const CellType& getRandCellType(); // Assuming this function is defined somewhere
};

#endif // GRID_H