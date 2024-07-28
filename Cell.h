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

class AirBehavior : public CellBehavior {
public:
    static AirBehavior& instance();

    void update() override; // Air does nothing but is permeable

private:
    AirBehavior();
};

class StoneBehavior : public CellBehavior {
public:
    static StoneBehavior& instance();

    void update() override; // Stone does nothing but is impermeable

private:
    StoneBehavior();
};

class SandBehavior : public CellBehavior {
public:
    static SandBehavior& instance();

    void update() override;

private:
    SandBehavior();
};

struct CellType {
    enum Type {
        AIR,
        STONE,

        _CELLTYPES // Special member to count the number of cell types
    } type;

    sf::Color color;
    CellBehavior* behavior;

    CellType(Type t);

    void update() const;
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