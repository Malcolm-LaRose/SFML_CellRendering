#pragma once


#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <SFML/Graphics.hpp>
#include "Grid.h"
#include "Renderer.h"
#include "World.h"
#include "Settings.h"

class World;

class EventHandler {
public:
    EventHandler(Grid& grid, Renderer* rend, World& world);

    void handleEvents();

    bool running = true;
    bool paused = true; // Start paused
    float timer = 0.0f; // For simtime calculations

private:
    sf::Vector2i firstPos;
    sf::Vector2i secondPos;

    Grid& grid;
    World& world;
    Renderer* renderer;
    sf::RenderWindow* window;

    void handleMouseHover();
    void bresenhamTool(int x0, int y0, int x1, int y1);
    void bresenhamLineHighlighter(int x0, int y0, int x1, int y1);
    void plotCircle(const sf::Vector2i& pos1, const sf::Vector2i& pos2);
    void circleHighlighter(const sf::Vector2i& pos1, const sf::Vector2i& pos2);

    GoL_Settings& gols = GoL_Settings::getSettings();
};


#endif