#include "Map.h"
#include <BWEM/bwem.h>
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Map::Map(BlackCrow &parent) : bc(parent) {
		mapTiles = std::vector<std::vector<Cell>>(Broodwar->mapWidth(), std::vector<Cell>(Broodwar->mapHeight()));
	}

	void Map::onStart() {

		// Create Ares
		for (const BWEM::Area& bwemArea : bc.bwem.Areas()) {
			areas.emplace_back(std::make_shared<Area>(bc, bwemArea));
		}

		// Set Tile Information
		for (int x = 0; x < Broodwar->mapWidth(); x++) {
			for (int y = 0; y < Broodwar->mapHeight(); y++) {
				mapTiles[x][y].buildable = Broodwar->isBuildable(x, y, true);
				mapTiles[x][y].resourceBuildable = true;
				mapTiles[x][y].mineralLine = true;

				int areaId = bc.bwem.GetTile(TilePosition(x, y)).AreaId();
				if (areaId >= 1)
					getArea(areaId)->associatedTiles.emplace_back(TilePosition(x, y));
			}
		}
	}

	AreaPtr Map::getArea(int id) {
		return areas[id - 1];
	}

	AreaPtr Map::getArea(const BWEM::Area& bwemArea) {
		return areas[bwemArea.Id() - 1];
	}

	AreaPtr Map::getNearestArea(TilePosition position) {
		return getArea(*bc.bwem.GetNearestArea(position));
	}
}