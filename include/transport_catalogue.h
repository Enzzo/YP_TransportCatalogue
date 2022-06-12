#pragma once

#include <deque>
#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "domain.h"

namespace router::tc {

    class TransportCatalogue {
    public:
        using StopsDistance = std::unordered_map<std::pair<std::shared_ptr<Stop>, std::shared_ptr<Stop>>, size_t, HashPairStops>;
        
        void AddBus(const Bus& bus);
        void AddStop(const Stop& stop);
        void SetDistanceBetweenStops(std::shared_ptr<Stop> from, std::shared_ptr<Stop> to, size_t distance);
        size_t GetDistanceBetweenStops(std::shared_ptr<Stop> from, std::shared_ptr<Stop> to) const;
        std::shared_ptr<Bus> FindBus(const std::string_view bus_number) const;
        std::shared_ptr<Stop> FindStop(const std::string_view stop_name) const;
        std::optional<BusStat> GetBusStat(const std::string_view bus_number) const;
        std::shared_ptr<Buses> GetAllBusesStop(const std::string_view stop_name) const;
        const Buses& GetBuses() const;
        const Stops& GetStops() const;        
        const StopsDistance& GetStopsDistance() const;

    private:
        std::deque<Bus> buses_;
        std::deque<Stop> stops_;
        Buses ptr_buses_;
        Stops ptr_stops_;
        std::unordered_map<std::string_view, std::shared_ptr<Bus>> index_buses_;
        std::unordered_map<std::string_view, std::shared_ptr<Stop>> index_stops_;
        std::unordered_map<std::shared_ptr<Bus>, BusStat> info_of_buses_;
        std::unordered_map<std::shared_ptr<Stop>, Buses> buses_for_stops_;
        StopsDistance length_between_stops_;
        void SetBusStat(std::shared_ptr<Bus> bus);
    };
}
