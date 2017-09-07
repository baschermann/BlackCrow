#pragma once
#include <BWAPI.h>
#include "Common.h"


namespace BlackCrow {

	class BlackCrow;

	struct PathResult {
		bool hasPath = false;
		std::vector<PairUint> path;
		std::vector<PairUint> visitedNodes;
	};

	class PathFinder {
	public:
		PathFinder(BlackCrow& blackCrow);

		PathResultUptr findPath(PairUint start, PairUint end);
	private:
		BlackCrow &bc;
	};
}