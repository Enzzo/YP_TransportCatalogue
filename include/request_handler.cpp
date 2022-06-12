#include "request_handler.h"

namespace router::control {

    RequestHandler::RequestHandler(const ser::Serialization& serializator,
                                   const tc::TransportCatalogue& base_data,
                                   const renderer::MapRenderer& renderer,
                                   const tr::TransportRouter& router)
        : serializator_(serializator)
        , base_data_(base_data)
        , renderer_(renderer)
        , router_(router) {}

    std::optional<tc::BusStat> RequestHandler::GetBusStat(const std::string_view bus_number) const {
        return base_data_.GetBusStat(bus_number);
    }

    std::shared_ptr<tc::Buses> RequestHandler::GetBusesByStop(const std::string_view stop_name) const {
        return base_data_.GetAllBusesStop(stop_name);
    }

    proto_renderer::Map RequestHandler::RenderMap() const {
        return serializator_.LoadMap();
    }

    std::optional<tr::ReportRouter> RequestHandler::GetReportRouter(const std::string_view from,
                                                               const std::string_view to) const {
        return router_.GetReportRouter(from, to);
    }

}