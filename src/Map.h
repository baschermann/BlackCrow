#pragma once
#include <BWAPI.h>
#include <vector>

namespace BlackCrow {

	class BlackCrow;

	// Draws debug information
	class Map {

	public:
		struct Cell {
			bool buildable;
			bool resourceBuildable;
			bool mineralLine;
		};

		struct MiniCell {
			bool walkable;
			bool chokepoint;
		};

		int tileWidth; // Tiles TILE_SIZE or 32 pixels per tile
		int tileHeight;
		int width;
		int height;
		std::vector<std::vector<Cell>> mapTiles;

		Map(BlackCrow &parent);
		~Map();
		void onStart();

	private:
		BlackCrow &bc;

	};
}