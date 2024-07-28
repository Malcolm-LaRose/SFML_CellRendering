#include "Cell.h"

// CellBehavior implementation
CellBehavior::CellBehavior(State st) : state(st) {}

CellBehavior::~CellBehavior() {}

// AirBehavior implementation
AirBehavior& AirBehavior::instance() {
    AirBehavior instance;
    return instance;
}

void AirBehavior::update() {
    // Air cells do nothing
}

AirBehavior::AirBehavior() : CellBehavior(GAS) {}

// StoneBehavior implementation
StoneBehavior& StoneBehavior::instance() {
    static StoneBehavior instance;
    return instance;
}

void StoneBehavior::update() {
    // Stone cells do nothing but other blocks can't pass through them
}

StoneBehavior::StoneBehavior() : CellBehavior(SOLID) {}

// SandBehavior implementation
SandBehavior& SandBehavior::instance() {
    static SandBehavior instance;
    return instance;
}

void SandBehavior::update() {
    // Called on a cell... 


    // Get current row/col --> how?


    // If space below is empty, move down and return
    

    // If space empty both down and left and right, pick at random

    // If space empty down and left move down and left

    // If space empty down and right move down and right


}

SandBehavior::SandBehavior() : CellBehavior(SOLID) {}

// CellType implementation
CellType::CellType(Type t) : type(t) {
    switch (type) {
    case AIR:
        color = Color::AIR;
        behavior = &AirBehavior::instance();
        break;
    case STONE:
        color = Color::STONE;
        behavior = &StoneBehavior::instance();
        break;
    default: // Error type
        color = Color::PHSORNG;
        behavior = nullptr;
        break;
    }
}

void CellType::update() const {
    if (behavior != nullptr) {
        behavior->update();
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