#pragma once
#include <BWAPI.h>

namespace BlackCrow {

	class BlackCrow;
	class Geyser;

	class Planned {
	public:

		enum class Status {
			ACTIVE,
			FAILED,
			COMPLETED
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

	class PlannedUnit : public Planned {
	public:
		PlannedUnit(BlackCrow& parent, BWAPI::UnitType type, BWAPI::Position nearTo);

		BWAPI::UnitType type;
		BWAPI::Position nearTo;
		BWAPI::Unit unit = nullptr;

		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
		BWAPI::Unit reservedLarva();

	private:
		bool alreadyGrabbedLarva = false;
	};

	class PlannedBuilding : public Planned {
	public:
		PlannedBuilding(BlackCrow& parent, BWAPI::UnitType type, BWAPI::TilePosition buildPosition);

		BWAPI::UnitType type;
		BWAPI::TilePosition buildPosition;
		BWAPI::Unit droneOrBuilding = nullptr;

		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
	};

	class PlannedExtractor : public Planned {
	public:
		PlannedExtractor(BlackCrow& parent, Geyser& geyser);

		Geyser& geyser;
		BWAPI::Unit drone = nullptr;

		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;

	private:
		bool alreadyGrabbedDrone = false;
		bool alreadyBuiltExtractor = false;
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