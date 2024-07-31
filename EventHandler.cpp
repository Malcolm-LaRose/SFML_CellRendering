#include "EventHandler.h"

// Constructor
EventHandler::EventHandler(Grid& grid, Renderer* rend, World& world)
    : grid(grid), renderer(rend), world(world) {
    window = &(renderer->window);
}

void EventHandler::handleEvents() {
    sf::Event event;

    while (window->pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            window->close();
            running = false;
            break;
        case sf::Event::MouseButtonPressed: {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            const int& col = mousePos.x / gols.cellDist;
            const int& row = mousePos.y / gols.cellDist;
            firstPos = sf::Vector2i(row, col);
            break;
        }
        case sf::Event::MouseButtonReleased: {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            const int& col = mousePos.x / gols.cellDist;
            const int& row = mousePos.y / gols.cellDist;

            if (event.mouseButton.button == sf::Mouse::Left) {
                grid.clearHighlightedCells();
                bresenhamTool(firstPos.x, firstPos.y, row, col);
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                grid.clearHighlightedCells();
                plotCircle(sf::Vector2i(row, col), firstPos);
            }
            break;
        }
        case sf::Event::MouseMoved: {
            handleMouseHover();

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                bresenhamLineHighlighter(firstPos.x, firstPos.y, secondPos.x, secondPos.y);
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                circleHighlighter(secondPos, firstPos);
            }
            break;
        }
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Space) { // Single step
                world.update(); // Need to tell World this (function pointer?)
            }
            if (event.key.code == sf::Keyboard::P) { // Pause/unpause continuous stepping
                paused = !paused;
                if (!paused) {
                    timer = 0.0f;
                }
            }
            if (event.key.code == sf::Keyboard::R) {
                grid.resetGrid();
            }
            if (event.key.code == sf::Keyboard::Z) {
                grid.randomizeGrid();
            }
            break;
        }
    }
}

void EventHandler::handleMouseHover() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    const int mouseCol = mousePos.x / gols.cellDist;
    const int mouseRow = mousePos.y / gols.cellDist;

    if (sf::Vector2i(mouseRow, mouseCol) != secondPos) { // Skips clearing and adding if mouse hasn't moved to a new cell
        secondPos = sf::Vector2i(mouseRow, mouseCol);

        grid.clearHighlightedCells();
        grid.addHighlightedCell(secondPos);
    }
}

void EventHandler::bresenhamTool(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    while (true) {
        grid.updateCellTypeAt(x0, y0, CellType::SAND);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
}

void EventHandler::bresenhamLineHighlighter(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    grid.clearHighlightedCells(); // Clear previous highlights

    while (true) {
        grid.addHighlightedCell(sf::Vector2i(x0, y0));
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
}

void EventHandler::plotCircle(const sf::Vector2i& pos1, const sf::Vector2i& pos2) {
    int xm = pos2.x;
    int ym = pos2.y;

    int r = dists(pos1, pos2);

    int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
    do {
        grid.updateCellTypeAt(xm - x, ym + y, CellType::SAND); /*   I. Quadrant */
        grid.updateCellTypeAt(xm - y, ym - x, CellType::SAND); /*  II. Quadrant */
        grid.updateCellTypeAt(xm + x, ym - y, CellType::SAND); /* III. Quadrant */
        grid.updateCellTypeAt(xm + y, ym + x, CellType::SAND); /*  IV. Quadrant */
        r = err;
        if (r <= y) err += ++y * 2 + 1;           /* e_xy+e_y < 0 */
        if (r > x || err > y) err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);
}

void EventHandler::circleHighlighter(const sf::Vector2i& pos1, const sf::Vector2i& pos2) {
    int xm = pos2.x;
    int ym = pos2.y;

    int r = dists(pos1, pos2);

    grid.clearHighlightedCells();

    int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
    do {
        grid.addHighlightedCell(sf::Vector2i(xm - x, ym + y)); /*   I. Quadrant */
        grid.addHighlightedCell(sf::Vector2i(xm - y, ym - x)); /*  II. Quadrant */
        grid.addHighlightedCell(sf::Vector2i(xm + x, ym - y)); /* III. Quadrant */
        grid.addHighlightedCell(sf::Vector2i(xm + y, ym + x)); /*  IV. Quadrant */
        r = err;
        if (r <= y) err += ++y * 2 + 1;           /* e_xy+e_y < 0 */
        if (r > x || err > y) err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);
}