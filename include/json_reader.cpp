#include <algorithm>

#include "json_builder.h"
#include "json_reader.h"

namespace router::reader {
    
    using namespace router::tc;
    using namespace std;

    JSONreader::JSONreader(ser::Serialization& serializator,
                           TransportCatalogue& base_data,
                           renderer::MapRenderer& map_renderer,
                           control::RequestHandler& request_handle,
                           tr::TransportRouter& transport_router)
        : serializator_(serializator)
        , base_data_(base_data)
        , map_renderer_(map_renderer)
        , request_handler_(request_handle)
        , transport_router_(transport_router) {}

    void JSONreader::ReadBase(istream& in) {
        const auto load = json::Load(in).GetRoot().AsDict();
        base_requests_ = load.at("base_requests"s).AsArray();
        serializator_.SetSetting(MakeSerializationSetting(load.at("serialization_settings"s).AsDict()));
        map_renderer_.SetSettings(MakeRenderSettings(load.at("render_settings").AsDict()));
        transport_router_.SetSettings(MakeRouterSettings(load.at("routing_settings"s).AsDict()));
        LoadTransportCatalogue();
        LoadMapRenderer();
        //LoadTransportRouter();
        serializator_.CreateBase();
    }
    
    void JSONreader::ReadRequests(istream& in) {
        const auto load = json::Load(in).GetRoot().AsDict();
        stat_requests_ = load.at("stat_requests"s).AsArray();
        serializator_.SetSetting(MakeSerializationSetting(load.at("serialization_settings"s).AsDict()));
        serializator_.AccessBase();
        LoadTransportRouter();
    }

    void JSONreader::Answer(ostream& out) {
        using namespace json;
        for (const auto& request : stat_requests_) {
            const auto& description = request.AsDict();
            const auto& type = description.at("type"s).AsString();
            if (type == "Stop"s) {
                answer_on_requests_.push_back(AnswerStop(description));
            } else if (type == "Bus"s) {
                answer_on_requests_.push_back(AnswerBus(description));
            } else if (type == "Map"s) {
                answer_on_requests_.push_back(AnswerMap(description));
            } else if (type == "Route"s) {
                answer_on_requests_.push_back(AnswerRoute(description));
            }
        }
        const Document report(answer_on_requests_);
        Print(report, out);
    }

    void JSONreader::LoadTransportCatalogue() {
        LoadStops();
        LoadDistances();
        LoadBuses();
    }

    void JSONreader::LoadMapRenderer() {
        map_renderer_.SetBorder(base_data_.GetStops());
        map_renderer_.SetTrail(base_data_.GetBuses());
        map_renderer_.SetStation(base_data_.GetStops());
    }

    void JSONreader::LoadTransportRouter() {
        transport_router_.MakeGraph();
    }

    void JSONreader::LoadStops() {
        for (const auto& request : base_requests_) {
            const auto& description = request.AsDict();
            if (description.at("type"s).AsString() == "Stop"s) {
                base_data_.AddStop(MakeStop(description));
            }
        }
    }

    void JSONreader::LoadDistances() {
        for (const auto& request : base_requests_) {
            const auto& description = request.AsDict();
            if (description.at("type"s).AsString() == "Stop"s) {
                SetDistancesFromStop(description);
            }
        }
    }

    void JSONreader::LoadBuses() {
        for (const auto& request : base_requests_) {
            const auto& description = request.AsDict();
            if (description.at("type"s).AsString() == "Bus"s) {
                base_data_.AddBus(MakeBus(description));
            }
        }
    }

    Stop JSONreader::MakeStop(const json::Dict& description) const {
        return { description.at("name"s).AsString(),
                 description.at("latitude"s).AsDouble(),
                 description.at("longitude"s).AsDouble() };
    }

    void JSONreader::SetDistancesFromStop(const json::Dict& description) {
        const auto from = base_data_.FindStop(description.at("name"s).AsString());
        for (const auto& [stop_name, distance] : description.at("road_distances"s).AsDict()) {
            const auto to = base_data_.FindStop(stop_name);
            base_data_.SetDistanceBetweenStops(from, to, distance.AsInt());
        }
    }

    Bus JSONreader::MakeBus(const json::Dict& description) const {
        Bus bus;
        bus.is_ring = description.at("is_roundtrip"s).AsBool();
        bus.number = description.at("name"s).AsString();
        const auto& stops = description.at("stops"s).AsArray();
        for (const auto& stop : stops) {
            const auto& stop_name = stop.AsString();
            bus.stops.push_back(base_data_.FindStop(stop_name));
        }
        return bus;
    }

