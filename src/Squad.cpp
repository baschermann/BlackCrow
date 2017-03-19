#include "Squad.h"
#include "SquadUnit.h"
#include "BWEM/bwem.h"
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Squad::Squad(BlackCrow &parent) : bc(parent) {}

	Squad::~Squad() {}

	void Squad::onFrame() {
	}

	void Squad::add(SquadUnit* unit) {
		units.insert(unit);
	}

	void Squad::remove(SquadUnit* unit) {
		units.erase(unit);
	}

	void Squad::moveAll(BWAPI::Position position, bool queue) {
		for (SquadUnit* unit : units) {
			unit->move(position, queue);
		}
	}

	// ######## Scout Squad #########
	ScoutSquad::ScoutSquad(BlackCrow &parent) : Squad(parent) {
		scoutLocations = new std::list<BWAPI::TilePosition>();
	}

	ScoutSquad::~ScoutSquad() {
		delete scoutLocations;
	}

	void ScoutSquad::onFrame() {
		Squad::onFrame();

		while (scoutLocations->size() > 0 && Broodwar->isVisible(scoutLocations->front()))
			scoutLocations->pop_front(); // FIXME: Kruecke: Smells like memory leak. Or is there a "delete" anywhere?

		if (scoutLocations->size() > 0) {
			BWAPI::TilePosition tilePosition = scoutLocations->front();
			moveAll(BWAPI::Position(tilePosition.x * 32, tilePosition.y * 32), false); // Kruecke: This will spam hard!
		}
		// TODO: Kruecke: else? Maybe remove scout squad or something.
	}

	bool ScoutSquad::isStillScouting() {
		return scoutLocations->size() > 0;
	}

	int ScoutSquad::locationsToScout() {
		return scoutLocations->size();
	}

	std::list<BWAPI::TilePosition>* ScoutSquad::getScoutingPositions() {
		return scoutLocations;
	}

	void ScoutSquad::addScoutPosition(BWAPI::TilePosition tilePosition) {
		if (scoutLocations->size() <= 0)
			moveAll(BWAPI::Position(tilePosition.x * 32, tilePosition.y * 32), false);

		scoutLocations->push_back(tilePosition);
	}

	void ScoutSquad::addStartLocations() {
		// Kruecke: No, you do not change the starting positions here.
		// 1) In each loop iteration "tilePosition" is a _copy_ as there is no reference ("&") in the type. It is safe to manipulate that copy.
		// 2) StartingLocations() returns a _const_ vector. Any attempt to change its values will result in a compiler error.
		//    Try: bc.bwem.StartingLocations().front() = TilePosition(123, 234); // Attempt to change first value in vector.
		for (BWAPI::TilePosition tilePosition : bc.bwem.StartingLocations()) {
			if (!Broodwar->isVisible(tilePosition)) {
				tilePosition.x += 1; // TODO Do I change the starting position?!?
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