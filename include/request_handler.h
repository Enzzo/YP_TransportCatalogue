#pragma once
#include <optional>
#include <memory>
#include <set>

#include "transport_router.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

class RequestHandler {
	const tc::TransportCatalogue& base_;
	const renderer::MapRenderer& map_renderer_;
	const tr::TransportRouter& router_;

public:
	RequestHandler(const tc::TransportCatalogue&, const renderer::MapRenderer&, const tr::TransportRouter&);
	std::optional<BusInfo> GetBusInfo(const std::string_view) const;
	std::shared_ptr<Buses> GetBusesByStop(const std::string_view) const;
	void RenderMap(std::ostream& out = std::cout)const;
	std::optional<tr::ReportRouter> GetReportRouter(const std::string_view, const std::string_view)const;
};