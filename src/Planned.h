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
		virtual float progressPercent() = 0;
		virtual std::string getName() = 0;

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

		operator std::string() const;
		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
		float progressPercent() override;
		std::string getName() override;
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

		operator std::string() const;
		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
		float progressPercent() override;
		std::string getName() override;
	};

	class PlannedExtractor : public Planned {
	public:
		PlannedExtractor(BlackCrow& parent, Geyser& geyser);

		Geyser& geyser;
		BWAPI::Unit drone = nullptr;

		operator std::string() const;
		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
		float progressPercent() override;

	private:
		bool alreadyBuiltExtractor = false;
		bool geyserUnitFound = false;
		std::string getName() override;
	};

	class PlannedTech : public Planned {
	public:
		PlannedTech(BlackCrow& parent, BWAPI::TechType type);

		BWAPI::TechType type;

		operator std::string() const;
		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
		float progressPercent() override;
		std::string getName() override;
	};

	class PlannedUpgrade : public Planned {
	public:
		PlannedUpgrade(BlackCrow& parent, BWAPI::UpgradeType type, int level);

		BWAPI::UpgradeType type;
		int level;
		BWAPI::Unit researchingBuilding = nullptr;

		operator std::string() const;
		int getMineralPrice() override;
		int getGasPrice() override;
		void onFrame() override;
		float progressPercent() override;
		std::string getName() override;
	};

}