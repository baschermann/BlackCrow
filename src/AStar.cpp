#include "AStar.h"
#include "BlackCrow.h"
#include "Util.h"

namespace BlackCrow {

	AStar::AStar(BlackCrow& blackCrow, PairUint start, PairUint end) : bc(blackCrow), start(start), end(end) {};

	void AStar::fillResultIn(std::vector<PairUint>& resultPath) {
		auto lastIt = closed.find(end);
		if (lastIt != closed.end()) { // No Path found
			Node last = *lastIt;
			do {
				resultPath.push_back(last.pos);
				last = *closed.find(last.prev);
			} while (last.hasPrev);
		}
	}
	
	void AStar::startSearching() {
		open.insert(Node(start), 0);

		do {
			Node currentNode = open.popTop();
			
			closed.insert(currentNode);
			analyzedNodes++;

			if (currentNode.pos == end)
				return; // Found a path

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
	}

	void AStar::expandNode(Node& successor, float distanceCost, const Node& currentNode, PairUint& end) {
		
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

	}

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
}