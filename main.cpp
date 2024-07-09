// Sandy Rendering
// 
// Goals for AFTER completion --> don't touch until multiple types works first, do roughly in order
//
// Clean up/ refactor code to make more modular/extendable --> Move on from this for now except for items below
// --> Try to separate GOL code from rendering code as much as possible
// --> Separate bresenham algorithm from cell flipping so I can resuse it more easily (line and circle previews...) --> Do line and circle preview functions
// Add error checking
// more optimization...



#include "color.h"
#include "Settings.h"

#include <GL/glew.h>

#include <SFML/Graphics.hpp>

#include <array>
#include <cmath>
// #include <iostream>
#include <vector>

GoL_Settings& gols = GoL_Settings::getSettings();




struct CellType { // Binds type to color, can extend later to more things like behaviour
	enum Type {
		OFF,
		ON, // Essentially 'error' type for any higher cell types

		_CELLTYPES // Special member to count the number of cell types
	} type;

	const sf::Color color() const {
		switch (type) {
		case OFF: return Color::DRKGRY;  // Example color
		case ON: return Color::PHSORNG;   // Example color
		default: return sf::Color::Transparent; // Fallback color
		}
	}

	CellType(Type t) : type(t) {}
};

const CellType& getRandCellType() {
	const int randInt = genRandomInt(0, CellType::_CELLTYPES - 1);
	return CellType::Type(randInt); 
}


class Cell {
public:
	Cell() : type(CellType::OFF) {}

	void updateCellType(const CellType& ty) {
		type = ty;
	}
	 
	void flipCellType() { //  On / Off
		if (type.type == CellType::OFF) {
			type = CellType::ON;
		return;
		}
		else if (type.type == CellType::ON) type = CellType::OFF;
		return;
	}

	const CellType& getCellType() const {
		return type;
	}


private:
	CellType type; 

};

class Grid {
public:

	Grid() {
		grid.resize(gols.rows, std::vector<Cell>(gols.cols)); 
	}

	const std::vector<std::vector<Cell>>& getGrid() const {
		return grid;
	}

	const CellType& getCellTypeAt(const int& row, const int& col) const {
		const Cell& cell = grid[row][col];
		return cell.getCellType();
	}

	void updateCellTypeAt(const int& row, const int& col, const CellType& ty) {
		grid[row][col].updateCellType(ty);
	}

	void flipCellTypeAt(const int& row, const int& col) {

		if (row < 0 || row >= gols.rows || col < 0 || col >= gols.cols) return;

		grid[row][col].flipCellType();
	}

	void resetGrid() {
		for (int row = 0; row < gols.rows; row++) {
			for (int col = 0; col < gols.cols; col++) {
				updateCellTypeAt(row, col, CellType::OFF);
				iterNum = 0;
			}
		}
	}

	void randomizeGrid() {
		for (int row = 0; row < gols.rows; row++) {
			for (int col = 0; col < gols.cols; col++) {
				const CellType& randCellType = getRandCellType();
				updateCellTypeAt(row, col, randCellType);
				iterNum = 0;
			}
		}
	}

	void incIterNum() {
		++iterNum;
	}


private:

	std::vector<std::vector<Cell>> grid; // MIGHT BE CHEAPER TO USE POINTERS SOON

	uint32_t iterNum = 0;


};

class GameOfLife {
public:
	GameOfLife() {}

	void gameOfLife() {
		Grid updatedGrid;
		bool gridChanged = false;

		for (int row = 0; row < gols.rows; row++) { // Make sure not to check edges
			for (int col = 0; col < gols.cols; col++) {
				int numAlive = checkMooreNeighborhoodFor(row, col, CellType::ON);

				const CellType& currentCellType = grid.getCellTypeAt(row, col);

				// Actual GoL
				if (currentCellType.type == CellType::ON) {
					if (numAlive < 2 || numAlive > 3) {
						updatedGrid.updateCellTypeAt(row, col, CellType::OFF);
						if (!gridChanged) gridChanged = true;
					}
					else {
						updatedGrid.updateCellTypeAt(row, col, CellType::ON);
					}
				}
				else if (currentCellType.type == CellType::OFF) {
					if (numAlive == 3) {
						updatedGrid.updateCellTypeAt(row, col, CellType::ON);
						if (!gridChanged) gridChanged = true;
					}
				}


			}
		}

		if (gridChanged) {
			for (int row = 0; row < gols.rows; ++row) {
				for (int col = 0; col < gols.cols; ++col) {
					const CellType& nextType = updatedGrid.getCellTypeAt(row, col);
					grid.updateCellTypeAt(row, col, nextType);
				}
			}
		}
		grid.incIterNum();

	}

