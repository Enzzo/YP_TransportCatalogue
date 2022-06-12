#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

#include "json.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "transport_router.h"

#define DEBUG

using namespace std::literals;

int main(int args, char* argv[]) try {
	tc::TransportCatalogue base;
	tr::TransportRouter router(base);
	renderer::MapRenderer renderer;
	RequestHandler request(base, renderer, router);
	JSONreader reader(base, renderer, request, router);

#ifdef DEBUG
	std::ifstream ifst("input.txt");
	std::istream& ist(ifst);

	std::ofstream ofst("output.txt");
	std::ostream& ost(ofst);
#else
	std::istream& ist(std::cin);
	std::ostream& ost(std::cout);
#endif

	const json::Document doc = json::Load(ist);
	const auto& mode = doc.GetRoot().AsDict();

	if (mode.count("base_requests") || 
		mode.count("render_settings")) {
		reader.ReadRequest(doc);
	}
 	if (mode.count("stat_requests")) {
		reader.ReadRequests(doc);
	}

	if (args == 2 && argv[1] == "--render-only"sv) {
		request.RenderMap(ost);
	}
	else {
		reader.ReadTransportCatalogue(ost);
	}
	return 0;
}
catch (const std::exception& e) {
	std::cerr << "Error: "sv << e.what() << std::endl;
}