#include <BWAPI.h>
#include "Mapdata.h"

using namespace BWAPI;
using namespace Filter;

namespace { BWEM::Map* map; }

Mapdata* mapdata = new Mapdata();

Mapdata::Mapdata() {}

void Mapdata::onStart() {

	map = &BWEM::Map::Instance();

	// Map Tiles
	tileWidth = Broodwar->mapWidth();
	tileHeight = Broodwar->mapHeight();

	mapTiles = std::vector<std::vector<Cell>>(tileWidth);
	for (int i = 0; i < tileWidth; i++) {
		mapTiles[i] = std::vector<Cell>(tileHeight);
	}

	for (int x = 0; x < tileWidth; x++) {
		for (int y = 0; y < tileHeight; y++) {
			mapTiles[x][y] = Cell();
			mapTiles[x][y].buildable = Broodwar->isBuildable(x, y, true);
			mapTiles[x][y].resourceBuildable = true;
		}
	}

	width = tileWidth * TILE_SIZE;
	height = tileHeight * TILE_SIZE;
}