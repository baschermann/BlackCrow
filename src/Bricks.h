#pragma once
#include <BWAPI.h>

namespace BlackCrow {

	class BlackCrow;



	class Bricks {
	public:
		Bricks(BlackCrow& parent);
		void onStart();
		void onFrame();

	protected:
		BlackCrow &bc;
	};
}