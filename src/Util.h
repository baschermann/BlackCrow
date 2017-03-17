#pragma once
#include <BWAPI.h>
namespace BlackCrow {

	class BlackCrow;
	class BaseInformation;
	class Util;

	// Draws debug information
	class Util {

	public:
		typedef struct RgbColor {
			unsigned char r;
			unsigned char g;
			unsigned char b;
		} RgbColor;

		typedef struct HsvColor {
			unsigned char h;
			unsigned char s;
			unsigned char v;
		} HsvColor;

		Util(BlackCrow &bc);
		BWAPI::Unit findClosestUnit(BWAPI::Unitset* units, BWAPI::Position position);
		BaseInformation* Util::findClosestMiningBase(BWAPI::Position position);
		BaseInformation* Util::findClosestMiningBaseWithWorkers(BWAPI::Position position);
		BaseInformation* getClosestExpansionLocation(BWAPI::TilePosition position);
		double Util::distance(int x1, int y1, int x2, int y2);
		RgbColor HsvToRgb(HsvColor hsv);
		HsvColor RgbToHsv(RgbColor rgb);

	private:
		BlackCrow &bc;
	};
}