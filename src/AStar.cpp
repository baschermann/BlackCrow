#include "AStar.h"
#include "BlackCrow.h"
#include "Util.h"
#include "fiboqueue.h"

namespace BlackCrow {

	using namespace BWAPI;

	AStar::AStar(BlackCrow& blackCrow, PairUint start, PairUint end) : bc(blackCrow), start(start), end(end) {};

	std::vector<PairUint> AStar::startSearching() {
		open.emplace(Node(start));

		do {
			Node const currentNode = open.top();
			open.pop();
			
			closed.emplace(currentNode.pos, currentNode);
			analyzedNodes++;

			if (currentNode.pos == end) {
				std::vector<PairUint> result;

				Node node = currentNode;
				do {
					result.push_back(node.pos);
					auto nodeIt = closed.find(node.prev);
					node = nodeIt->second;
				} while (node.hasPrev);

				std::reverse(std::begin(result), std::end(result));
				return result;
			}

			// Expand neighbour nodes
			// Top Row
			expandNode(Node(PairUint(currentNode.pos.first - 1, currentNode.pos.second - 1)), 1.41421356237f, currentNode, end);
			expandNode(Node(PairUint(currentNode.pos.first, currentNode.pos.second - 1)), 1, currentNode, end);
			expandNode(Node(PairUint(currentNode.pos.first + 1, currentNode.pos.second - 1)), 1.41421356237f, currentNode, end);
			// Middle Row
			expandNode(Node(PairUint(currentNode.pos.first - 1, currentNode.pos.second)), 1, currentNode, end);
			expandNode(Node(PairUint(currentNode.pos.first + 1, currentNode.pos.second)), 1, currentNode, end);
			// Bottom Row
			expandNode(Node(PairUint(currentNode.pos.first - 1, currentNode.pos.second + 1)), 1.41421356237f, currentNode, end);
			expandNode(Node(PairUint(currentNode.pos.first, currentNode.pos.second + 1)), 1, currentNode, end);
			expandNode(Node(PairUint(currentNode.pos.first + 1, currentNode.pos.second + 1)), 1.41421356237f, currentNode, end);
		} while (!open.empty());
		
		// No Path found
		return {};
	}

	void AStar::expandNode(Node& successor, const float distanceCost, const Node& currentNode, const PairUint& end) {

		lookedAtNodes++;
		
		if (closed.count(successor.pos) != 0
			|| successor.pos.first < 0
			|| successor.pos.first >= bc.map.miniTileWidth
			|| successor.pos.second < 0
			|| successor.pos.second >= bc.map.miniTileHeight
			|| !bc.map.miniTiles[successor.pos.first][successor.pos.second].walkable
			) {
			return; // Skip this node
		}

		const auto totalCost = currentNode.total + distanceCost;
		const auto it = open.find(successor);

		if (it != open.end() && it->total <= totalCost)
			return;

		const auto heuristic = heuristicDistance(successor.pos, end);

		successor.prev = currentNode.pos;
		successor.hasPrev = true;
		successor.total = totalCost;
		successor.heuristic = heuristic;

		if (it != open.end())
			open.update(it, successor);
		else {
			open.emplace(successor);
			addedNodes++;
		}
	}

	// Heuristic distance calculations
	// http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html#heuristics-for-grid-maps
	#define D 1
	#define D2 1.414213562373f
	// Diagonal distance
	float AStar::heuristicDistance(const PairUint &a, const PairUint &b) {
		auto dx = a.first - b.first;
		auto dy = a.second - b.second;
		return D * (dx + dy) + (D2 - 2 * D) * std::min(dx, dy);
	}
}