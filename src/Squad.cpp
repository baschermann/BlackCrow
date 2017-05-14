#include "Squad.h"
#include "SquadUnit.h"
#include "BWEM/bwem.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Squad::Squad(BlackCrow &parent) : bc(parent) {}

	void Squad::onFrame() {}

	void Squad::add(SquadUnitPtr unit) {
		units.push_back(unit);
	}

	void Squad::remove(SquadUnitPtr unit) {
		units.push_back(unit);
	}

	void Squad::moveAll(BWAPI::Position position, bool queue) {
		for (SquadUnitPtr unit : units) {
			unit->move(position, queue);
		}
	}

	// ######## Scout Squad #########
	ScoutSquad::ScoutSquad(BlackCrow &parent) : Squad(parent) {}

	void ScoutSquad::onFrame() {
		Squad::onFrame();

		while (scoutLocations.size() > 0 && Broodwar->isVisible(scoutLocations.back()))
			scoutLocations.pop_back();

		if (scoutLocations.size() > 0) {
			moveAll(BWAPI::Position(scoutLocations.back()), false); // Kruecke: This will spam hard! - TODO Not good then!
		}
	}

	bool ScoutSquad::isStillScouting() {
		return scoutLocations.size() > 0;
	}

	int ScoutSquad::locationsToScout() {
		return scoutLocations.size();
	}

	std::vector<BWAPI::TilePosition> ScoutSquad::getScoutingPositions() {
		return scoutLocations;
	}

	void ScoutSquad::addScoutPosition(BWAPI::TilePosition tilePosition) {
		scoutLocations.push_back(tilePosition);
	}

	void ScoutSquad::addStartLocations() {
		for (BWAPI::TilePosition tilePosition : bc.bwem.StartingLocations()) {
			if (!Broodwar->isVisible(tilePosition)) {
				tilePosition.x += 1;
				tilePosition.y += 1;
				addScoutPosition(tilePosition);
			}
		}
	}

	void ScoutSquad::addExpansions() {
		for (BWEM::Area area : bc.bwem.Areas()) {
			for (BWEM::Base base : area.Bases()) {
				if (!Broodwar->isVisible(base.Location())) {
					addScoutPosition(base.Location());
				}
			}
		}
	}

	void ScoutSquad::setGlobalSearch(bool searchGlobally) {
		globalSearch = searchGlobally;
	}
}