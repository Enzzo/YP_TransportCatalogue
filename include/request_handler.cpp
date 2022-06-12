#include "request_handler.h"

RequestHandler::RequestHandler(const tc::TransportCatalogue& base, const renderer::MapRenderer& map_renderer, const tr::TransportRouter& router) : base_(base), map_renderer_(map_renderer), router_(router) {}

std::optional<BusInfo> RequestHandler::GetBusInfo(const std::string_view number) const {
	return base_.GetBusInfo(number);
}

std::shared_ptr<Buses> RequestHandler::GetBusesByStop(const std::string_view stop_name) const {
	return base_.GetBusesByStop(stop_name);
}

void RequestHandler::RenderMap(std::ostream& out) const {
	map_renderer_.GetDocument().Render(out);
}

std::optional<tr::ReportRouter> RequestHandler::GetReportRouter(const std::string_view from, const std::string_view to) const {
	return router_.GetReportRouter(from, to);
}