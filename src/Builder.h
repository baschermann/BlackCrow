#pragma once
#include <BWAPI.h>

namespace BlackCrow {

	class BlackCrow;
	class Base;

	class Builder {
	public:
		Builder(BlackCrow &crow);

		struct AdditionalDimensions {
			int left = 0;
			int right = 0;
			int top = 0;
			int bottom = 0;
		};

		std::list<BWAPI::TilePosition> positions;

		void onStart();
		BWAPI::TilePosition getBuildingSpot(BWAPI::UnitType type, bool inMineralLine);
		BWAPI::TilePosition getBuildingSpot(BWAPI::UnitType type, Base& base, bool inMineralLine);
		BWAPI::TilePosition getBuildingSpot(BWAPI::UnitType type, BWAPI::Position searchPosition, bool inMineralLine);
		BWAPI::TilePosition getBuildingSpot(BWAPI::UnitType type, BWAPI::TilePosition searchPosition, bool inMineralLine);
		void onBuildingDetected(BWAPI::Unit unit);
		void onBuildingDestroyed(BWAPI::Unit unit);
		bool canBuildTypeAt(BWAPI::UnitType type, int x, int y, bool inMineralLine);
		void Builder::setMineralLine(BWAPI::Position base, BWAPI::Position resource, int xStart, int xEnd, int yStart, int yEnd, bool buildable);

	private:
		BlackCrow &bc;

		void setBuildable(int xStart, int xEnd, int yStart, int yEnd, bool buildable);
		void setResourceBuildable(int xStart, int xEnd, int yStart, int yEnd, bool buildable);
		void recheckNearbyBuildings(BWAPI::Unit building, AdditionalDimensions ad);
		AdditionalDimensions getAdditionalDimensions(BWAPI::UnitType buildingType);
	};
}