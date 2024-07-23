#include "Cell.h"

// CellBehavior implementation
CellBehavior::CellBehavior(bool p) : permeable(p) {}

CellBehavior::~CellBehavior() {}

// AirBehavior implementation
AirBehavior& AirBehavior::instance() {
    static AirBehavior instance;
    return instance;
}

void AirBehavior::update() {
    // Air cells do nothing
}

AirBehavior::AirBehavior() : CellBehavior(true) {}

// StoneBehavior implementation
StoneBehavior& StoneBehavior::instance() {
    static StoneBehavior instance;
    return instance;
}

void StoneBehavior::update() {
    // Stone cells do nothing but other blocks can't pass through them
}

StoneBehavior::StoneBehavior() : CellBehavior(false) {}

// SandBehavior implementation
SandBehavior& SandBehavior::instance() {
    static SandBehavior instance;
    return instance;
}

void SandBehavior::update() {
    // Sand falls straight down (need to figure out piling)
}

SandBehavior::SandBehavior() : CellBehavior(true) {}

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