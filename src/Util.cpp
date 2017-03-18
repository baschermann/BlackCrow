#include "Util.h"
#include "BlackCrow.h"

namespace BlackCrow {
	using namespace BWAPI;
	using namespace Filter;

namespace Util {

	double distance(int x1, int y1, int x2, int y2) {
		double diffY = (double)(y1 - y2);
		double diffX = (double)(x1 - x2);
		return sqrt((diffY * diffY) + (diffX * diffX));
	}

	BWAPI::Unit findClosestUnit(BWAPI::Unitset* units, BWAPI::Position position) {
		double closestDistance = 99999999999;
		BWAPI::Unit closestUnit = nullptr;

		for (BWAPI::Unit unit : *units) {
			double dst = distance(position.x, position.y, unit->getPosition().x, unit->getPosition().y);
			if (dst < closestDistance) {
				closestDistance = dst;
				closestUnit = unit;
			}
		}

		return closestUnit;
	}

	// From Leszek Z at http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
	RgbColor HsvToRgb(HsvColor hsv) {
		RgbColor rgb;
		unsigned char region, remainder, p, q, t;

		if (hsv.s == 0) {
			rgb.r = hsv.v;
			rgb.g = hsv.v;
			rgb.b = hsv.v;
			return rgb;
		}

		region = hsv.h / 43;
		remainder = (hsv.h - (region * 43)) * 6;

		p = (hsv.v * (255 - hsv.s)) >> 8;
		q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
		t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

		switch (region) {
		case 0:
			rgb.r = hsv.v; rgb.g = t; rgb.b = p;
			break;
		case 1:
			rgb.r = q; rgb.g = hsv.v; rgb.b = p;
			break;
		case 2:
			rgb.r = p; rgb.g = hsv.v; rgb.b = t;
			break;
		case 3:
			rgb.r = p; rgb.g = q; rgb.b = hsv.v;
			break;
		case 4:
			rgb.r = t; rgb.g = p; rgb.b = hsv.v;
			break;
		default:
			rgb.r = hsv.v; rgb.g = p; rgb.b = q;
			break;
		}

		return rgb;
	}

	Util::HsvColor Util::RgbToHsv(RgbColor rgb) {
		HsvColor hsv;
		unsigned char rgbMin, rgbMax;

		rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
		rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

		hsv.v = rgbMax;
		if (hsv.v == 0) {
			hsv.h = 0;
			hsv.s = 0;
			return hsv;
		}

		hsv.s = 255 * long(rgbMax - rgbMin) / hsv.v;
		if (hsv.s == 0) {
			hsv.h = 0;
			return hsv;
		}

		if (rgbMax == rgb.r)
			hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
		else if (rgbMax == rgb.g)
			hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
		else
			hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

		return hsv;
	}

	// Mako from StackOverflow @ http://stackoverflow.com/questions/3706219/algorithm-for-iterating-over-an-outward-spiral-on-a-discrete-2d-grid-from-the-or
	SpiralOut::SpiralOut() : layer(1), leg(0), x(0), y(0) {}

	void SpiralOut::goNext() {
		switch (leg) {
		case 0: ++x; if (x == layer) ++leg; break;
		case 1: ++y; if (y == layer) ++leg; break;
		case 2: --x; if (-x == layer) ++leg; break;
		case 3: --y; if (-y == layer) { leg = 0; ++layer; } break;
		}
	}
}
}