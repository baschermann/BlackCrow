#include "BuildingManager.h"

using namespace BWAPI;
using namespace Filter;

namespace { BWEM::Map* map; }

BuildingManager* buildingManager = new BuildingManager();

BuildingManager::BuildingManager() {
}


BuildingManager::~BuildingManager() {
}

void BuildingManager::onStart() {
	map = &BWEM::Map::Instance();

	for (const BWEM::Area& area : map->Areas()) {
		for (const BWEM::Base& base : area.Bases()) {

			// Block Hatchery Location
			setBuildable(base.Location().x, base.Location().x + 3, base.Location().y, base.Location().y + 2, false);
			setResourceBuildable(base.Location().x, base.Location().x + 3, base.Location().y, base.Location().y + 2, false);

			
			// Block for Minerals
			for (const BWEM::Mineral* mineral : base.Minerals()) {
				setBuildable(mineral->TopLeft().x, mineral->BottomRight().x, mineral->TopLeft().y, mineral->BottomRight().y, false);
				setResourceBuildable(mineral->TopLeft().x - 3, mineral->BottomRight().x + 3 , mineral->TopLeft().y - 3, mineral->BottomRight().y + 3, false);
			}

			// Block for Geysirs
			for (const BWEM::Geyser* geyser : base.Geysers()) {
				setBuildable(geyser->TopLeft().x, geyser->BottomRight().x, geyser->TopLeft().y, geyser->BottomRight().y, false);
				setResourceBuildable(geyser->TopLeft().x - 3, geyser->BottomRight().x + 3, geyser->TopLeft().y - 3, geyser->BottomRight().y + 3, false);
			}

			// TODO Flag Mineral Line Tiles
		}	
	}

	// Block for Neutral Buildings
	for (const auto& neutral : map->StaticBuildings()) {
		setBuildable(neutral->TopLeft().x, neutral->BottomRight().x, neutral->TopLeft().y, neutral->BottomRight().y, false);
	}
}

void BuildingManager::setBuildable(int xStart, int xEnd, int yStart, int yEnd, bool buildable) {
	for (int x = xStart; x <= xEnd; x++) {
		for (int y = yStart; y <= yEnd; y++) {
			if (x >= 0 && x < mapdata->tileWidth && y >= 0 && y < mapdata->tileHeight)
				mapdata->mapTiles[x][y].buildable = buildable;
		}
	}
}

void BuildingManager::setResourceBuildable(int xStart, int xEnd, int yStart, int yEnd, bool buildable) {
	for (int x = xStart; x <= xEnd; x++) {
		for (int y = yStart; y <= yEnd; y++) {
			if (x >= 0 && x < mapdata->tileWidth && y >= 0 && y < mapdata->tileHeight)
				mapdata->mapTiles[x][y].resourceBuildable = buildable;
		}
	}
}


BWAPI::TilePosition BuildingManager::getBuildingSpot(BWAPI::UnitType type, BWAPI::Position searchPosition, bool inMineralLine) {
	return getBuildingSpot(type, TilePosition(searchPosition.x / 32, searchPosition.y / 32), inMineralLine);
}


BWAPI::TilePosition BuildingManager::getBuildingSpot(BWAPI::UnitType type, BWAPI::TilePosition searchPosition, bool inMineralLine) {
	bool found = false;
	int searchRadius = 0;

	points.clear();
	Broodwar << points.size() << std::endl;

	/*
	// TODO, take inMineralLine into account
	while (true) {
		// start tile position
		for (int x = searchPosition.x - searchRadius; x < searchPosition.x + searchRadius; x++) {
			for (int y = searchPosition.y - searchRadius; y < searchPosition.y + searchRadius; y++) {

				points.push_back(Position(x * 32 + 16, y * 32 + 16));

				if (x < 0 || x >= mapdata->tileWidth || y < 0 || y >= mapdata->tileHeight || !mapdata->mapTiles[x][y].buildable)
					break;

				bool canBuild = true;
				// for every tile of the size of building
				for (int tilePosX = x; tilePosX < x + type.tileWidth(); tilePosX++) {
					for (int tilePosY = y; tilePosY < y + type.tileHeight(); tilePosY++) {
						
						if (tilePosX < 0 || tilePosX >= mapdata->tileWidth || tilePosY < 0 || tilePosY >= mapdata->tileHeight) {
							canBuild = false;
							break;
						}

						canBuild &= mapdata->mapTiles[tilePosX][tilePosY].buildable;
						
						if (type == UnitTypes::Zerg_Hatchery)
							canBuild &= mapdata->mapTiles[tilePosX][tilePosY].resourceBuildable;

						if (type.requiresCreep())
							canBuild &= Broodwar->hasCreep(TilePosition(tilePosY, tilePosX));
					}
				}

				if (canBuild == true)
					return TilePosition(x, y);
			}
		}
		searchRadius++;
	}
	*/

	/*
	int X = 300;
	int Y = 300;

	int x, y, dx, dy;
	x = y = dx = 0;
	dy = -1;

	int t = std::max(X, Y);
	int maxI = t*t;

	for (int i = 0; i < maxI; i++) {
		if ((-X / 2 <= x) && (x <= X / 2) && (-Y / 2 <= y) && (y <= Y / 2)){
			
			

		} 
		
		if ((x == y) || ((x < 0) && (x == -y)) || ((x > 0) && (x == 1 - y))){
			t = dx;
			dx = -dy;
			dy = t;
		}

		x += dx;
		y += dy;
	}
	*/

	
	//SpiralOut* o = new SpiralOut();
	
	/*
	for (int i = 0; i < mapdata->width * mapdata->height; i++) {
		if (canBuildTypeAt(type, o.x + searchPosition.x, o.y + searchPosition.y))
			return TilePosition(o.x + searchPosition.x, o.y + searchPosition.y);
		else
			o.goNext();
	}
	*/

	assert(false);
	return TilePosition();
}


