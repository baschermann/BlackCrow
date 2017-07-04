#pragma once
#include <BWAPI.h>
#include "Common.h"

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

		// Mako from StackOverflow @ http://stackoverflow.com/questions/3706219/algorithm-for-iterating-over-an-outward-spiral-on-a-discrete-2d-grid-from-the-or
		class SpiralOut {
		protected:
			unsigned layer;
			unsigned leg;
		public:
			int x, y;
			SpiralOut();
			void goNext();
		};

		/// TODO make template class
		class DampenedAverager {
		public:
			double average = 0;
			DampenedAverager(int size, double dampFactor);
			void add(double number);
		private:
			std::deque<double> list;
			int maxSize = 10;
			double dampFactor = 0.05;
		};

		RgbColor HsvToRgb(HsvColor hsv);
		HsvColor RgbToHsv(RgbColor rgb);

		double distance(int x1, int y1, int x2, int y2);
		double distance(BWAPI::Position p1, BWAPI::Position p2);
		double distance(BWAPI::TilePosition p1, BWAPI::TilePosition p2);
		BWAPI::Unit findClosestUnit(std::vector<BWAPI::Unit> units, BWAPI::Position position);

		BWAPI::Position middleOfBuilding(BWAPI::UnitType building);

		int randomIntIncl(int a, int b);
		bool isFightingBuilding(const EnemyUnitPtr eu);
	}
}