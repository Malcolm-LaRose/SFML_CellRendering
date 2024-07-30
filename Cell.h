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

private:
    CellType type;
};

#endif