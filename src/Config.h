#pragma once
#include <BWAPI.h>

namespace BlackCrow {

	// Config!
	class Config {

	public:
		Config();

		int localSpeed;
		int maxDrones;
		float saturationLevel;
		float barSize;
	};
}