#pragma once
#include <BWAPI.h>
#include "Common.h"
#include <queue>
#include "PriorityQueue.h"

namespace BlackCrow {

	class BlackCrow;

	class AStar {
	public:
		AStar(BlackCrow& blackCrow, PairUint start, PairUint end);
		std::vector<PairUint> startSearching();

		int analyzedNodes = 0;
		int lookedAtNodes = 0;
		int addedNodes = 0;

	//private:

		struct Node {
			PairUint pos;
			PairUint prev;
			bool hasPrev = false;
			float total = 0; // g-value
			float heuristic = 0; // h-value

			Node(PairUint position) : pos(position) {};

			bool operator==(const Node &other) const {
				return pos == other.pos;
			}
		};

		
		struct NodeHasher {
			inline size_t operator() (const Node& node) const {
				return (node.pos.first * 5923) & (node.pos.second * 45751);
			}
		};
		

		struct Compare {
			bool operator()(const Node &a, const Node &b) {
				return a.total + a.heuristic > b.total + b.heuristic;
			}
		};
		

		BlackCrow& bc;
		PairUint start;
		PairUint end;
		
		//Open open;
		PriorityQueue<Node, Compare> open;
		std::map<PairUint, Node> closed;

		void expandNode(Node& successor, const float distanceCost, const Node& currentNode, const PairUint& end);
	};

	
}
