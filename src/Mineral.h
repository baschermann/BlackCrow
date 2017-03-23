#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>

namespace BlackCrow {

	class Worker;

	class Mineral {
	public:
		Mineral(BWEM::Mineral& mineral);

		BWEM::Mineral* bwemMineral;
		std::vector<Worker> workers;
		
		bool exists();
		void addWorker(Worker& worker);
		Worker removeWorker();
		void removeWorker(Worker& worker);
	};
}

