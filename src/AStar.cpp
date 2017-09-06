#include "AStar.h"
#include "BlackCrow.h"
#include "Util.h"
#include "fiboqueue.h"

namespace BlackCrow {

	using namespace BWAPI;

	AStar::AStar(BlackCrow& blackCrow, PairUint start, PairUint end) : bc(blackCrow), start(start), end(end) {};

	/*
	void AStar::fillResultIn(std::vector<PairUint>& resultPath) {
		
		auto lastIt = closed.find(end);
		if (lastIt != closed.end()) { // No Path found
			Node last = *lastIt;
			do {
				resultPath.push_back(last.pos);
				last = *closed.find(last.prev);
			} while (last.hasPrev);
		}
		
	}*/
	
	std::vector<PairUint> AStar::startSearching() {
		open.emplace(Node(start));

		do {
			Node const currentNode = open.top();
			open.pop();
			
			closed.emplace(currentNode.prev, currentNode);
			analyzedNodes++;

			if (currentNode.pos == end) {
				std::vector<PairUint> result;

				Node node = currentNode;
				do {
					result.push_back(node.pos);
					auto nodeIt = closed.find(node.prev);
					node = nodeIt->first;
				} while (node.hasPrev);

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
		const auto index = open.find_if([&](const Node &node) { return node.pos == successor.pos; });

		if (index < open.size() && open[index].total <= totalCost)
			return;

		const auto heuristic = Util::distance(successor.pos.first, successor.pos.second, end.first, end.second);

		successor.prev = currentNode.pos;
		successor.hasPrev = true;
		successor.total = totalCost;

		if (index < open.size())
			open.update(index, successor);
		else
			open.emplace(successor);


		/*
		if (closed.find(successor) != closed.end()
			|| successor.pos.first < 0
			|| successor.pos.first >= bc.map.miniTileWidth
			|| successor.pos.second < 0
			|| successor.pos.second >= bc.map.miniTileHeight
			|| !bc.map.miniTiles[successor.pos.first][successor.pos.second].walkable
			) {
			return; // Skip this node
		}
		
		float tentativeG = currentNode.g + distanceCost;

		if (open.contains(successor) && tentativeG >= successor.g)
			return;

		successor.prev = currentNode.pos;
		successor.hasPrev = true;
		successor.g = tentativeG;

		float f = tentativeG + (float)Util::distance(successor.pos.first, successor.pos.second, end.first, end.second); // TODO + heuristic cost
		
		if (open.contains(successor))
			open.update(successor, f);
		else
			open.insert(successor, f);
			*/

	}

	/*
	// +++ Open +++
	void AStar::Open::insert(Node node, float f) {
		queue.push(PriorityNode(node.pos, f));
		node.f = f;
		map.insert_or_assign(node.pos, node);
	};

	AStar::Node AStar::Open::popTop() {
		PriorityNode pn = queue.top();
		queue.pop();

		auto nodeIt = map.find(pn.pos);
		if (nodeIt == map.end())
			return popTop();

		Node node = nodeIt->second;
		if(node.f != pn.f) // an updated value won't be correct. Pop it and try again!
			return popTop();
		
		map.erase(nodeIt);
		return node;
	};

	bool AStar::Open::contains(const Node& node) {
		return contains(node.pos);
	};

	bool AStar::Open::contains(const PairUint& pos) {
		return map.find(pos) == map.end() ? false : true;
	};

	void AStar::Open::update(const Node& node, float f) {
		update(node.pos, f);
	};

	void AStar::Open::update(const PairUint& pos, float f) {
		queue.push(PriorityNode(pos, f));
		map.find(pos)->second.f = f;
	};

	bool AStar::Open::empty() {
		return queue.empty();
	}
	*/
}