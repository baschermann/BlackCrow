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
		//void fillResultIn(std::vector<PairUint>& resultPath);

		int analyzedNodes = 0;

	private:
		/*
		struct PriorityNode {
			PairUint pos;
			float f;

			PriorityNode(PairUint pos, float f) : pos(pos), f(f) {}

			bool operator<(const PriorityNode& other) const {
				return (f > other.f); // Reversed for smallest
			}

			bool operator==(const PriorityNode &other) const {
				return pos.first == other.pos.first
					&& pos.second == other.pos.second;
			}
		};
		*/

		struct Node {
			PairUint pos;
			PairUint prev;
			bool hasPrev = false;
			float total = 0; // g-value
			float heuristic = 0; // h-value

			//Node(PairUint position) : pos(position), prev(PairUint(0, 0)) {};
			//Node(PairUint position, PairUint previous) : pos(position), prev(previous) {};
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
				//return a.totalCost + a.heuristic > b.totalCost + b.heuristic;
				return a.total + a.heuristic > b.total + b.heuristic;
			}
		};
		

		BlackCrow& bc;
		PairUint start;
		PairUint end;
		
		//Open open;
		PriorityQueue<Node, Compare> open;
		//std::unordered_set<Node, NodeHasher> closed;
		std::map<PairUint, Node> closed;

		void expandNode(Node& successor, const float distanceCost, const Node& currentNode, const PairUint& end);
	};

	
}
