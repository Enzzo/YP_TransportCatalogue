#include <cstdint>
#include <fstream>
#include <sstream>
#include <utility>
#include <variant>

#include "serialization.h"

namespace router::ser {
    
Serialization::Serialization(tc::TransportCatalogue& transport_catalogue,
                             renderer::MapRenderer& map_renderer,
                             tr::TransportRouter& transport_router) 
    : transport_catalogue_(transport_catalogue)
    , map_renderer_(map_renderer)
    , transport_router_(transport_router) {}
    
void Serialization::SetSetting(const Path& path_to_base) {
    path_to_base_ = path_to_base;
}
    
void Serialization::CreateBase() {
    std::ofstream out_file(path_to_base_, std::ios::binary);
    SaveStops();
    SaveFromToDistance();
    SaveBuses();
    SaveMap();
    SaveParameters();
    base_.SerializeToOstream(&out_file);
}
    
void Serialization::AccessBase() {
    std::ifstream in_file(path_to_base_, std::ios::binary);
    base_.ParseFromIstream(&in_file);
    LoadStops();
    LoadFromToDistances();
    LoadBuses();
    LoadParameters();
}
  
//----------------------------------------TransportCatalogue----------------------------------------------------    
    
proto_tc::Stop Serialization::SaveStop(const tc::Stop& stop) const {
    proto_tc::Stop result;
    result.set_name(stop.name);
    result.mutable_coords()->set_lat(stop.coords.lat);
    result.mutable_coords()->set_lon(stop.coords.lon);
    result.set_free(stop.free);
    return result;
}
    
proto_tc::FromToDistance Serialization::SaveFromToDistance(std::shared_ptr<tc::Stop> from,
                                                           std::shared_ptr<tc::Stop> to, uint64_t distance) const {
    proto_tc::FromToDistance result;
    result.set_from(from->name);
    result.set_to(to->name);
    result.set_distance(distance);
    return result;
}
    
proto_tc::Bus Serialization::SaveBus(const tc::Bus& bus) const {
    proto_tc::Bus result;
    result.set_is_ring(bus.is_ring);
    result.set_number(bus.number);
    for (const auto& stop : bus.stops) {
        result.add_stop_names(stop->name);
    }
    return result;
}
    
void Serialization::SaveStops() {
    for (const auto& stop : transport_catalogue_.GetStops()) {
        *base_.add_stops() = std::move(SaveStop(*stop));
    }
}
    
void Serialization::SaveFromToDistance() {
    for (const auto& [from_to, distance] : transport_catalogue_.GetStopsDistance()) {
        *base_.add_stops_distance() = std::move(SaveFromToDistance(from_to.first, from_to.second, distance));
    }
}
    
void Serialization::SaveBuses() {
    for (const auto& bus : transport_catalogue_.GetBuses()) {
        *base_.add_buses() = std::move(SaveBus(*bus));
    }
}
    
void Serialization::LoadStop(const proto_tc::Stop& stop) {
    tc::Stop target;
    target.name = stop.name();
    target.coords.lat = stop.coords().lat();
    target.coords.lon = stop.coords().lon();
    target.free = stop.free();
    transport_catalogue_.AddStop(target);
}
    
void Serialization::LoadFromToDistance(const proto_tc::FromToDistance& from_to_distance) {
    const auto& from = transport_catalogue_.FindStop(from_to_distance.from());
    const auto& to = transport_catalogue_.FindStop(from_to_distance.to());
    transport_catalogue_.SetDistanceBetweenStops(from, to, from_to_distance.distance());
}
    
void Serialization::LoadBus(const proto_tc::Bus& bus) {
    tc::Bus target;
    target.is_ring = bus.is_ring();
    target.number = bus.number();
    for (int i = 0; i < bus.stop_names_size(); ++i) {
        target.stops.push_back(transport_catalogue_.FindStop(bus.stop_names(i)));
    }
    transport_catalogue_.AddBus(target);
}
    
void Serialization::LoadStops() {
    for (int i = 0; i < base_.stops_size(); ++i) {
        LoadStop(base_.stops(i));
    }
}
    
void Serialization::LoadFromToDistances() {
    for (int i = 0; i < base_.stops_distance_size(); ++i) {
        LoadFromToDistance(base_.stops_distance(i));
    }
}
    
void Serialization::LoadBuses() {
    for (int i = 0; i < base_.buses_size(); ++i) {
        LoadBus(base_.buses(i));
    }
}
    
//----------------------------------------MapRenderer----------------------------------------------------------
void Serialization::SaveMap() const{
    std::ostringstream out;
    map_renderer_.GetDocument().Render(out);
    base_.mutable_map()->set_description(out.str());
}
    
proto_renderer::Map Serialization::LoadMap() const {
    return base_.map();
}       
    
//----------------------------------------TransportRouter----------------------------------------------------------
    
void Serialization::SaveParameters() const {
    tr::Settings settings = transport_router_.GetSettings();
    auto& parameters = *base_.mutable_parameters();
    parameters.set_bus_wait_time(settings.bus_wait_time);
    parameters.set_bus_velocity(settings.bus_velocity);
}
    
void Serialization::LoadParameters() {
    tr::Settings result;
    const auto& parameters = base_.parameters();
    result.bus_wait_time = parameters.bus_wait_time();
    result.bus_velocity = parameters.bus_velocity();
    transport_router_.SetSettings(result);
}

}