    renderer::Settings JSONreader::MakeRenderSettings(const json::Dict& description) const {
        auto make_point = [](const json::Node& node) {
            using namespace renderer;
            const auto& array = node.AsArray();
            return MakePoint(array.front().AsDouble(),
                array.back().AsDouble());
        };

        auto make_color = [](const json::Node& node) {
            using namespace renderer;
            if (node.IsArray()) {
                const auto& array = node.AsArray();
                if (array.size() == 3) {
                    return MakeColor(array.at(0).AsInt(),
                        array.at(1).AsInt(), array.at(2).AsInt());
                } else if (array.size() == 4) {
                    return MakeColor(array.at(0).AsInt(),
                        array.at(1).AsInt(), array.at(2).AsInt(),
                        array.at(3).AsDouble());
                }
            } 
            return MakeColor(node.AsString());
        };

        renderer::Settings settings;
        settings.width = description.at("width"s).AsDouble();
        settings.height = description.at("height"s).AsDouble();
        settings.padding = description.at("padding"s).AsDouble();
        settings.line_width = description.at("line_width"s).AsDouble();
        settings.stop_radius = description.at("stop_radius"s).AsDouble();
        settings.bus_label_font_size = description.at("bus_label_font_size"s).AsInt();
        settings.bus_label_offset = make_point(description.at("bus_label_offset"s));
        settings.stop_label_font_size = description.at("stop_label_font_size"s).AsInt();
        settings.stop_label_offset = make_point(description.at("stop_label_offset"s));
        settings.underlayer_color = make_color(description.at("underlayer_color"s));
        settings.underlayer_width = description.at("underlayer_width"s).AsDouble();
        const auto& array_color = description.at("color_palette"s).AsArray();
        settings.color_palette.reserve(array_color.size());
        for (const auto& color : array_color) {
            settings.color_palette.push_back(make_color(color));
        }
        return settings;
    }

    tr::Settings JSONreader::MakeRouterSettings(const json::Dict& description) const {
        return { description.at("bus_wait_time"s).AsInt(),
                 description.at("bus_velocity"s).AsDouble() };
    }
    
    JSONreader::Path JSONreader::MakeSerializationSetting(const json::Dict& description) const {
        return Path(description.at("file"s).AsString());
    } 

    json::Node JSONreader::AnswerStop(const json::Dict& description) {
        json::Builder builder_node;
        builder_node.StartDict()
                        .Key("request_id"s).Value(description.at("id"s).AsInt());
        const auto info = request_handler_.GetBusesByStop(description.at("name"s).AsString());
        if (info) {
            builder_node.Key("buses"s).StartArray();
            for (const auto& bus : *info) {
                builder_node.Value(bus->number);
            }
            builder_node.EndArray();
        } else {
            builder_node.Key("error_message"s).Value("not found"s);
        }
        return builder_node.EndDict().Build();
    }

    json::Node JSONreader::AnswerBus(const json::Dict& description) {
        json::Builder builder_node;
        builder_node.StartDict()
                        .Key("request_id"s).Value(description.at("id"s).AsInt());
        const auto& info = request_handler_.GetBusStat(description.at("name"s).AsString());
        if (info) {
            builder_node.Key("curvature"s).Value(info->curvature)
                        .Key("route_length"s).Value(static_cast<int>(info->route_length))
                        .Key("stop_count"s).Value(static_cast<int>(info->count_stops))
                        .Key("unique_stop_count"s).Value(static_cast<int>(info->count_unique_stops));
        } else {
            builder_node.Key("error_message"s).Value("not found"s);
        }
        return builder_node.EndDict().Build();
    }

    json::Node JSONreader::AnswerMap(const json::Dict& description) {
        json::Builder builder_node;
        builder_node.StartDict().Key("request_id"s).Value(description.at("id"s).AsInt());
        builder_node.Key("map"s).Value(request_handler_.RenderMap().description());
        return builder_node.EndDict().Build();
    }

    json::Node JSONreader::AnswerRoute(const json::Dict& description) {
        using namespace graph;
        json::Builder builder_node;
        builder_node.StartDict()
            .Key("request_id"s).Value(description.at("id"s).AsInt());
        const auto& report = request_handler_.GetReportRouter(description.at("from"s).AsString(),
                                                              description.at("to"s).AsString());
        if (report) {
            builder_node.Key("total_time"s).Value(report->total_minutes)
                        .Key("items"s).StartArray();
            for (const auto& info : report->information) {
                builder_node.StartDict()
                                .Key("type"s).Value("Wait"s)
                                .Key("time"s).Value(info.wait.minutes)
                                .Key("stop_name"s).Value(info.wait.stop_name)
                            .EndDict()
                            .StartDict()
                                .Key("type"s).Value("Bus"s)
                                .Key("time"s).Value(info.bus.minutes)
                                .Key("bus"s).Value(info.bus.number)
                                .Key("span_count"s).Value(info.bus.span_count)
                            .EndDict();
            }
            builder_node.EndArray();
        } else {
            builder_node.Key("error_message"s).Value("not found"s);
        }
        return builder_node.EndDict().Build();
    }

}