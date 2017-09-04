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
		};

		std::vector<std::vector<Cell>> tiles;
		std::vector<std::vector<MiniCell>> miniTiles;
		std::vector<AreaPtr> areas;

		unsigned int tileWidth = 0;
		unsigned int tileHeight = 0;
		unsigned int miniTileWidth = 0;
		unsigned int miniTileHeight = 0;

		Map(BlackCrow &parent);
		void onStart();
		AreaPtr getArea(int id);
		AreaPtr getArea(const BWEM::Area& bwemArea);
		AreaPtr getNearestArea(BWAPI::TilePosition position);

	private:
		BlackCrow &bc;
	};
}