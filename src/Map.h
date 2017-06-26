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

		std::vector<std::vector<Cell>> mapTiles;
		std::vector<AreaPtr> areas;

		Map(BlackCrow &parent);
		void onStart();
		AreaPtr getArea(int id);
		AreaPtr getArea(const BWEM::Area& bwemArea);
		AreaPtr getNearestArea(BWAPI::TilePosition position);

	private:
		BlackCrow &bc;
	};
}