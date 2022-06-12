#pragma once

#include <string_view>
#include <string>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>

#include "geo.h"


struct Stop {
	std::string name;
	geo::Coordinates coord;
};

struct Bus {
	std::string number;
	bool is_ring = false;
	std::vector<Stop*> stops;
};

struct BusInfo {
	int stops = 0;
	int unique_stops = 0;
	double route_length = 0.0;
	double curvature = 0.0;
};

namespace detail {
	class DistanceHasher {
	public:
		size_t operator()(const std::pair<Stop*, Stop*>& p) const {
			std::hash<const void*> sh1;
			std::hash<const void*> sh2;
			size_t h1 = sh1(p.first);
			size_t h2 = sh2(p.second) * 37 ^ 3;
			return h1 + h2;
		}
	};
}

namespace tc {
	using namespace detail;
	class TransportCatalogue {
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, Bus*> index_buses_;
		std::unordered_map<std::string, Stop*> index_stops_;
		std::unordered_map<Stop*, std::unordered_set<Bus*>> buses_for_stops_;
		std::unordered_map<std::pair<Stop*, Stop*>, int, DistanceHasher> distance_between_stops_;

	public:
		void AddBus(const Bus&);
		void AddStop(const Stop&);
		[[nodiscard]] Bus* FindBus(const std::string& bus_number)const;
		[[nodiscard]] Stop* FindStop(const std::string& stop_name)const;
		[[nodiscard]] const BusInfo GetBusInfo(const Bus& bus)const;
		[[nodiscard]] const std::unordered_set<Bus*> GetBusesByStop(const std::string&) const;
		void AddDistanceBetweenStops(const std::string&, const std::string&, const int);
		[[nodiscard]] inline int GetDistanceBetweenStops(const std::string&, const std::string&) const;
	};
}