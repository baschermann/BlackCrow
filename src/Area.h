#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>

namespace BlackCrow {

	class BlackCrow;
	class EnemyUnit;
	class BWEM::Area;

	class Area {
	public:
		Area(BlackCrow &parent);
		~Area();


		const BWEM::Area* area;
		void addEnemyBuilding(EnemyUnit* enemyBuilding);
		void removeEnemyBuilding(EnemyUnit* enemyBuilding);
		bool isOccupied();

	private:
		BlackCrow &bc;

		bool occupied;
		std::list<EnemyUnit*>* enemyBuildings;
	};
}