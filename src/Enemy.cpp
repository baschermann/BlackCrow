#include "Enemy.h"
#include "EnemyUnit.h"
#include <assert.h>
#include <BWEM/bwem.h>
#include "BlackCrow.h"
#include "Map.h"

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
				newEnemy->tilePosition = unit->getTilePosition();
				setEnemyUnitArea(newEnemy, bc.map.getNearestArea(TilePosition(unit->getPosition())));
				eu = newEnemy;
			}

			eu->tilePosition = unit->getTilePosition();
			eu->position = unit->getPosition();
			updateEnemyRangeInMapTiles(eu);
		}
	}

	void Enemy::onFrame() {
		auto euIt = enemies.begin();

		while (euIt != enemies.end()) {
			EnemyUnitPtr eu = *euIt;
			Unit unit = Broodwar->getUnit(eu->id);

			if (unit->isVisible()) {
				eu->isGhost = false;

				if (unit->getType() != eu->type)
					eu->type = unit->getType();
				
				eu->position = unit->getPosition();
				if (eu->tilePosition != unit->getTilePosition()) {
					eu->tilePosition = unit->getTilePosition();
					setEnemyUnitArea(eu, bc.map.getNearestArea(eu->tilePosition));
					updateEnemyRangeInMapTiles(eu);
				}

				eu->lastSeen = Broodwar->getFrameCount();

				Broodwar->drawCircleMap(unit->getPosition(), eu->type.groundWeapon().maxRange(), Colors::Blue);

				// Handling dead extractors
				if (eu->type == UnitTypes::Resource_Vespene_Geyser)
					euIt = enemies.erase(euIt);
				else
					euIt++;
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

	/*
	void Enemy::updateEnemyRangeInMapTiles(const EnemyUnitPtr& eu, const TilePosition& current, const TilePosition& old) {
		if (eu->type.canAttack()) {
			int maxRange = eu->type.groundWeapon().maxRange();
			int tileRadius = (int)std::ceil(maxRange / 32);

			// Remove old
			if (old != BWAPI::TilePositions::Invalid) {
				for (int x = old.x - tileRadius; x < old.x + tileRadius; x++) {
					for (int y = old.y - tileRadius; y < old.y + tileRadius; y++) {
						double radius = Util::distance(x, y, old.x, old.y);
						if (radius <= maxRange) {
							auto& enemies = bc.map.tiles[x][y].enemiesInRange;
							enemies.erase(std::remove(enemies.begin(), enemies.end(), eu), enemies.end());
						}
					}
				}
			}

			// Add new
			for (int x = current.x - tileRadius; x < current.x + tileRadius; x++) {
				for (int y = current.y - tileRadius; y < current.y + tileRadius; y++) {
					double radius = Util::distance(x, y, current.x, current.y);
					if (radius <= maxRange) {
						bc.map.tiles[x][y].enemiesInRange.push_back(eu);
					}
				}
			}
		}
	}
	*/

	void Enemy::updateEnemyRangeInMapTiles(const EnemyUnitPtr& eu) {
		//updateEnemyRangeInMapTiles(eu, current, BWAPI::TilePositions::Invalid);
		if (eu->type.canAttack()) {
			int maxRange = eu->type.groundWeapon().maxRange();
			int tileRadius = (int)std::ceil(maxRange / 32);

			// Remove old
			for (auto& pos : eu->weaponRangeTiles) {
				//bc.map.tiles[pos.first][pos.second].enemiesInRange.erase
				auto& enemies = bc.map.tiles[pos.first][pos.second].enemiesInRange;
				enemies.erase(std::remove(enemies.begin(), enemies.end(), eu), enemies.end());
			}

			// Add new
			for (int x = eu->tilePosition.x - tileRadius - 1; x <= eu->tilePosition.x + tileRadius + 1; x++) {
				for (int y = eu->tilePosition.y - tileRadius - 1; y <= eu->tilePosition.y + tileRadius + 1; y++) {
					double radius = Util::distance(x*32 + 16, y*32 + 16, eu->position.x, eu->position.y);
					if (radius <= maxRange && x >= 0 && x < (int)bc.map.tileWidth && y >= 0 && y < (int)bc.map.tileHeight) {
						bc.map.tiles[x][y].enemiesInRange.push_back(eu);
						eu->weaponRangeTiles.push_back(PairUint(x, y));
					}
				}
			}
		}
	}
}