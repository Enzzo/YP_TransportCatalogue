#include <iostream>
#include <fstream>
#include <string>

#include "json.h"
#include "json_reader.h"
#include "map_renderer.h"

int main() {
	tc::TransportCatalogue base;	
	MapRenderer renderer;
	RequestHandler request(base, renderer);
	JSONreader reader(base, renderer, request);

	const json::Document doc = json::Load(std::cin);
	const auto& mode = doc.GetRoot().AsMap();
	
	if (mode.count("base_requests") || mode.count("render_settings")) {
		reader.ReadRequest(doc);
	}
	if (mode.count("stat_requests")) {
		reader.ReadRequests(doc);
		reader.ReadTransportCatalogue();

		std::ofstream out("out.svg");
		request.RenderMap(out);
	}

	return 0;
}