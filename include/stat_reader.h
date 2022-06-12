#pragma once

#include <iostream>

#include "transport_catalogue.h"

namespace st {
	using tc::TransportCatalogue;

	class Reader {
		const TransportCatalogue& transport_catalogue_;
		std::string mode_;
		std::string description_;

	public:
		explicit Reader(const TransportCatalogue& transport_catalogue) : transport_catalogue_(transport_catalogue) {}
		void Request(std::istream & = std::cin);
		void Answer(std::ostream & = std::cout)const;
	
	private:
		void PrintStatBus(std::ostream&) const;
		void PrintStatStop(std::ostream&) const;
	};
}