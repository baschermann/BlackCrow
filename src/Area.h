#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>
#include "EnemyUnit.h"

namespace BlackCrow {

	class BlackCrow;
	class EnemyUnit;
	class BWEM::Area;

	class Area {
	public:
		Area(BlackCrow &parent, const BWEM::Area& bwemArea);
		const BWEM::Area& bwemArea;

		bool isEnemyOwned;
		std::vector<BWAPI::TilePosition> associatedTiles;

		void addEnemyBuilding(const EnemyUnit& enemyBuilding);
		void removeEnemyBuilding(const EnemyUnit& enemyBuilding);

	private:
		BlackCrow &bc;
		std::vector<EnemyUnit> enemyBuildings;
	};
}