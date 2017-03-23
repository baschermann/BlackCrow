#include "BlackCrow.h"
#include "Base.h"
#include "Area.h"


namespace BlackCrow {

	
	Base::Base(BlackCrow& parent, BWEM::Base& bwemBase, Area& area) : bc(parent), bwemBase(bwemBase), area(area) {

	}

	void Base::addWorker(Worker worker) {

	}

	Worker Base::removeWorker() {
		// TODO Take Mineral into account

	}

	Worker Base::removeWorker(BWAPI::Position closestTo) {

	}

	bool Base::workerNeeded() {

	}

	int Base::workersNeeded() {

	}

	Mineral& Base::findMineralForWorker() {

	}
}