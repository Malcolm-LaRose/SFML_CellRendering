#include "World.h"

// Constructor
World::World() : window(nullptr), clock(nullptr), renderer(nullptr) {
    renderer = new Renderer(grid);
    window = &(renderer->window);
    clock = &(renderer->clock);
    eventHandler = new EventHandler(grid, renderer, *this);
}

void World::mainLoop() {
    float frameTime = 0;
    float totalFrameTime = 0;
    int frameCount = 0;

    const float& delay = 1.0f / gols.stepsPerSec;

    while ((eventHandler->running)) {
        frameTime = clock->restart().asMicroseconds();
        totalFrameTime += frameTime;
        eventHandler->timer += frameTime / 1000000.0f;

        eventHandler->handleEvents();
        if ((eventHandler->timer) >= delay) {
            if (!(eventHandler->paused)) {
                update();
            }
            (eventHandler->timer) -= delay;
        }
        renderer->renderAll();
        renderer->frameCounterDisplay(frameTime, frameCount / (totalFrameTime / 1000000));
        frameCount++;
        window->display();
    }

    renderer->clearCells(); // Clear the cells vertex array, is this nec?
}

inline void World::update() {
    // Call update for each cell here, update behavior to be determined by celltype
    grid.updateGrid(); // 
}