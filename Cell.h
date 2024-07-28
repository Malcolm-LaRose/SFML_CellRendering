#pragma once


#ifndef CELL_H
#define CELL_H

#include "Color.h"



class CellBehavior {
public:
    virtual void update() = 0;
    virtual ~CellBehavior(); // Ensure virtual destructor for polymorphic behavior

    enum State {
        SOLID, // Nothing passes through solids
        LIQUID, // Liquids flow
        GAS // Gasses do nothing
        
    } state;

protected:
    CellBehavior(State st);
};



struct CellType {
    enum Type {
        AIR,
        STONE,

        _CELLTYPES // Special member to count the number of cell types
    } type;

    sf::Color color;

    CellType(Type t);
};

class Cell {
public:
    Cell();

    void changeCellType(const CellType& ty);
    const CellType& getCellType() const;

private:
    CellType type;
};

#endif