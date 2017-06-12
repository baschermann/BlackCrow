#pragma once
#include <BWAPI.h>

namespace BlackCrow {

	// Note: Values can be modified by the program. These are starting values.
	class Config {

	public:
		Config();

		int localSpeed = 8; // Game Speed
		int maxDrones = 70;
		double mineralSaturationMultiplier = 2; // Workers per mineral patch
		float barSize = 3; // Health/Shield/Energy bar size
	};
}