#pragma once
#include <BWAPI.h>

namespace BlackCrow {

	class BlackCrow;
	class EnemyUnit;

	class Enemy {
	public:
		Enemy(BlackCrow &parent);

		void onStart();
		void onFrame();
		std::list<EnemyUnit> enemies;
		void enemyDiscovered(BWAPI::Unit unit);
		void enemyDestroyed(BWAPI::Unit unit);

		// Predicate for whether to include the unit or not
		// bool pred(const EnemyUnit& enemyUnit)
		template<class UnaryPredicate>
		EnemyUnit* getClosestEnemy(BWAPI::Position closestTo, UnaryPredicate condition) {
			EnemyUnit* closestEnemy = nullptr;
			double closestDistance = std::numeric_limits<double>().max();

			for (EnemyUnit& enemyUnit : enemies) {
				if (condition(enemyUnit)) {
					double distance = Util::distance(closestTo, enemyUnit.position);

					if (distance < closestDistance) {
						closestEnemy = &enemyUnit;
						closestDistance = distance;
					}
				}
			}

			return closestEnemy;
		}

	private:
		BlackCrow &bc;

		EnemyUnit* findEnemy(int id);
	};

}