	Grid grid;

private:

	const bool isInBounds(const int& row, const int& col) const {
		if (row < 0 || col < 0) {
			return false;
		}
		if (row >= gols.rows || col >= gols.cols) {
			return false;
		}

		else {
			return true;
		}
	}

	const int& checkMooreNeighborhoodFor(const int& row, const int& col, const CellType& ty) const {
		int count = 0;
		for (int i = -1; i <= 1; ++i) {
			for (int j = -1; j <= 1; ++j) {
				if (i == 0 && j == 0) continue;
				if (!isInBounds(row + i, col + j)) {
					continue;
				}
				if ((grid.getCellTypeAt(row + i, col + j).type == ty.type)) {
					count++;
				}
			}
		}

		return count;
	}

};



class World {
public:

	World() {

		// Init SFML
		window.create(sf::VideoMode(gols.initScreenWidth, gols.initScreenHeight), "Physics Box", sf::Style::Titlebar | sf::Style::Close);
		window.setKeyRepeatEnabled(false);
		window.setVerticalSyncEnabled(false); // Probably ignored by driver
		window.setMouseCursorVisible(false);
		// window.setFramerateLimit(targetFPS);


		// Init background
		setupVertexBuffer(borderAndBGRect, 0, 0, gols.initScreenWidth, gols.initScreenHeight, Color::EIGENGRAU);

		// Calculate cell positions
		if (gols.cellDist == 1) {
			cells.setPrimitiveType(sf::Points);
			cells.resize(gols.rows * gols.cols);
		}
		else {
			cells.setPrimitiveType(sf::Triangles);
			cells.resize(gols.rows * gols.cols * 6);
		}
		calcVertices();

		// Init Font
		initFont();

		// Set framecounter position
		frameText.setPosition(gols.borderSize, gols.borderSize);
	}

	~World() {
		borderAndBGRect.~VertexBuffer();
	}

