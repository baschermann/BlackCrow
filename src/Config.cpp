#include <BWAPI.h>
#include "Config.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Config::Config() {
		localSpeed = 15;
		maxDrones = 200;
		saturationLevel = 0.5;
		mineralSaturationMultiplier = 2;
		barSize = 3;
	}
}