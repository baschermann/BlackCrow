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

		// Mako from StackOverflow @ http://stackoverflow.com/questions/3706219/algorithm-for-iterating-over-an-outward-spiral-on-a-discrete-2d-grid-from-the-or
		class SpiralOut{
		protected:
			unsigned layer;
			unsigned leg;
		public:
			int x, y;
			SpiralOut();
			void goNext();
		};
	}
}