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
		std::vector<Worker> workers;
		
		bool exists();
		void registerWorker(Worker& worker);
		void unregisterWorker(Worker& worker);
	};

	bool operator==(const Mineral& left, const Mineral& right);
}

