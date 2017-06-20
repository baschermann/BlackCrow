#include "Bricks.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Brick::Brick() {}

	void Brick::run() {

	}

	void Brick::successor(BrickPtr successor) {
		successors.push_back(successor);
	}

	namespace Bricks {
		BrickPtr newBrick() {
			return std::make_shared<Brick>();
		}
	}
}