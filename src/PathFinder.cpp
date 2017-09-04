#include "PathFinder.h"
#include "BlackCrow.h"
#include "AStar.h"

namespace BlackCrow {

	using namespace BWAPI;

	PathFinder::PathFinder(BlackCrow& blackCrow) : bc(blackCrow) {}

	PathResultUptr PathFinder::findPath(PairUint start, PairUint end) {
		AStar astar(bc, start, end);
		astar.startSearching();
		Broodwar->sendText("Analyzed nodes: %i", astar.analyzedNodes);

		auto pathResult = std::make_unique<PathResult>(PathResult());
		astar.fillResultIn(pathResult->path);
		pathResult->hasPath = pathResult->path.size() > 0 ? true : false;

		return pathResult;
	}
}