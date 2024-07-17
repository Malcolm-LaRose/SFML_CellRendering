// Sandy Rendering
// 
// GOALS
// 
// Address potential future bottlenecks
// --> Make the cells vertex array a vertex buffer
// --> Learn about hashing / hash tables
// --> The current implementation redraws the entire grid every frame, which can be optimized by only updating the vertices of cells that changed state.
// --> Make grid store cells linearly (only one vector) (way later, figure out sand stuff first)
// 
// Clean up/ refactor code to make more modular/extendable --> Move on from this for now except for items below
// --> Break into more smaller classes (maybe?) --> e.g. event handling separately?
// --> Add error checking/ reporting
// Add gui stuff: debug info, tool selection, etc...



#include "Color.h"
#include "Settings.h"

#include <GL/glew.h>

#include <SFML/Graphics.hpp>

#include <array>
#include <algorithm>
#include <cmath>
// #include <iostream> // Debugging only
#include <vector>
#include <unordered_set>

GoL_Settings& gols = GoL_Settings::getSettings();

// Define hash function for sf::Vector2i --> I don't understand how this works
namespace std {
	template<> struct hash<sf::Vector2i> {
		size_t operator()(const sf::Vector2i& vec) const {
			// Use a simple hash combining x and y values
			return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1);
		}
	};
}


struct CellType { // Binds type to color, can extend later to more things like behaviour
	enum Type {
		AIR, // New default should be air
		STONE, // Can be stone instead

		_CELLTYPES // Special member to count the number of cell types
	} type;

	const sf::Color color() const {
		switch (type) {
		case AIR: return Color::TRANSP;  // Example color
		case STONE: return Color::DRKGRY;   // Example color
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
	Cell() : type(CellType::AIR) {}

	void updateCellType(const CellType& ty) {
		type = ty;
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
		return grid[row][col].getCellType();
	}

	void updateCellTypeAt(const int& row, const int& col, const CellType& ty) {

		if (col < 0 || col >= gols.cols) return;
		if (row < 0 || row >= gols.rows) return;

		grid[row][col].updateCellType(ty);
	}

	void resetGrid() {
		for (int row = 0; row < gols.rows; row++) {
			for (int col = 0; col < gols.cols; col++) {
				updateCellTypeAt(row, col, CellType::AIR);
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

	inline void incIterNum() {
		++iterNum;
	}

	void addHighlightedCell(const sf::Vector2i& cell) {
		highlightedCells.insert(cell);
	}

	const bool isCellHighlighted(const sf::Vector2i& cell) const {
		return highlightedCells.find(cell) != highlightedCells.end();
	}

	inline void clearHighlightedCells() {
		highlightedCells.clear();
	}

	const std::unordered_set<sf::Vector2i>& getHighlightedCells() const {
		return highlightedCells;
	}


private:

	std::vector<std::vector<Cell>> grid; // Can be 1d vector instead

	std::unordered_set<sf::Vector2i> highlightedCells; 

	uint32_t iterNum = 0;


};

class TypeSelection {

	// Should make a GUI when the appropriate button is pressed for selecting cell type used for the tools


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
			cells.resize(gols.numCells);
		}
		else {
			cells.setPrimitiveType(sf::Triangles);
			cells.resize(gols.numCells * 6);
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

	//void setupVertexBuffer(sf::VertexBuffer& vertexBuffer, int xPos, int yPos, int width, int height, const sf::Color& color) {
	//	sf::Vertex vertexArray[4];
	//	vertexArray[0].position = sf::Vector2f(xPos, yPos);
	//	vertexArray[1].position = sf::Vector2f(xPos, yPos + height);
	//	vertexArray[2].position = sf::Vector2f(xPos + width, yPos + height);
	//	vertexArray[3].position = sf::Vector2f(xPos + width, yPos);

	//	for (auto& vertex : vertexArray) {
	//		vertex.color = color;
	//	}

	//	vertexBuffer.create(4);
	//	vertexBuffer.update(vertexArray);
	//}


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
	sf::VertexArray cells; // Maybe make me a buffer?

	// GameOfLife game;
	Grid grid;

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

				break;
			}
			case sf::Event::MouseButtonReleased: {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				const int& col = mousePos.x / gols.cellDist;
				const int& row = mousePos.y / gols.cellDist;

				if (event.mouseButton.button == sf::Mouse::Left) {
					grid.clearHighlightedCells();
					bresenhamTool(firstPos.x, firstPos.y, secondPos.x, secondPos.y); 
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					grid.clearHighlightedCells();
					plotCircle(secondPos, firstPos);
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
					update();
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

	void handleMouseHover() {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		const int mouseCol = mousePos.x / gols.cellDist;
		const int mouseRow = mousePos.y / gols.cellDist;


		if (sf::Vector2i(mouseRow, mouseCol) != secondPos) { // Skips clearing and adding if mouse hasn't moved to a new cell
			secondPos = sf::Vector2i(mouseRow, mouseCol);

			grid.clearHighlightedCells();
			grid.addHighlightedCell(secondPos);
		}



	}


	void bresenhamTool(int x0, int y0, int x1, int y1) { // Draw a line between point a and point b 
		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx + dy, e2; /* error value e_xy */

		while (true) { 
			grid.updateCellTypeAt(x0, y0, CellType::STONE);
			if (x0 == x1 && y0 == y1) break;
			e2 = 2 * err;
			if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
			if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
		}
	}

	void bresenhamLineHighlighter(int x0, int y0, int x1, int y1) {
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

	void plotCircle(const sf::Vector2i& pos1, const sf::Vector2i& pos2) {
		int xm, ym;

		xm = pos2.x;
		ym = pos2.y;

		int r = dist(pos1, pos2);

		int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
		do {
			grid.updateCellTypeAt(xm - x, ym + y, CellType::STONE); /*   I. Quadrant */
			grid.updateCellTypeAt(xm - y, ym - x, CellType::STONE); /*  II. Quadrant */
			grid.updateCellTypeAt(xm + x, ym - y, CellType::STONE); /* III. Quadrant */
			grid.updateCellTypeAt(xm + y, ym + x, CellType::STONE); /*  IV. Quadrant */
			r = err;
			if (r <= y) err += ++y * 2 + 1;           /* e_xy+e_y < 0 */
			if (r > x || err > y) err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
		} while (x < 0);
	}

	void circleHighlighter(const sf::Vector2i& pos1, const sf::Vector2i& pos2) {
		int xm, ym;

		xm = pos2.x;
		ym = pos2.y;

		int r = dist(pos1, pos2);

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


	inline void update() {
		// game.gameOfLife(); // Functionality for GoL all here
	}


	inline void renderWorld() {
		window.draw(borderAndBGRect);
	}

	void calcVertices() { // Used once to place all of the vertices
		if constexpr (gols.cellDist == 1) {
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

	void vertexRenderGrid() { // Used to update colors, doesn't really do anything else

		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		const int& mouseCol = mousePos.x / gols.cellDist;
		const int& mouseRow = mousePos.y / gols.cellDist;

		if constexpr (gols.cellDist == 1) {

			int i = 0;

			for (int row = 0; row < gols.rows; ++row) {
				for (int col = 0; col < gols.cols; ++col) {

					sf::Color color = grid.getCellTypeAt(row, col).color();

					if (grid.isCellHighlighted(sf::Vector2i(row, col))) {
						color = Color::CYAN; // Highlight color
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


	inline void renderAll() {
		renderWorld();
		vertexRenderGrid();
	}

};


int main() {

	World world;

	world.mainLoop();

	return 0;
}