#include <fstream>
#include <iostream>
#include <string_view>

#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "serialization.h"
#include "transport_catalogue.h"
#include "transport_router.h"

using namespace std::literals;
using namespace router;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);
    
    tc::TransportCatalogue base_data;
    tr::TransportRouter router(base_data);
    renderer::MapRenderer map_renderer;
    ser::Serialization serializator(base_data, map_renderer, router);
    control::RequestHandler request_handler(serializator, base_data, map_renderer, router);
    reader::JSONreader json_reader(serializator, base_data, map_renderer, request_handler, router);

    if (mode == "make_base"sv) {
        json_reader.ReadBase();
    } else if (mode == "process_requests"sv) {
        json_reader.ReadRequests();
        json_reader.Answer();
    } else {
        PrintUsage();
        return 1;
    }
    
    return 0;
}
