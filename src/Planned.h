#pragma once
#include <BWAPI.h>

namespace BlackCrow {

	class BlackCrow;

	class Planned {
	public:

		enum class Status {
			ACTIVE,
			FAILED
		};

		Planned() = delete;
		virtual ~Planned() = default;
		virtual int getMineralPrice() = 0;
		virtual int getGasPrice() = 0;
		virtual void onFrame() = 0;
		Status getStatus() { return status; };

	protected:
		BlackCrow &bc;
		Status status;
		Planned(BlackCrow& parent);
	};

	class PlannedUnitt : public Planned {
	public:
		PlannedUnitt(BlackCrow& parent, BWAPI::UnitType type);

		BWAPI::UnitType type;

		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
	};

	class PlannedBuilding : public Planned {
	public:
		PlannedBuilding(BlackCrow& parent, BWAPI::UnitType type, BWAPI::TilePosition buildPosition);

		BWAPI::UnitType type;
		BWAPI::TilePosition buildPosition;

		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
	};

	class PlannedTech : public Planned {
	public:
		PlannedTech(BlackCrow& parent, BWAPI::TechType type);

		BWAPI::TechType type;

		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
	};

	class PlannedUpgrade : public Planned {
	public:
		PlannedUpgrade(BlackCrow& parent, BWAPI::UpgradeType type);

		BWAPI::UpgradeType type;

		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
	};

}