#include <iostream>
#include <iomanip>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

int main() {
	tc::TransportCatalogue transport_catalogue;
	inp::Reader input(transport_catalogue);
	st::Reader output(transport_catalogue);

	std::cout << std::setprecision(6);

	int base_request_count;
	std::cin >> base_request_count;
	//make base
	for (int i = 0; i < base_request_count; ++i) {
		input.Request();
	}

	std::cin >> base_request_count;
	//read base
	for (int i = 0; i < base_request_count; ++i) {
		output.Request();
		output.Answer();
	}
	return 0;
}