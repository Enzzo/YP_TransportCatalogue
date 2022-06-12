#include <algorithm>
#include <numeric>

#include "transport_catalogue.h"

namespace tc {
	using namespace detail;
	void TransportCatalogue::AddStop(const Stop& stop) {
		Stop* found_stop = FindStop(stop.name);

		if (found_stop == nullptr) {
			std::deque<Stop>::iterator it;
			it = stops_.emplace(stops_.end(), stop);
			auto ptr_stop = &*it;
			index_stops_[ptr_stop->name] = ptr_stop;
			buses_for_stops_[ptr_stop];
		}
		else {
			*found_stop = { stop.name, stop.coord };
		}
	}

	void TransportCatalogue::AddBus(const Bus& bus) {
		Bus* found_bus = FindBus(bus.number);

		if (found_bus == nullptr) {
			std::deque<Bus>::iterator it = buses_.emplace(buses_.end(), bus);
			auto ptr_bus = &*it;
			index_buses_[ptr_bus->number] = ptr_bus;

			for (auto& stop : ptr_bus->stops) {
				buses_for_stops_[stop].emplace(ptr_bus);
			}
		}
		else {
			*found_bus = { bus.number, bus.is_ring, bus.stops };
		}
	}

	[[nodiscard]] Bus* TransportCatalogue::FindBus(const std::string& bus) const {
		if (index_buses_.count(bus) != 0) {
			return index_buses_.at(bus);
		}
		return nullptr;
	}

	[[nodiscard]] Stop* TransportCatalogue::FindStop(const std::string& stop) const {
		if (index_stops_.count(stop) != 0) {
			return index_stops_.at(stop);
		}
		return nullptr;
	}

	[[nodiscard]] const BusInfo TransportCatalogue::GetBusInfo(const Bus& bus)const {
		using geo::ComputeDistance;

		std::unordered_set<std::string> unique_stops;
		BusInfo info;
		const std::vector<Stop*>& v_stops = bus.stops;
		std::vector<double> dist;

		std::for_each(v_stops.begin(), v_stops.end(), [&unique_stops](auto stop) {
			unique_stops.insert(stop->name);
			});

		double geographical_length = std::transform_reduce(bus.stops.begin() + 1, bus.stops.end(),
			bus.stops.begin(), 0.0, std::plus<>{}, [](const Stop* s2, const Stop* s1) {
				return ComputeDistance(s1->coord, s2->coord);
			}
		);

		info.stops = bus.stops.size();
		info.unique_stops = unique_stops.size();

		info.route_length = std::transform_reduce(bus.stops.begin() + 1, bus.stops.end(),
			bus.stops.begin(), 0, std::plus<>{}, [this](const Stop* s2, const Stop* s1) {
				return GetDistanceBetweenStops(s1->name, s2->name);
			}
		);
		info.curvature = info.route_length / geographical_length;

		return info;
	}

	[[nodiscard]] const std::unordered_set<Bus*> TransportCatalogue::GetBusesByStop(const std::string& stop_name) const {
		Stop* stop = FindStop(stop_name);
		if (!stop) {
			return {};
		}
		return std::unordered_set<Bus*>(buses_for_stops_.at(stop));
	}

	void TransportCatalogue::AddDistanceBetweenStops(const std::string& from, const std::string& to, const int distance) {
		Stop* f = FindStop(from);
		Stop* t = FindStop(to);
		if (!f) { AddStop(std::move(Stop({ from, {0.0, 0.0} }))); }
		if (!t) { AddStop(std::move(Stop({ to, {0.0, 0.0} }))); }
		auto forward = std::make_pair(FindStop(from), FindStop(to));
		auto backward = std::make_pair(FindStop(to), FindStop(from));
		distance_between_stops_[forward] = distance;
		auto it = distance_between_stops_.find(backward);

		if (it == distance_between_stops_.end()) {
			distance_between_stops_.emplace((backward), distance);
		}
	}

	int TransportCatalogue::GetDistanceBetweenStops(const std::string& stop1, const std::string& stop2) const {
		const std::pair<Stop*, Stop*> p(FindStop(stop1), FindStop(stop2));

		return distance_between_stops_.at(p);
	}
}