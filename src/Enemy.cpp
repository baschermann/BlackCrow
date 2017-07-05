#include "Enemy.h"
#include "EnemyUnit.h"
#include <assert.h>
#include <BWEM/bwem.h>
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Enemy::Enemy(BlackCrow &parent) : bc(parent) {}

	void Enemy::onStart() {}

	void Enemy::enemyDiscovered(BWAPI::Unit unit) {
		if (Broodwar->self()->isEnemy(unit->getPlayer())) {
			EnemyUnitPtr eu = getEnemy(unit->getID());

			if (!eu) {
				enemies.emplace_back(std::make_shared<EnemyUnit>());
				EnemyUnitPtr newEnemy = enemies.back();

				newEnemy->id = unit->getID();
				newEnemy->isVisible = true;
				newEnemy->lastSeen = Broodwar->getFrameCount();
				newEnemy->type = unit->getType();
				newEnemy->tilePosition = BWAPI::TilePositions::Invalid;
				setEnemyUnitArea(newEnemy, bc.map.getNearestArea(TilePosition(unit->getPosition())));

				eu = newEnemy;
			}

			if (eu->type.isBuilding()) {
				if (eu->tilePosition.x != unit->getTilePosition().x || eu->tilePosition.y != unit->getTilePosition().y) {
					eu->tilePosition = unit->getTilePosition();
					assert(eu->tilePosition != BWAPI::TilePositions::Invalid);
					setEnemyUnitArea(eu, bc.map.getNearestArea(eu->tilePosition));
				}
			} else {
				if (eu->position != unit->getPosition()) {
					eu->position = unit->getPosition();
					setEnemyUnitArea(eu, bc.map.getNearestArea(TilePosition(unit->getPosition())));
				}
			}
		}
	}

	void Enemy::onFrame() {
		auto euIt = enemies.begin();

		while (euIt != enemies.end()) {
			EnemyUnitPtr eu = *euIt;
			Unit unit = Broodwar->getUnit(eu->id);

			if (unit->isVisible()) {
				eu->isGhost = false;

				if (eu->type.isBuilding() && (unit->getTilePosition().x != eu->tilePosition.x || unit->getTilePosition().y != eu->tilePosition.y)) {
					eu->tilePosition.x = unit->getTilePosition().x;
					eu->tilePosition.y = unit->getTilePosition().y;
					setEnemyUnitArea(eu, bc.map.getNearestArea(eu->tilePosition));
				} else {
					eu->position.x = unit->getPosition().x;
					eu->position.y = unit->getPosition().y;
					setEnemyUnitArea(eu, bc.map.getNearestArea(TilePosition(unit->getPosition())));
				}

				if (unit->getType() != eu->type) {
					eu->type = unit->getType();
				}

				eu->lastSeen = Broodwar->getFrameCount();

				// Handling dead extractors
				if (eu->type == UnitTypes::Resource_Vespene_Geyser) {
					//Broodwar->sendText("Dead Geyser detected. Deleting that thing");
					euIt = enemies.erase(euIt);
				} else {
					euIt++;
				}
			} else {
				eu->isVisible = false;

				// Handling a ghost
				if (Broodwar->isVisible(TilePosition(eu->position))) {
					eu->isGhost = true;
				} else {
					if (!eu->type.isBuilding() && eu->lastSeen + ghostTime(eu->type) <= Broodwar->getFrameCount()) {
						eu->isGhost = true;
					}
				}

				euIt++;
			}
		}
	}

	void Enemy::onUnitDestroyed(BWAPI::Unit unit) {
		auto enemyUnitIt = enemies.begin();
		while (enemyUnitIt != enemies.end()) {
			if ((*enemyUnitIt)->id == unit->getID()) {
				EnemyUnitPtr& eu = *enemyUnitIt;
				eu->isDestroyed = true;
				removeEnemyUnitFromArea(eu, eu->area);
				enemies.erase(enemyUnitIt);
				return;
			} else
				enemyUnitIt++;
		}
	}

	EnemyUnitPtr Enemy::getEnemy(int id) {
		for (EnemyUnitPtr enemyUnit : enemies) {
			if (enemyUnit->id == id)
				return enemyUnit;
		}
		return nullptr;
	}

	double Enemy::ghostTime(BWAPI::UnitType type) {
		return 1700 / type.topSpeed();
	}

	bool Enemy::hasKnownBuilding() {
		auto buildingIt = std::find_if(enemies.begin(), enemies.end(), [](EnemyUnitPtr enemyUnit) { return enemyUnit->type.isBuilding(); });
		if (buildingIt != enemies.end())
			return true;
		return false;
	}

	void Enemy::setEnemyUnitArea(const EnemyUnitPtr& eu, const AreaPtr& area) {
		removeEnemyUnitFromArea(eu, eu->area);
		eu->area = area;
		area->enemies.push_back(eu);
	}

	void Enemy::removeEnemyUnitFromArea(const EnemyUnitPtr& eu, const AreaPtr& area) {
		if (eu->area)
			area->enemies.erase(std::remove(area->enemies.begin(), area->enemies.end(), eu), area->enemies.end());
	}
}