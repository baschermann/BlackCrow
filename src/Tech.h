#pragma once
#include <BWAPI.h>

namespace BlackCrow {

	class BlackCrow;
	struct Resources;

	class TechItem {
	public:
		enum class TechType {
			Unit,
			Upgrade,
			Tech,
			None
		};

		TechItem& operator=(const TechItem& other) {
			type = other.type;
			unit = other.unit;
			upgrade = other.upgrade;
			tech = other.tech;
			return *this;
		}

		TechType type;
		BWAPI::UnitType unit;
		BWAPI::UpgradeType upgrade;
		BWAPI::TechType tech;

		TechItem(BlackCrow& blackcrow, BWAPI::UnitType unit);
		TechItem(BlackCrow& blackcrow, BWAPI::UpgradeType upgrade);
		TechItem(BlackCrow& blackcrow, BWAPI::TechType tech);
		TechItem(BlackCrow& blackcrow);

		std::string getName();
		int timeNeeded();
		Resources cost();

	private:
		BlackCrow& bc;
	};

	class TechPath {
	public:
		TechPath(BlackCrow& blackcrow, BWAPI::UnitType unit);
		TechPath(BlackCrow& blackcrow, BWAPI::UpgradeType upgrade);
		TechPath(BlackCrow& blackcrow, BWAPI::TechType tech);

		std::vector<TechItem> items;

		int size();
		int timeNeeded();
		Resources cost();

	private:
		BlackCrow& bc;

		TechItem getEarlierTech(BWAPI::UnitType unit);
		TechItem getEarlierTech(BWAPI::UpgradeType upgrade);
		TechItem getEarlierTech(BWAPI::TechType tech);
		TechItem getEarlierTech(TechItem item);
		void fillTechPath(TechItem item);
	};

	
	class Tech {
	public:
		Tech(BlackCrow& blackcrow);

		std::vector<TechPath> pathes;

		void onStart();
		void onFrame();

		TechPath getTechPath(BWAPI::UnitType type);
		TechPath getTechPath(BWAPI::UpgradeType type);
		TechPath getTechPath(BWAPI::TechType type);

		const bool isAlreadyTeching(TechPath& path);
		int timeNeededTotal();
		Resources costTotal();

		bool isPaused();
		void pauseTeching();
		void continueTeching();
		void cancelAll();

	private:
		BlackCrow& bc;
	};
	
}