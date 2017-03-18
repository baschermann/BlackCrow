#include "Builder.h"
#include <BWEM/bwem.h>
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Builder::Builder(BlackCrow &parent) : bc(parent) {}

	Builder::~Builder() {}

	void Builder::onStart() {

		for (const BWEM::Area& area : bc.bwem.Areas()) {
			for (const BWEM::Base& base : area.Bases()) {

				// Block Hatchery Location
				setBuildable(base.Location().x, base.Location().x + 3, base.Location().y, base.Location().y + 2, false);
				setResourceBuildable(base.Location().x, base.Location().x + 3, base.Location().y, base.Location().y + 2, false);


				// Block for Minerals
				for (const BWEM::Mineral* mineral : base.Minerals()) {
					setBuildable(mineral->TopLeft().x, mineral->BottomRight().x, mineral->TopLeft().y, mineral->BottomRight().y, false);
					setResourceBuildable(mineral->TopLeft().x - 3, mineral->BottomRight().x + 3, mineral->TopLeft().y - 3, mineral->BottomRight().y + 3, false);
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
		for (const auto& neutral : bc.bwem.StaticBuildings()) {
			setBuildable(neutral->TopLeft().x, neutral->BottomRight().x, neutral->TopLeft().y, neutral->BottomRight().y, false);
		}
	}

	void Builder::setBuildable(int xStart, int xEnd, int yStart, int yEnd, bool buildable) {
		for (int x = xStart; x <= xEnd; x++) {
			for (int y = yStart; y <= yEnd; y++) {
				if (x >= 0 && x < bc.map.tileWidth && y >= 0 && y < bc.map.tileHeight)
					bc.map.mapTiles[x][y].buildable = buildable;
			}
		}
	}

	void Builder::setResourceBuildable(int xStart, int xEnd, int yStart, int yEnd, bool buildable) {
		for (int x = xStart; x <= xEnd; x++) {
			for (int y = yStart; y <= yEnd; y++) {
				if (x >= 0 && x < bc.map.tileWidth && y >= 0 && y < bc.map.tileHeight)
					bc.map.mapTiles[x][y].resourceBuildable = buildable;
			}
		}
	}


	BWAPI::TilePosition Builder::getBuildingSpot(BWAPI::UnitType type, BWAPI::Position searchPosition, bool inMineralLine) {
		return getBuildingSpot(type, TilePosition(searchPosition.x / 32, searchPosition.y / 32), inMineralLine);
	}

	BWAPI::TilePosition Builder::getBuildingSpot(BWAPI::UnitType type, BWAPI::TilePosition searchPosition, bool inMineralLine) {
		bool found = false;
		int searchRadius = 0;

		Broodwar->sendText("Looking around %i, %i", searchPosition.x, searchPosition.y);

		Util::SpiralOut spiral;

		positions.clear();

		for (int i = 0; i < bc.map.width * bc.map.height; i++) {
			if (canBuildTypeAt(type, spiral.x + searchPosition.x, spiral.y + searchPosition.y)) {
				Broodwar->sendText("Building Location found at %i, %i", spiral.x + searchPosition.x, spiral.y + searchPosition.y);
				return TilePosition(spiral.x + searchPosition.x, spiral.y + searchPosition.y);
			}
			else
				spiral.goNext();
		}
		
		assert(false);
		return TilePosition();
	}

	bool Builder::canBuildTypeAt(UnitType type, int x, int y) {
		if (x < 0 || x >= bc.map.tileWidth || y < 0 || y >= bc.map.tileHeight || !bc.map.mapTiles[x][y].buildable)
			return false;

		positions.push_back(TilePosition(x, y));

		bool canBuild = true;
		// for every tile of the size of building
		for (int tilePosX = x; tilePosX < x + type.tileWidth(); tilePosX++) {
			for (int tilePosY = y; tilePosY < y + type.tileHeight(); tilePosY++) {

				if (tilePosX < 0 || tilePosX >= bc.map.tileWidth || tilePosY < 0 || tilePosY >= bc.map.tileHeight) {
					return false;
				}

				canBuild &= bc.map.mapTiles[tilePosX][tilePosY].buildable;

				if (type == UnitTypes::Zerg_Hatchery)
					canBuild &= bc.map.mapTiles[tilePosX][tilePosY].resourceBuildable;

				if (type.requiresCreep())
					canBuild &= Broodwar->hasCreep(TilePosition(tilePosX, tilePosY));
			}
		}

		return canBuild;
	}

	void Builder::onBuildingDetected(BWAPI::Unit building) {
		AdditionalDimensions ad = getAdditionalDimensions(building->getType());

		if (building->getType().isBuilding())
			setBuildable(building->getTilePosition().x - ad.left, building->getTilePosition().x + building->getType().tileWidth() - 1 + ad.right, building->getTilePosition().y - ad.top, building->getTilePosition().y + building->getType().tileHeight() - 1 + ad.bottom, false);
	}

	void Builder::onBuildingDestroyed(BWAPI::Unit building) {
		AdditionalDimensions ad = getAdditionalDimensions(building->getType());

		if (building->getType().isBuilding())
			setBuildable(building->getTilePosition().x - ad.left, building->getTilePosition().x + building->getType().tileWidth() - 1 + ad.right, building->getTilePosition().y - ad.top, building->getTilePosition().y + building->getType().tileHeight() - 1 + ad.bottom, true);

		recheckNearbyBuildings(building, ad);
	}

	void Builder::recheckNearbyBuildings(Unit building, AdditionalDimensions ad) {
		Position topLeft = Position((building->getTilePosition().x - 4) * 32, (building->getTilePosition().y - 4) * 32);
		Position bottomRight = Position((building->getTilePosition().x + building->getType().tileWidth() + 4) * 32, (building->getTilePosition().y + building->getType().tileHeight() + 4) * 32);

		for (BWAPI::Unit unit : Broodwar->getUnitsInRectangle(topLeft, bottomRight)) {
			if (unit->getType().isBuilding())
				onBuildingDetected(unit);
		}
	}

	Builder::AdditionalDimensions Builder::getAdditionalDimensions(BWAPI::UnitType buildingType) {
		AdditionalDimensions ad;

		if (buildingType == UnitTypes::Zerg_Hatchery) {
			ad.left = 1;
			ad.right = 1;
			ad.bottom = 1;
			ad.top = 1;
		}

		return ad;
	}
}