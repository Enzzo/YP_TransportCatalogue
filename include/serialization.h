#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "domain.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

#include <map_renderer.pb.h>
#include <transport_catalogue.pb.h>

namespace router::ser {

class Serialization {
public:
    using                                       Path = std::filesystem::path;
    
                                                Serialization(tc::TransportCatalogue& transport_catalogue,
                                                              renderer::MapRenderer& map_renderer,
                                                              tr::TransportRouter& transport_router);
    
    void                                        SetSetting(const Path& path_to_base);
    
    void                                        CreateBase();
    
    void                                        AccessBase();
    
    //----------------------------------------TransportCatalogue----------------------------------------------------
private:
    proto_tc::Stop                              SaveStop(const tc::Stop& stop) const;
    
    proto_tc::FromToDistance                    SaveFromToDistance(std::shared_ptr<tc::Stop> from,
                                                                   std::shared_ptr<tc::Stop> to,
                                                                   uint64_t distance) const;
    
    proto_tc::Bus                               SaveBus(const tc::Bus& bus) const;
    
    void                                        SaveStops();
    
    void                                        SaveFromToDistance();
    
    void                                        SaveBuses();
    
    void                                        LoadStop(const proto_tc::Stop& stop);
    
    void                                        LoadFromToDistance(const proto_tc::FromToDistance& from_to_distance);
    
    void                                        LoadBus(const proto_tc::Bus& bus);
    
    void                                        LoadStops();
    
    void                                        LoadFromToDistances();
    
    void                                        LoadBuses();
    
    //----------------------------------------MapRenderer----------------------------------------------------------
private:
     void                                        SaveMap() const;
 
public:
    proto_renderer::Map                          LoadMap() const;
    
    //----------------------------------------TransportRouter----------------------------------------------------------
private:
    void                                        SaveParameters() const;
    
    void                                        LoadParameters();
    
private:
    Path                                         path_to_base_;
    tc::TransportCatalogue&                      transport_catalogue_;
    renderer::MapRenderer&                       map_renderer_;
    tr::TransportRouter&                         transport_router_;
    mutable proto_tc::TransportCatalogue         base_;
};
    
}