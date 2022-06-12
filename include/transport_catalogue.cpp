#include <algorithm>
#include <iterator>
#include <unordered_set>

#include "transport_catalogue.h"

namespace router::tc {

    using namespace std;

    void TransportCatalogue::AddBus(const Bus& bus) {
        const auto& iter_bus = buses_.emplace(buses_.end(), bus);
        const auto ptr_bus = make_shared<Bus>(*iter_bus);
        ptr_buses_.emplace(ptr_bus);
        for (auto& stop : ptr_bus->stops) {
            stop->free = false;
            buses_for_stops_[stop].emplace(ptr_bus);
        }
        SetBusStat(ptr_bus);
        index_buses_[ptr_bus->number] = ptr_bus;
    }

    void TransportCatalogue::AddStop(const Stop& stop) {
        const auto& iter_stop = stops_.emplace(stops_.end(), stop);
        const auto ptr_stop = make_shared<Stop>(*iter_stop);
        ptr_stops_.emplace(ptr_stop);
        buses_for_stops_[ptr_stop];
        index_stops_[ptr_stop->name] = ptr_stop;
    }

    void TransportCatalogue::SetDistanceBetweenStops(shared_ptr<Stop> from,
                                                     shared_ptr<Stop> to,
                                                     size_t distance) {
        length_between_stops_[make_pair(from, to)] = distance;
    }

    size_t TransportCatalogue::GetDistanceBetweenStops(shared_ptr<Stop> from,
                                                       shared_ptr<Stop> to) const {
        auto pair = make_pair(from, to);
        if (length_between_stops_.count(pair) != 0) {
            return length_between_stops_.at(pair);
        }
        pair = make_pair(to, from);
        if (length_between_stops_.count(pair) != 0) {
            return length_between_stops_.at(pair);
        }
        return 0;
    }

    shared_ptr<Bus> TransportCatalogue::FindBus(const string_view bus_number) const {
        if (index_buses_.count(bus_number) != 0) {
            return index_buses_.at(bus_number);
        }
        return nullptr;
    }

    shared_ptr<Stop> TransportCatalogue::FindStop(const string_view stop_name) const {
        if (index_stops_.count(stop_name) != 0) {
            return index_stops_.at(stop_name);
        }
        return nullptr;
    }

    optional<BusStat> TransportCatalogue::GetBusStat(const string_view bus_number) const {
        const shared_ptr<Bus>& bus = FindBus(bus_number);
        if (!bus) {
            return nullopt;
        }
        return info_of_buses_.at(bus);
    }

    shared_ptr<Buses> TransportCatalogue::GetAllBusesStop(const string_view stop_name) const {
        shared_ptr<Stop> stop = FindStop(stop_name);
        if (!stop) {
            return nullptr;
        }
        return make_shared<Buses>(buses_for_stops_.at(stop));
    }

    const Buses& TransportCatalogue::GetBuses() const {
        return ptr_buses_;
    }

    const Stops& TransportCatalogue::GetStops() const {
        return ptr_stops_;
    }
    
    const TransportCatalogue::StopsDistance& TransportCatalogue::GetStopsDistance() const {
        return length_between_stops_;
    }

    void TransportCatalogue::SetBusStat(shared_ptr<Bus> bus) {
        //using namespace geo::ComputeDistance;
        double route_length_geo = 0;
        size_t route_length_real = 0;
        const auto& stops = bus->stops;
        for (auto it = stops.begin(); distance(it, stops.end()) != 1; ++it) {
            const auto& current_stop = *it;
            const auto& next_stop = *(it + 1);
            route_length_geo += ComputeDistance(current_stop->coords, next_stop->coords);
            route_length_real += GetDistanceBetweenStops(current_stop, next_stop);
        }

        if (!bus->is_ring) {
            for (auto it = stops.rbegin(); distance(it, stops.rend()) != 1; ++it) {
                const auto& current_stop = *it;
                const auto& next_stop = *(it + 1);
                route_length_real += GetDistanceBetweenStops(current_stop, next_stop);
            }
            route_length_geo *= 2;
        }
        
        unordered_set<string_view> unique_stops;
        for (const auto& stop : stops) {
            unique_stops.emplace(stop->name);
        }
        
        BusStat info{ bus->is_ring ? bus->stops.size() : 2 * bus->stops.size() - 1,
                      unique_stops.size(), 
                      route_length_real,
                      route_length_real / route_length_geo };
        info_of_buses_[bus] = move(info);
    }
}