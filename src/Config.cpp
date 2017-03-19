#include <BWAPI.h>
#include "Config.h"

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	Config* config = new Config();

	Config::Config() {
		localSpeed = 6;
		maxDrones = 200;
		saturationLevel = 0.5;
		mineralSaturationMultiplier = 2;
		barSize = 3;
	}
}