#include "Cell.h"


// CellType implementation
CellType::CellType(Type t) : type(t) {
    switch (type) {
    case AIR:
        color = Color::AIR;
        break;
    case STONE:
        color = Color::STONE;
        break;
    case SAND:
        color = Color::SAND;
        break;
    default: // Error type
        color = Color::PHSORNG;
        break;
    }
}



// Cell implementation
Cell::Cell() : type(CellType::AIR) {}

void Cell::changeCellType(const CellType& ty) {
    type = ty;
}

const CellType& Cell::getCellType() const {
    return type;
}

void Cell::update() {
    
}