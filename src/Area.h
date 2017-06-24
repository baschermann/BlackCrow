#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>
#include "EnemyUnit.h"
#include "Common.h"

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

		void addEnemyBuilding(EnemyUnitPtr enemyBuilding);
		void removeEnemyBuilding(EnemyUnitPtr enemyBuilding);

	private:
		BlackCrow &bc;
		std::vector<EnemyUnitPtr> enemyBuildings;
		std::vector<EnemyUnitPtr> enemyUnits;
	};
}