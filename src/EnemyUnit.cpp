#include "EnemyUnit.h"

namespace BlackCrow {

	EnemyUnit::EnemyUnit() {
		int lastSeen = 0;
		bool isVisible = true;
	}

	bool operator==(const EnemyUnit& left, const EnemyUnit& right) {
		return left.id == right.id;
	}
}