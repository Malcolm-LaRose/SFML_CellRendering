#include "Renderer.h"

// Constructor
Renderer::Renderer(Grid& grid) : grid(grid) {
    // Init SFML
    window.create(sf::VideoMode(gols.initScreenWidth, gols.initScreenHeight), "Physics Box", sf::Style::Titlebar | sf::Style::Close);
    window.setKeyRepeatEnabled(false);
    window.setVerticalSyncEnabled(false); // Probably ignored by driver
    window.setMouseCursorVisible(false);
    // window.setFramerateLimit(targetFPS);

    // Init background
    setupVertexBuffer(borderAndBGRect, 0, 0, gols.initScreenWidth, gols.initScreenHeight, Color::EIGENGRAU);

    // Calculate cell positions
    if constexpr (gols.cellDist == 1) {
        cells.setPrimitiveType(sf::Points);
        cells.resize(gols.numCells);
    }
    else {
        cells.setPrimitiveType(sf::Triangles);
        cells.resize(gols.numCells * 6); // Leak
    }
    calcVertices();

    // Init Font
    initFont();

    // Set frame counter position
    frameText.setPosition(gols.borderSize, gols.borderSize);
}

inline void Renderer::renderAll() {
    renderWorld();
    renderVertices();
}

inline void Renderer::clearCells() {
    cells.clear();
}

void Renderer::frameCounterDisplay(const int& frameTime, const int& avg) {
    frameText.setString("F Time (us): " + std::to_string(frameTime) + "\nAvg FPS: " + std::to_string(avg)); // Using std::to_string

    window.draw(frameText);
}

void Renderer::calcVertices() {
    if constexpr (gols.cellDist == 1) {
        int i = 0;
        for (int row = 0; row < gols.rows; ++row) {
            for (int col = 0; col < gols.cols; ++col) {
                const int& x = col * (gols.cellDist) + gols.borderSize;
                const int& y = row * (gols.cellDist) + gols.borderSize;

                const sf::Color& color = grid.getCellTypeAt(row, col).color;

                cells[i].position = sf::Vector2f(x, y);
                cells[i].color = color;
                ++i;
            }
        }
    }
    else {
        int i = 0;
        for (int row = 0; row < gols.rows; ++row) {
            for (int col = 0; col < gols.cols; ++col) {
                // Position of upper left corner of cell
                const int& x = col * (gols.cellDist) + gols.borderSize;
                const int& y = row * (gols.cellDist) + gols.borderSize;

                const sf::Color& color = grid.getCellTypeAt(row, col).color;

                // First triangle (top-left, top-right, bottom-right)
                cells[i].position = sf::Vector2f(x, y);
                cells[i + 1].position = sf::Vector2f(x + gols.cellDist, y);
                cells[i + 2].position = sf::Vector2f(x + gols.cellDist, y + gols.cellDist);

                // Second triangle (top-left, bottom-right, bottom-left)
                cells[i + 3].position = sf::Vector2f(x, y);
                cells[i + 4].position = sf::Vector2f(x + gols.cellDist, y + gols.cellDist);
                cells[i + 5].position = sf::Vector2f(x, y + gols.cellDist);

                for (int j = 0; j < 6; ++j) {
                    cells[i + j].color = color;
                }

                i += 6; // Move to the next set of vertices
            }
        }
    }
}

void Renderer::initFont() {
    font.loadFromFile(".\\Montserrat-Regular.ttf");
    frameText.setCharacterSize(24);
    frameText.setFillColor(Color::WHITE);
    frameText.setFont(font);
}

void Renderer::setupVertexBuffer(sf::VertexBuffer& vertexBuffer, const int& xPos, const int& yPos, const int& width, const int& height, const sf::Color& color) {
    sf::Vertex vertices[6];

    // First triangle (top-left, top-right, bottom-right)
    vertices[0].position = sf::Vector2f(xPos, yPos);
    vertices[1].position = sf::Vector2f(xPos + width, yPos);
    vertices[2].position = sf::Vector2f(xPos + width, yPos + height);

    // Second triangle (top-left, bottom-right, bottom-left)
    vertices[3].position = sf::Vector2f(xPos, yPos);
    vertices[4].position = sf::Vector2f(xPos + width, yPos + height);
    vertices[5].position = sf::Vector2f(xPos, yPos + height);

    for (int i = 0; i < 6; ++i) {
        vertices[i].color = color;
    }

    // Create the vertex buffer
    vertexBuffer.create(6);
    vertexBuffer.setPrimitiveType(sf::Triangles);
    vertexBuffer.update(vertices);
}

inline void Renderer::renderWorld() {
    window.draw(borderAndBGRect);
}

void Renderer::renderVertices() {
    if constexpr (gols.cellDist == 1) {
        int i = 0;
        for (int row = 0; row < gols.rows; ++row) {
            for (int col = 0; col < gols.cols; ++col) {
                sf::Color color = grid.getCellTypeAt(row, col).color;

                if (grid.isCellHighlighted(sf::Vector2i(row, col))) {
                    color = Color::CYAN; // Highlight color
                }

                cells[i].color = color;
                ++i;
            }
        }
        window.draw(cells); // Maybe move to render world function
    }
    else {
        int i = 0;
        for (int row = 0; row < gols.rows; ++row) {
            for (int col = 0; col < gols.cols; ++col) {
                sf::Color color = grid.getCellTypeAt(row, col).color;

                if (grid.isCellHighlighted(sf::Vector2i(row, col))) {
                    color = Color::CYAN; // Highlight color
                }

                // Set the color of the vertices
                for (int j = 0; j < 6; ++j) {
                    cells[i + j].color = color; // Change this to the desired color
                }

                i += 6; // Move to the next set of vertices
            }
        }
        window.draw(cells);
    }
}