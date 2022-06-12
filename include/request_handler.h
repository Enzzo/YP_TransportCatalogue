#pragma once
#include <optional>
#include <memory>
#include <set>

#include "transport_catalogue.h"
#include "map_renderer.h"

class RequestHandler {
	const tc::TransportCatalogue& base_;
	const MapRenderer& map_renderer_;
public:
	RequestHandler(const tc::TransportCatalogue&, const MapRenderer&);
	std::optional<BusInfo> GetBusInfo(const std::string_view) const;
	std::shared_ptr<Buses> GetBusesByStop(const std::string_view) const;
	void RenderMap(std::ostream& out = std::cout)const;
};