	void setupVertexBuffer(sf::VertexBuffer& vertexBuffer, const int& xPos, const int& yPos, const int& width, const int& height, const sf::Color& color) {

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


	void frameCounterDisplay(const int& frameTime, const int& avg) {
		frameText.setString("F Time (us): " + itostr(frameTime) + "\nAvg FPS: " + itostr(avg)); // Maybe do with c style strings?

		window.draw(frameText);
	}

	void mainLoop() {

		float frameTime = 0;
		float totalFrameTime = 0;
		int frameCount = 0;

		const float& delay = 1.0f / gols.stepsPerSec;


		while (running) {

			frameTime = clock.restart().asMicroseconds();
			totalFrameTime += frameTime;
			timer += frameTime / 1000000.0f;

			handleEvents();
			if (timer >= delay) {
				if (!paused) {
					update();
				}
				timer -= delay;
			}
			renderAll();
			frameCounterDisplay(frameTime, frameCount / (totalFrameTime / 1000000));
			frameCount++;
			window.display();

		}



	}



private:

	sf::RenderWindow window;
	sf::Clock clock;
	sf::Font font;
	sf::Text frameText;

	sf::VertexBuffer borderAndBGRect;
	sf::VertexArray cells;

	GameOfLife game;
	Grid& grid = game.grid;

	bool running = true;
	bool paused = true; // Start paused
	float timer = 0; // For simtime calcs

	sf::Mouse mouse; // Could this be used?
	sf::Vector2i firstPos;
	sf::Vector2i secondPos;

	void initFont() {
		font.loadFromFile(".\\Montserrat-Regular.ttf");
		frameText.setCharacterSize(24);
		frameText.setFillColor(Color::WHITE);
		frameText.setFont(font);
	}


	void handleEvents() {
		sf::Event event;

		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				running = false;
				break;
			case sf::Event::MouseButtonPressed: {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				const int& col = mousePos.x / gols.cellDist;
				const int& row = mousePos.y / gols.cellDist;
				firstPos = sf::Vector2i(row, col);

				// Line preview tool maybe goes here? How to turn off line preview when mouse released...

				break;
			}
			case sf::Event::MouseButtonReleased: {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				const int& col = mousePos.x / gols.cellDist;
				const int& row = mousePos.y / gols.cellDist;
				secondPos = sf::Vector2i(row, col);

				if (event.mouseButton.button == sf::Mouse::Left) {
					// DDATool(secondPos, firstPos);
					bresenhamTool(firstPos.x, firstPos.y, secondPos.x, secondPos.y); // Cleaner endpoints than DDA
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					plotCircle(secondPos, firstPos);
				}
				break;
			}
			case sf::Event::MouseMoved: {

				break;
			}
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Space) { // Single step
					update();
				}
				if (event.key.code == sf::Keyboard::P) { // Pause/unpause continuous stepping
					paused = !paused;
					if (!paused) {
						timer = 0.0f;
					}
				}
				if (event.key.code == sf::Keyboard::R) {
					grid.resetGrid(); // This is why we wanted grid to be public (private seems overused!)
				}
				if (event.key.code == sf::Keyboard::Z) {
					grid.randomizeGrid();
				}
				break;
			}
		}
	}

	void bresenhamTool(int x0, int y0, int x1, int y1) { // Draw a line between point a and point b 
		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx + dy, e2; /* error value e_xy */

		while (true) { 
			grid.flipCellTypeAt(x0, y0);
			if (x0 == x1 && y0 == y1) break;
			e2 = 2 * err;
			if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
			if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
		}
	}

	void linePreview(const sf::Vector2f& firstPos, const sf::Vector2f& currentPos) {
		// Make a line from 1st pos to current mouse pos
		// 
		// Store that line in an array
		// Flip cells corresponding to that line to cyan
		// on each update, check if the line has changed
		// If the line changed, revert old line to cyan and make new line

	}

	void plotCircle(const sf::Vector2i& pos1, const sf::Vector2i& pos2) {
		int xm, ym;

		xm = pos2.x;
		ym = pos2.y;

		int r = dist(pos1, pos2);

		int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
		do {
			grid.flipCellTypeAt(xm - x, ym + y); /*   I. Quadrant */
			grid.flipCellTypeAt(xm - y, ym - x); /*  II. Quadrant */
			grid.flipCellTypeAt(xm + x, ym - y); /* III. Quadrant */
			grid.flipCellTypeAt(xm + y, ym + x); /*  IV. Quadrant */
			r = err;
			if (r <= y) err += ++y * 2 + 1;           /* e_xy+e_y < 0 */
			if (r > x || err > y) err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
		} while (x < 0);
	}

	void update() {
		game.gameOfLife();
	}


	void renderWorld() {
		window.draw(borderAndBGRect);
	}

	void calcVertices() {
		if (gols.cellDist == 1) {

			int i = 0;
			for (int row = 0; row < gols.rows; ++row) {
				for (int col = 0; col < gols.cols; ++col) {
					const int& x = col * (gols.cellDist) + gols.borderSize;
					const int& y = row * (gols.cellDist) + gols.borderSize;

					const sf::Color& color = grid.getCellTypeAt(row, col).color();

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

					const sf::Vector2i& pos = sf::Mouse::getPosition(window);

					const sf::Color& color = grid.getCellTypeAt(row, col).color();


					// First triangle (top-left, top-right, bottom-right)
					cells[i].position = sf::Vector2f(x, y);
					cells[i + 1].position = sf::Vector2f(x + gols.cellDist, y);
					cells[i + 2].position = sf::Vector2f(x + gols.cellDist, y + gols.cellDist);

					// Second triangle (top-left, bottom-right, bottom-left)
					cells[i + 3].position = sf::Vector2f(x, y);
					cells[i + 4].position = sf::Vector2f(x + gols.cellDist, y + gols.cellDist);
					cells[i + 5].position = sf::Vector2f(x, y + gols.cellDist);

					// Set the color of the vertices
					for (int j = 0; j < 6; ++j) {
						cells[i + j].color = color; // Change this to the desired color
					}

					i += 6; // Move to the next set of vertices

				}
			}
		}
	}

	void vertexRenderGrid() { // Maybe separate color flipping from rendering...

		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		const int& mouseCol = mousePos.x / gols.cellDist;
		const int& mouseRow = mousePos.y / gols.cellDist;

		if (gols.cellDist == 1) {

			int i = 0;
			for (int row = 0; row < gols.rows; ++row) {
				for (int col = 0; col < gols.cols; ++col) {

					sf::Color color = grid.getCellTypeAt(row, col).color();

					if ((row == mouseRow) && (col == mouseCol)) {
						color = Color::CYAN;
					}


					cells[i].color = color;
					++i;
				}
			}

			window.draw(cells);
		}
		else {
			int i = 0;
			for (int row = 0; row < gols.rows; ++row) {
				for (int col = 0; col < gols.cols; ++col) {

					sf::Color color = grid.getCellTypeAt(row, col).color();

					if ((row == mouseRow) && (col == mouseCol)) {
						color = Color::CYAN;
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


	void renderAll() {
		renderWorld();
		vertexRenderGrid();
	}

};


int main() {

	World world;

	world.mainLoop();

	return 0;
}