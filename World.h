#pragma once


#ifndef WORLD_H
#define WORLD_H

#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "EventHandler.h"
#include "Grid.h"
#include "Settings.h" // Assuming this is where `gols` is defined

class EventHandler;

class World {
public:
    World();

    void mainLoop();
    void update(); // Defined inline if it’s a simple method

private:
    Renderer* renderer;
    sf::RenderWindow* window;
    sf::Clock* clock;
    EventHandler* eventHandler;
    Grid grid;

    GoL_Settings& gols = GoL_Settings::getSettings();
};

#endif