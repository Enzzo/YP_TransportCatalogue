#pragma once

#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "domain.h"
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

namespace router::tr {

    class TransportRouter {
    public:
        using Graph = graph::DirectedWeightedGraph<tr::Road>;
        using Router = graph::Router<tr::Road>;

        explicit                                                   TransportRouter(const tc::TransportCatalogue& base_data);

        void                                                       SetSettings(const Settings& settings);
        
        Settings                                                   GetSettings() const;               

        void                                                       MakeGraph();

        std::optional<tr::ReportRouter>                            GetReportRouter(const std::string_view from,
                                                                                   const std::string_view to) const;

    private:
        const tc::TransportCatalogue&                              base_data_;
        Settings                                                   settings_;
        Graph                                                      graph_;
        std::shared_ptr<Router>                                    router_;
        std::unordered_map<std::string_view, graph::VertexId>      name_id_;
        std::unordered_map<graph::VertexId, std::string_view>      id_name_;
        std::unordered_set<std::string>                            names_;

        template <typename ItStop>
        void                                                       AddEdgesGraph(ItStop begin, ItStop end,
            const std::string_view name);

        void                                                       AddEdgeGraph(std::shared_ptr<tc::Stop> stop);

        graph::VertexId                                            MakeVertexId(std::shared_ptr<tc::Stop> stop);

        double                                                     CalculateWeightEdge(std::shared_ptr<tc::Stop> from,
            std::shared_ptr<tc::Stop> to) const;
    };

    template<typename ItStop>
    inline void TransportRouter::AddEdgesGraph(ItStop begin, ItStop end,
        const std::string_view name) {
        using namespace graph;
        for (auto from_id = begin; from_id != end; ++from_id) {
            const VertexId from = MakeVertexId(*from_id);
            for (auto to_id = next(from_id); to_id != end; ++to_id) {
                const VertexId to = MakeVertexId(*to_id);
                double sum_minutes = 0;
                int span_count = 0;
                for (auto it = from_id; it != to_id; ++it) {
                    sum_minutes += CalculateWeightEdge(*it, *(it + 1));
                    ++span_count;
                }
                sum_minutes += settings_.bus_wait_time;
                graph_.AddEdge({ from, to, Road{sum_minutes, std::string(name), span_count} });
            }
        }
    }

}