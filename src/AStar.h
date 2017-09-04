#pragma once
#include <BWAPI.h>
#include "Common.h"
#include <queue>

namespace BlackCrow {

	class BlackCrow;

	class AStar {
	public:
		AStar(BlackCrow& blackCrow, PairUint start, PairUint end);
		void startSearching();
		void fillResultIn(std::vector<PairUint>& resultPath);

		int analyzedNodes = 0;

	private:
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

		struct Node {
			PairUint pos;
			PairUint prev;
			bool hasPrev = false;
			float g = 0;
			float f;

			Node(PairUint position) : pos(position), prev(PairUint(0, 0)) {};
			Node(PairUint position, PairUint previous) : pos(position), prev(previous) {};

			bool operator==(const Node &other) const {
				return pos.first == other.pos.first
					&& pos.second == other.pos.second;
			}
		};

		struct NodeHasher {
			inline size_t operator() (const Node& node) const {
				return (node.pos.first * 5923) & (node.pos.second * 45751);
			}
		};

		class Open { // TODO Use Fibonacci Heap/Queue
		private:
			std::priority_queue<PriorityNode> queue;
			std::unordered_map<PairUint, Node, NodeHasher> map;
		public:
			void insert(Node node, float f);
			Node popTop();
			bool contains(const Node& node);
			bool contains(const PairUint& pos);
			void update(const Node& node, float f);
			void update(const PairUint& pos, float f);
			bool empty();
		};

		BlackCrow& bc;
		PairUint start;
		PairUint end;
		
		Open open;
		std::unordered_set<Node, NodeHasher> closed;

		void expandNode(Node& successor, float distanceCost, const Node& currentNode, PairUint& end);
	};
}