#pragma once
#include <BWAPI.h>
#include "Common.h"

namespace BlackCrow {

	class BlackCrow;
	class EnemyUnit;

	class Enemy {
	public:
		Enemy(BlackCrow &parent);

		void onStart();
		void onFrame();
		std::list<EnemyUnitPtr> enemies;
		void enemyDiscovered(BWAPI::Unit unit);
		void onUnitDestroyed(BWAPI::Unit unit);

		EnemyUnitPtr getEnemy(int id);
		bool hasKnownBuilding();

		// Can return nullptr
		// Predicate for whether to include the unit or not.
		// bool pred(const EnemyUnitPtr enemyUnit)
		template<class UnaryPredicate>
		EnemyUnitPtr getClosestEnemy(BWAPI::Position closestTo, UnaryPredicate condition) {
			EnemyUnitPtr closestEnemy = nullptr;
			double closestDistance = std::numeric_limits<double>().max();

			for (EnemyUnitPtr enemyUnit : enemies) {
				if (condition(enemyUnit)) {
					double distance = Util::distance(closestTo, enemyUnit->position);

					if (distance < closestDistance) {
						closestEnemy = enemyUnit;
						closestDistance = distance;
					}
				}
			}

			return closestEnemy;
		}

	private:
		BlackCrow &bc;

		double ghostTime(BWAPI::UnitType type);
		void setEnemyUnitArea(const EnemyUnitPtr& eu, const AreaPtr& area);
		void removeEnemyUnitFromArea(const EnemyUnitPtr& eu, const AreaPtr& area);
		void updateEnemyRangeInMapTiles(const EnemyUnitPtr& eu, const BWAPI::Unit u, const BWAPI::TilePosition& current);
		void updateEnemyRangeInMapTiles(const EnemyUnitPtr& eu, const BWAPI::Unit u, const BWAPI::TilePosition& current, const BWAPI::TilePosition& old);
	};

}