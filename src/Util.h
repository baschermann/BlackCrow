#pragma once
#include <BWAPI.h>
namespace BlackCrow {
	namespace Util {

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

		double distance(int x1, int y1, int x2, int y2);
		RgbColor HsvToRgb(HsvColor hsv);
		HsvColor RgbToHsv(RgbColor rgb);
		BWAPI::Unit findClosestUnit(BWAPI::Unitset* units, BWAPI::Position position);
	}
}