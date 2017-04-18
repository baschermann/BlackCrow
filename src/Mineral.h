#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>

namespace BlackCrow {

	class Worker;

	class Mineral {
	public:
		Mineral(const BWEM::Mineral* mineral);

		int id = -1;
		const BWEM::Mineral* bwemMineral;
		std::vector<std::shared_ptr<Worker>> workers;
		
		bool exists();
		void registerWorker(std::shared_ptr<Worker> worker);
		void unregisterWorker(std::shared_ptr<Worker> worker);
	};

	bool operator==(const Mineral& left, const Mineral& right);
}

