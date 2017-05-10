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

	private:
		BlackCrow &bc;

		EnemyUnit* findEnemy(int id);
	};

}