#include "World.h"


class TypeSelection {

	// Should make a GUI when the appropriate button is pressed for selecting cell type used for the tools
	// For example, pressing middle click brings up a menu for choosing celltype or tooltype... 


};




int main() {

	World world;

	world.mainLoop();

	return 0;
}

// Sandy Rendering
// 
// GOALS
// 
// Split into multiple files (split up world into eventhandler, world, and renderer first)
// Add GUI for tools and types
// 
// Address potential future bottlenecks
// --> Make the cells vertex array a vertex buffer
// --> The current implementation redraws the entire grid every frame, which can be optimized by only updating the vertices of cells that changed state.
// --> Make grid store cells linearly (only one vector) (way later, figure out cell update stuff first)
// 
// Clean up/ refactor code to make more modular/extendable --> Move on from this for now except for items below
// --> Break into more smaller classes (maybe?) --> e.g. event handling separately?
// --> Add error checking/ reporting
// 
// Add gui stuff: debug info, tool selection, etc...
//
// Look for memory leaks