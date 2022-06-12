#pragma once
#include <iostream>

#include "transport_catalogue.h"

namespace inp {
	using tc::TransportCatalogue;

	class Reader {
		TransportCatalogue& transport_catalogue_;

	public:
		Reader() = delete;
		explicit Reader(TransportCatalogue& transport_catalogue) : transport_catalogue_(transport_catalogue) {};

		void Request(std::istream& request = std::cin);

	private:
		void TrimString(std::string&) const;
		[[nodiscard]] std::pair<int, std::string> ParseDescription(std::istream&)const;
		void ParseStop(std::istream&);
		void ParseBus(std::istream&);
	};
}