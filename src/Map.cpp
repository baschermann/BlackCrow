#include "Map.h"
#include <BWEM/bwem.h>
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Map::Map(BlackCrow &parent) : bc(parent) {
		tileWidth = Broodwar->mapWidth();
		tileHeight = Broodwar->mapHeight();

		mapTiles = std::vector<std::vector<Cell>>(tileWidth, std::vector<Cell>(tileHeight));
	}

	void Map::onStart() {

		// Create Ares
		for (const BWEM::Area& bwemArea : bc.bwem.Areas()) {
			areas.emplace_back(Area(bc, bwemArea));
		}

		// Set Tile Information
		for (int x = 0; x < tileWidth; x++) {
			for (int y = 0; y < tileHeight; y++) {
				mapTiles[x][y].buildable = Broodwar->isBuildable(x, y, true);
				mapTiles[x][y].resourceBuildable = true;
				mapTiles[x][y].mineralLine = true;

				// Let the areas know which tiles they contain
				// ??? Is this more effective to use its own loop to not screw with the cache of mapTiles?
				int areaId = bc.bwem.GetTile(TilePosition(x, y)).AreaId();
				if (areaId >= 1)
					getArea(areaId).associatedTiles.emplace_back(TilePosition(x, y));
			}
		}
	}

	Area& Map::getArea(int id) {
		return areas[id - 1];
	}

	Area& Map::getArea(const BWEM::Area bwemArea) {
		return areas[bwemArea.Id() - 1];
	}
}