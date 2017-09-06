#include "PathFinder.h"
#include "BlackCrow.h"
#include "AStar.h"

namespace BlackCrow {

	using namespace BWAPI;

	PathFinder::PathFinder(BlackCrow& blackCrow) : bc(blackCrow) {}

	PathResultUptr PathFinder::findPath(PairUint start, PairUint end) {

		auto pathResult = std::make_unique<PathResult>(PathResult());

		AStar astar(bc, start, end);
		pathResult->path = astar.startSearching();
		Broodwar->sendText("Analyzed nodes: %i", astar.analyzedNodes);

		//astar.fillResultIn(pathResult->path);
		pathResult->hasPath = pathResult->path.size() > 0 ? true : false;

		return pathResult;
	}
}