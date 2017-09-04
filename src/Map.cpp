#include "Map.h"
#include <BWEM/bwem.h>
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Map::Map(BlackCrow &parent) : bc(parent) {
		tileWidth = Broodwar->mapWidth();
		tileHeight = Broodwar->mapHeight();
		miniTileWidth = Broodwar->mapWidth() * 4;
		miniTileHeight = Broodwar->mapHeight() * 4;

		tiles = std::vector<std::vector<Cell>>(tileWidth, std::vector<Cell>(tileHeight));
		miniTiles = std::vector<std::vector<MiniCell>>(miniTileWidth, std::vector<MiniCell>(miniTileHeight));

		
	}

	void Map::onStart() {

		// Create Ares
		for (const BWEM::Area& bwemArea : bc.bwem.Areas()) {
			areas.emplace_back(std::make_shared<Area>(bc, bwemArea));
		}

		// Set Tile Information
		for (unsigned int x = 0; x < tileWidth; x++) {
			for (unsigned int y = 0; y < tileHeight; y++) {
				tiles[x][y].buildable = Broodwar->isBuildable(x, y, true);
				tiles[x][y].resourceBuildable = true;
				tiles[x][y].mineralLine = true;

				int areaId = bc.bwem.GetTile(TilePosition(x, y)).AreaId();
				if (areaId >= 1)
					getArea(areaId)->associatedTiles.emplace_back(TilePosition(x, y));
			}
		}

		// Set MiniTile Information
		for (unsigned int x = 0; x < miniTileWidth; x++) {
			for (unsigned int y = 0; y < miniTileHeight; y++) {
				miniTiles[x][y].walkable = Broodwar->isWalkable(WalkPosition(x, y));
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