#pragma once
#include <iostream>

#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json.h"
#include "domain.h"

class JSONreader final {
	tc::TransportCatalogue& base_;
	renderer::MapRenderer& map_renderer_;
	RequestHandler& request_handler_;
	tr::TransportRouter& router_;
	json::Array base_requests_;
	json::Array stat_requests_;
	json::Array answers_;

public:
	JSONreader() = delete;
	explicit JSONreader(tc::TransportCatalogue& base, renderer::MapRenderer& map_renderer, RequestHandler& request, tr::TransportRouter& router)
		: base_(base)
		, map_renderer_(map_renderer)
		, request_handler_(request)		
		, router_(router) {};

	void ReadRequest(const json::Document&);
	void ReadRequests(const json::Document&);
	void ReadTransportCatalogue(std::ostream& ost = std::cout);

private:
	Stop MakeStop(const json::Dict&);
	Bus MakeBus(const json::Dict&);
	void SetDistancesFromStop(const json::Dict& description);

	renderer::Settings MakeRenderSettings(const json::Dict&) const;
	tr::Settings MakeRouterSettings(const json::Dict&) const;
	json::Node ReadStop(const json::Dict&);
	json::Node ReadBus(const json::Dict&);
	json::Node ReadMap(const json::Dict&);
	json::Node ReadRoute(const json::Dict&);

	void LoadStops();
	void LoadBuses();
	void LoadDistances();
	void LoadTransportRouter();

	svg::Point SetPoint(const json::Node&)const;
	svg::Color SetColor(const json::Node&)const;
};