#include "Util.h"
#include "MacroManager.h"

using namespace BWAPI;
using namespace Filter;

Util* util = new Util();

Util::Util() {
}

BWAPI::Unit Util::findClosestUnit(BWAPI::Unitset* units, BWAPI::Position position) {
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

// Base that can be actively mined from
BaseInformation* Util::findClosestMiningBase(BWAPI::Position position) {
	double closestDistance = 99999999999;
	BaseInformation* closestBase = nullptr;

	for (BaseInformation* base : *macroManager->bases) {
		if (base->hatchery) {
			double dst = distance(position.x, position.y, base->base->Center().x, base->base->Center().y);
			if (dst < closestDistance) {
				closestDistance = dst;
				closestBase = base;
			}
		}
	}

	return closestBase;
}

// Base that has either worker on minerals or gas
BaseInformation* Util::findClosestMiningBaseWithWorkers(BWAPI::Position position) {
	double closestDistance = 99999999999;
	BaseInformation* closestBase = nullptr;

	for (BaseInformation* base : *macroManager->bases) {
		if (base->hatchery && (base->workersOnMinerals.size() > 0 || base->workersOnGas.size() > 0)) {
			double dst = distance(position.x, position.y, base->base->Center().x, base->base->Center().y);
			if (dst < closestDistance) {
				closestDistance = dst;
				closestBase = base;
			}
		}
	}

	return closestBase;
}

BaseInformation* Util::getClosestExpansionLocation(BWAPI::TilePosition position) {
	double closestDistance = 99999999999;
	BaseInformation* closestBase = nullptr;

	for (BaseInformation* base : *macroManager->bases) {
		if (base->hatchery == nullptr && !base->isExpanding && base->base->GetArea()->AccessibleNeighbours().size() > 0) { // Handle Island Expansion
			double dst = distance(base->base->Location().x, base->base->Location().y, position.x, position.y);
			if (dst < closestDistance) {
				closestDistance = dst;
				closestBase = base;
			}
		}
	}

	return closestBase;
}

inline double Util::distance(int x1, int y1, int x2, int y2) {
	float diffY = y1 - y2;
	float diffX = x1 - x2;
	return sqrt((diffY * diffY) + (diffX * diffX));
}

// From Leszek Z at http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
Util::RgbColor Util::HsvToRgb(HsvColor hsv) {
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