#include "Tech.h"
#include "BlackCrow.h"
#include <BWAPI.h>
#include <numeric>

namespace BlackCrow {

	using namespace BWAPI;
	using namespace Filter;

	// ### Tech Item ###
	TechItem::TechItem(BlackCrow& blackcrow, BWAPI::UnitType) : bc(blackcrow), unit(unit) {
		type = TechType::Unit;
	};

	TechItem::TechItem(BlackCrow& blackcrow, BWAPI::UpgradeType) : bc(blackcrow), upgrade(upgrade) {
		type = TechType::Upgrade;
	};

	TechItem::TechItem(BlackCrow& blackcrow, BWAPI::TechType) : bc(blackcrow), tech(tech) {
		type = TechType::Tech;
	};

	TechItem::TechItem(BlackCrow& blackcrow) : bc(blackcrow) {
		type = TechType::None;
	};

	int TechItem::timeNeeded() {
		switch (type) {
		case TechType::Unit:
		{
			return unit.buildTime();
			break;
		}
		case TechType::Upgrade:
		{
			return upgrade.upgradeTime() + Broodwar->self()->getUpgradeLevel(upgrade) * upgrade.upgradeTimeFactor();
			break;
		}
		case TechType::Tech:
		{
			return tech.researchTime();
			break;
		}
		case TechType::None:
		{
			return 0;
			break;
		}
		}
		return 0;
	}

	Resources TechItem::cost() {
		Resources resources = { 0, 0 };

		switch (type) {
		case TechType::Unit:
		{
			resources.minerals += unit.mineralPrice();
			resources.gas += unit.gasPrice();
			break;
		}
		case TechType::Upgrade:
		{
			int upgradeLevel = Broodwar->self()->getUpgradeLevel(upgrade) + 1;
			resources.minerals += upgrade.mineralPrice(upgradeLevel);
			resources.gas += upgrade.gasPrice(upgradeLevel);
			Broodwar->sendText("Upgrade %s is at level (+1), costs: %i minerals, %i gas", upgrade.c_str());
			break;
		}
		case TechType::Tech:
		{
			resources.minerals += tech.mineralPrice();
			resources.gas += tech.gasPrice();
			break;
		}
		case TechType::None:
		{
			break;
		}
		}

		return resources;
	};

	// ### Tech Path ###
	TechPath::TechPath(BlackCrow& blackcrow, BWAPI::UnitType unit) : bc(blackcrow) {
		if (Broodwar->self()->hasUnitTypeRequirement(unit))
			return;

		TechItem item = getEarlierTech(unit);
		fillTechPath(item);
	}

	TechPath::TechPath(BlackCrow& blackcrow, BWAPI::UpgradeType upgrade) : bc(blackcrow) {
		TechItem item = getEarlierTech(upgrade);
		fillTechPath(item);
	}

	TechPath::TechPath(BlackCrow& blackcrow, BWAPI::TechType tech) : bc(blackcrow) {
		TechItem item = getEarlierTech(tech);
		fillTechPath(item);
	}


	int TechPath::size() { // TODO
		return items.size();
	}

	int TechPath::timeNeeded() { // TODO
		return std::accumulate(items.begin(), items.end(), 0, [](int sum, TechItem& item) { return sum + item.timeNeeded(); });
	}

	Resources TechPath::cost() { // TODO
		Resources res = { 0, 0 };
		for (TechItem item : items) {
			res = res + item.cost();
		}
		return res;
	}

	// Private
	void TechPath::fillTechPath(TechItem item) {
		while (item.type != TechItem::TechType::None) {
			items.push_back(item);
			item = getEarlierTech(item);
		}
	}

	TechItem TechPath::getEarlierTech(TechItem item) {
		TechItem newItem(bc);

		switch (item.type) {
		case TechItem::TechType::Unit:
			newItem = getEarlierTech(item.unit);
			break;
		case TechItem::TechType::Upgrade:
			newItem = getEarlierTech(item.upgrade);
			break;
		case TechItem::TechType::Tech:
			newItem = getEarlierTech(item.tech);
			break;
		case TechItem::TechType::None:
			break;
		}

		return newItem;
	}

	TechItem TechPath::getEarlierTech(BWAPI::UnitType unit) {
		for (auto required : unit.requiredUnits()) {
			if (required.first.isBuilding())
				return TechItem(bc, required.first);
		}
		return TechItem(bc);
	}

	TechItem TechPath::getEarlierTech(BWAPI::UpgradeType upgrade) {
		return TechItem(bc, upgrade.whatsRequired());
	}

	TechItem TechPath::getEarlierTech(BWAPI::TechType tech) {
		return TechItem(bc, tech.requiredUnit());
	}
}