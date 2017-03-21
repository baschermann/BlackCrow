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
		virtual int getMineralCost() = 0;
		virtual int getGasCost() = 0;
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

		int getMineralCost() override;
		int getGasCost() override;
		void onFrame() override {};
	};

	class PlannedBuilding : public Planned {
	public:
		PlannedBuilding(BlackCrow& parent, BWAPI::UnitType type);

		int getMineralCost() override;
		int getGasCost() override;
		void onFrame() override {};
	};

	class PlannedTech : public Planned {
	public:
		PlannedTech(BlackCrow& parent, BWAPI::TechType type);

		int getMineralCost() override;
		int getGasCost() override;
		void onFrame() override {};
	};

	class PlannedUpgrade : public Planned {
	public:
		PlannedUpgrade(BlackCrow& parent, BWAPI::UpgradeType type);

		int getMineralCost() override;
		int getGasCost() override;
		void onFrame() override {};
	};

}