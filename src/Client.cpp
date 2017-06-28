#include <BWAPI.h>
#include <BWAPI/Client.h>
#include "BlackCrow.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

using namespace BWAPI;

int main(int argc, const char* argv[]) {

	std::cout << "Connecting to Starcraft..." << std::endl;
	while (!BWAPIClient.connect())
		std::this_thread::sleep_for(std::chrono::milliseconds{ 350 });

	while (BWAPI::BWAPIClient.isConnected()) {

		std::cout << "Waiting for a game to begin..." << std::endl;
		while (!Broodwar->isInGame()) // Crashes when Broodwar is closed
			BWAPI::BWAPIClient.update();
				
		BlackCrow::BlackCrow bot;

		while (Broodwar->isInGame()) {
			for (auto &e : Broodwar->getEvents()) {

				switch (e.getType()) {
				case EventType::MatchStart:
					bot.onStart();
					break;
				case EventType::MatchEnd:
					bot.onEnd(e.isWinner());
					break;
				case EventType::MatchFrame:
					bot.onFrame();
					break;
				case EventType::MenuFrame:
					bot.onFrame();
					break;
				case EventType::SendText:
					bot.onSendText(e.getText());
					break;
				case EventType::ReceiveText:
					bot.onReceiveText(e.getPlayer(), e.getText());
					break;
				case EventType::PlayerLeft:
					bot.onPlayerLeft(e.getPlayer());
					break;
				case EventType::NukeDetect:
					bot.onNukeDetect(e.getPosition());
					break;
				case EventType::UnitDiscover:
					bot.onUnitDiscover(e.getUnit());
					break;
				case EventType::UnitEvade:
					bot.onUnitEvade(e.getUnit());
					break;
				case EventType::UnitShow:
					bot.onUnitShow(e.getUnit());
					break;
				case EventType::UnitHide:
					bot.onUnitHide(e.getUnit());
					break;
				case EventType::UnitCreate:
					bot.onUnitCreate(e.getUnit());
					break;
				case EventType::UnitDestroy:
					bot.onUnitDestroy(e.getUnit());
					break;
				case EventType::UnitMorph:
					bot.onUnitMorph(e.getUnit());
					break;
				case EventType::UnitRenegade:
					bot.onUnitRenegade(e.getUnit());
					break;
				case EventType::SaveGame:
					bot.onSaveGame(e.getText());
					break;
				case EventType::UnitComplete:
					bot.onUnitComplete(e.getUnit());
					break;
				default:
					// Kabooom goes here. There is a missing EventType
					assert(false);
				}
			}

			BWAPI::BWAPIClient.update();
		}
		std::cout << "Game ended" << std::endl;
	}
	std::cout << "Press ENTER to continue..." << std::endl;
	std::cin.ignore();
	return 0;
}