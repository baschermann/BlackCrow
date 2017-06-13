#pragma once

#include <BWAPI.h>

namespace BlackCrow {

	class BlackCrow;

	class Overlords {
	public:
		Overlords(BlackCrow& parent);
		void onStart();
		void onFrame();

	protected:
		BlackCrow &bc;
	};
}