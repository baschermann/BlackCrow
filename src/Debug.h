#pragma once
#include <BWAPI.h>
#include "Planned.h"
#include <memory>
#include "Map.h"

namespace BlackCrow {

	class BlackCrow;
	class UnitMix;

	class DebugPerformanceDisplay {
	public:
		void elapsedTime(double millis);
		void updateAndDraw(BlackCrow& bc, int x, int y);
		void setBackgroundColor(BWAPI::Color color);
		void setShowPercentage(double percentage);
		void setShowSpikes(bool show);

	private:
		std::list<double> frameTimeHistory;
		double highestFrameTime = 0;
		double highestFrameTimeAgo = 0;
		BWAPI::Color backgroundColor = BWAPI::Colors::Brown;
		double showPercentage = 0;
		bool showSpikes = true;
	};

	class Debug {

		struct Command {
			std::string command;
			std::string description;
			std::function<void(void)> action;
			bool isActive = false;
			bool executeOnce = false;

			template <class UnaryFunction>
			Command(std::string command,
				std::string description,
				bool active,
				bool executeOnce,
				UnaryFunction action)
				: command(command),
				description(description),
				isActive(active),
				executeOnce(executeOnce),
				action(action) {};
		};

	public:
		Debug(BlackCrow &parent);

		DebugPerformanceDisplay displayBot;
		DebugPerformanceDisplay displayBroodwar;
		std::vector<Command> commands;
		std::vector<std::function<void(const unsigned int x, const unsigned int y, const Map::Cell& cell)>> cellFunctions;
		std::vector<std::function<void(const unsigned int x, const unsigned int y, const Map::MiniCell& cell)>> miniCellFunctions;

		void onStart();
		void onFrame();
		bool command(std::string text);
		static std::string getShortName(std::string longName);

		template <class UnaryFunction>
		void addCommand(std::string command, std::string description, bool active, bool executeOnce, UnaryFunction action);

		template<class UnaryFunction>
		void runOnEveryCell(UnaryFunction action);

		template<class UnaryFunction>
		void runOnEveryMiniCell(UnaryFunction action);
		
	private:
		BlackCrow &bc;
		std::string getOnOffString(bool value);
	};
}