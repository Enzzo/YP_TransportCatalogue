#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "domain.h"
#include "geo.h"
#include "map_renderer.h"
#include "serialization.h"
#include "transport_catalogue.h"
#include "transport_router.h"

#include <map_renderer.pb.h>

namespace router::control {

    class RequestHandler {
    public:
    RequestHandler() = delete;

    RequestHandler(const ser::Serialization& serializator, const tc::TransportCatalogue& base_data, const renderer::MapRenderer& renderer, const tr::TransportRouter& router);
        std::optional<tc::BusStat> GetBusStat(const std::string_view bus_number) const;
        std::shared_ptr<tc::Buses> GetBusesByStop(const std::string_view stop_name) const;
        proto_renderer::Map RenderMap() const;
        std::optional<tr::ReportRouter> GetReportRouter(const std::string_view from, const std::string_view to) const;
             
    private:
        const ser::Serialization& serializator_;
        const tc::TransportCatalogue& base_data_;
        const renderer::MapRenderer& renderer_;
        const tr::TransportRouter& router_;
    };

}