bool BuildingManager::canBuildTypeAt(UnitType type, int x, int y) {
	points.push_back(Position(x * 32 + 16, y * 32 + 16));

	if (x < 0 || x >= mapdata->tileWidth || y < 0 || y >= mapdata->tileHeight || !mapdata->mapTiles[x][y].buildable)
		return false;

	bool canBuild = true;
	// for every tile of the size of building
	for (int tilePosX = x; tilePosX < x + type.tileWidth(); tilePosX++) {
		for (int tilePosY = y; tilePosY < y + type.tileHeight(); tilePosY++) {

			if (tilePosX < 0 || tilePosX >= mapdata->tileWidth || tilePosY < 0 || tilePosY >= mapdata->tileHeight) {
				return false;
			}

			canBuild &= mapdata->mapTiles[tilePosX][tilePosY].buildable;

			if (type == UnitTypes::Zerg_Hatchery)
				canBuild &= mapdata->mapTiles[tilePosX][tilePosY].resourceBuildable;

			if (type.requiresCreep())
				canBuild &= Broodwar->hasCreep(TilePosition(tilePosY, tilePosX));
		}
	}

	return canBuild;
}

void BuildingManager::onBuildingDetected(BWAPI::Unit building) {
	AdditionalDimensions ad = getAdditionalDimensions(building->getType());

	if (building->getType().isBuilding())
		setBuildable(building->getTilePosition().x - ad.left, building->getTilePosition().x + building->getType().tileWidth() - 1 + ad.right, building->getTilePosition().y - ad.top, building->getTilePosition().y + building->getType().tileHeight() - 1 + ad.bottom, false);
}

void BuildingManager::onBuildingDestroyed(BWAPI::Unit building) {
	AdditionalDimensions ad = getAdditionalDimensions(building->getType());

	if (building->getType().isBuilding())
		setBuildable(building->getTilePosition().x - ad.left, building->getTilePosition().x + building->getType().tileWidth() - 1 + ad.right, building->getTilePosition().y - ad.top, building->getTilePosition().y + building->getType().tileHeight() - 1 + ad.bottom, true);

	recheckNearbyBuildings(building, ad);
}

void BuildingManager::recheckNearbyBuildings(Unit building, AdditionalDimensions ad) {
	Position topLeft = Position((building->getTilePosition().x - 4) * 32, (building->getTilePosition().y - 4) * 32);
	Position bottomRight = Position((building->getTilePosition().x + building->getType().tileWidth() + 4) * 32, (building->getTilePosition().y + building->getType().tileHeight() + 4) * 32);

	for (BWAPI::Unit unit : Broodwar->getUnitsInRectangle(topLeft, bottomRight)) {
		if (unit->getType().isBuilding())
			onBuildingDetected(unit);
	}
}

BuildingManager::AdditionalDimensions BuildingManager::getAdditionalDimensions(BWAPI::UnitType buildingType) {
	AdditionalDimensions ad;

	if (buildingType == UnitTypes::Zerg_Hatchery) {
		ad.left = 1;
		ad.right = 1;
		ad.bottom = 1;
		ad.top = 1;
	}

	return ad;
}