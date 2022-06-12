#pragma once

#include <filesystem>
#include <iostream>

#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "serialization.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace router::reader {

    class JSONreader final {
    public:
        using Path = std::filesystem::path;
        
    JSONreader() = delete;

        explicit JSONreader(ser::Serialization& serializator,
                            tc::TransportCatalogue& base_data,
                            renderer::MapRenderer& map_renderer,
                            control::RequestHandler& request_handle,
                            tr::TransportRouter& transport_router);

        void ReadBase(std::istream& in = std::cin);        
        void ReadRequests(std::istream& in = std::cin);
        void Answer(std::ostream& out = std::cout);

    private:
        ser::Serialization& serializator_;
        tc::TransportCatalogue& base_data_;
        renderer::MapRenderer& map_renderer_;
        control::RequestHandler& request_handler_;
        tr::TransportRouter& transport_router_;
        json::Array base_requests_;
        json::Array stat_requests_;
        json::Array answer_on_requests_;

        void LoadTransportCatalogue();
        void LoadMapRenderer();
        void LoadTransportRouter();
        void LoadStops();
        void LoadDistances();
        void LoadBuses();
        tc::Stop MakeStop(const json::Dict& description) const;
        void SetDistancesFromStop(const json::Dict& description);
        tc::Bus MakeBus(const json::Dict& description) const;

        renderer::Settings MakeRenderSettings(const json::Dict& description) const;
        tr::Settings MakeRouterSettings(const json::Dict& description) const;        
        Path MakeSerializationSetting(const json::Dict& description) const;
        json::Node AnswerStop(const json::Dict& description);
        json::Node AnswerBus(const json::Dict& description);
        json::Node AnswerMap(const json::Dict& description);
        json::Node AnswerRoute(const json::Dict& description);
    };
}
