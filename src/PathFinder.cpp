#include "PathFinder.h"
#include "BlackCrow.h"
#include "AStar.h"

namespace BlackCrow {

	using namespace BWAPI;

	PathFinder::PathFinder(BlackCrow& blackCrow) : bc(blackCrow) {}

	PathResultUptr PathFinder::findPath(PairUint start, PairUint end) {

		auto pathResult = std::make_unique<PathResult>(PathResult());

		AStar astar(bc, start, end);

		auto startTime = std::chrono::high_resolution_clock::now();
		pathResult->path = astar.startSearching();
		auto endTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> diff = endTime - startTime;

		Broodwar->sendText("Looked at: %i, added: %i, analyzed: %i, time (ms): %f", astar.lookedAtNodes, astar.addedNodes, astar.analyzedNodes, diff.count());

		pathResult->hasPath = pathResult->path.size() > 0 ? true : false;

		
		for (auto it = astar.closed.begin(); it != astar.closed.end(); ++it) {
			pathResult->visitedNodes.push_back(it->first);
		}

		return pathResult;
	}
}