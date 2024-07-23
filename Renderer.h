#pragma once


#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "Grid.h"
#include "Settings.h"

class Renderer {
public:
    Renderer(Grid& grid);

    void renderAll();
    void clearCells();
    void frameCounterDisplay(const int& frameTime, const int& avg);

    sf::RenderWindow window;
    sf::Clock clock;

private:
    sf::Font font;
    sf::Text frameText;

    sf::VertexBuffer borderAndBGRect;
    sf::VertexArray cells;

    Grid& grid;

    void calcVertices();
    void initFont();
    void setupVertexBuffer(sf::VertexBuffer& vertexBuffer, const int& xPos, const int& yPos, const int& width, const int& height, const sf::Color& color);
    void renderWorld();
    void renderVertices();

    GoL_Settings& gols = GoL_Settings::getSettings();
};

#endif