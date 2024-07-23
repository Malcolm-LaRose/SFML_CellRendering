#pragma once


#ifndef SETTINGS_H
#define SETTINGS_H

#include <random>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstring>

struct GoL_Settings {
    // Declaration of static members
    inline static GoL_Settings& getSettings() {
        static GoL_Settings instance;
        return instance;
    }
    static constexpr char borderSize = 0;
    static constexpr char xPos = borderSize;
    static constexpr char yPos = borderSize;
    static constexpr int cols = 640;
    static constexpr int rows = 360;
    static constexpr int numCells = rows * cols;
    static constexpr char cellSpacing = borderSize;
    static constexpr char cellSize = 4;
    static constexpr char cellDist = cellSize + cellSpacing;
    static constexpr char targetFPS = 60;
    static constexpr int initScreenWidth = cols * cellDist + borderSize;
    static constexpr int initScreenHeight = rows * cellDist + borderSize;
    static constexpr char stepsPerSec = 22;
};

// Function declarations
int genRandomInt(const int& min, const int& max);
float genRandomFloat(const int& min, const int& max);
bool genRandomBool(const float& pTrue);
const int& dists(const sf::Vector2i& pos1, const sf::Vector2i& pos2);
std::string itostr(unsigned int val);

#endif // SETTINGS_H