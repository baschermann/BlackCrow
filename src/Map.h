#pragma once
#include <BWAPI.h>
#include <vector>
#include "Area.h"

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
		std::vector<std::vector<Cell>> mapTiles;
		std::vector<Area> areas;

		Map(BlackCrow &parent);
		void onStart();
		Area& getArea(int id);
		Area& getArea(const BWEM::Area bwemArea);

	private:
		BlackCrow &bc;
	};
}