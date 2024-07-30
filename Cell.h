#pragma once


#ifndef CELL_H
#define CELL_H

#include "Color.h"


struct CellType { // Cell type should determine color and behavior...
    enum Type {
        AIR,
        STONE,
        SAND,

        _CELLTYPES // Special member to count the number of cell types
    } type;

    sf::Color color;

    CellType(Type t);
};

class Cell { // Maybe make me an abstract class?
public:
    Cell();

    void changeCellType(const CellType& ty);
    const CellType& getCellType() const;

    // Would be good to switch over celltype here and update accordingly, rather than switch every call of update
    void update() {} // Update could be a function pointer whose address to the correct update behavior is determined in cell type implementation

private:
    CellType type;
};

#endif