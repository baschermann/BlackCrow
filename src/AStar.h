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

			// Needed for (tree) set/map
			inline bool operator<(const Node &other) const {
				return pos.second < other.pos.second || (pos.second == other.pos.second && pos.first < other.pos.first);
			}
		};

		/*
		struct NodeHasher {
			inline size_t operator() (const Node& node) const {
				return (node.pos.first * 5923) & (node.pos.second * 45751);
			}
		};
		*/

		// Comparator to find nodes in PriorityQueue.
		struct ValueCompare {
			bool operator()(const Node &a, const Node &b) const { return a < b; }
		};

		// Comparator to put cheapest nodes first.
		struct PrioCompare {
			bool operator()(const Node &a, const Node &b) const {
				return a.total + a.heuristic > b.total + b.heuristic;
			}
		};

		BlackCrow& bc;
		PairUint start;
		PairUint end;
		
		//Open open;
		PriorityQueue<Node, ValueCompare, PrioCompare> open;
		std::map<PairUint, Node> closed;

		void expandNode(Node& successor, const float distanceCost, const Node& currentNode, const PairUint& end);
		float heuristicDistance(const PairUint &a, const PairUint &b);
	};	
}