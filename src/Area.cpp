#include "Area.h"
#include <algorithm>
#include "BlackCrow.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Area::Area(BlackCrow &parent, const BWEM::Area& bwemArea) : bc(parent), bwemArea(bwemArea) {}

	bool operator==(const Area& left, const Area& right) {
		return left.bwemArea.Id() == right.bwemArea.Id();
	}
}