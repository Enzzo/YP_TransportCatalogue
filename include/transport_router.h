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

namespace tr {
    class TransportRouter {
    public:
        using Graph = graph::DirectedWeightedGraph<Road>;
        using Router = graph::Router<Road>;

    private:
        const tc::TransportCatalogue& transport_catalogue_;
        Settings settings_;
        Graph graph_;
        std::shared_ptr<Router> router_;        
        std::unordered_map<std::string_view, graph::VertexId> name_id_;
        std::unordered_map<graph::VertexId, std::string_view> id_name_;
        std::unordered_set<std::string> names_;

    public:
        explicit TransportRouter(const tc::TransportCatalogue& transport_catalogue) : transport_catalogue_(transport_catalogue), graph_(450) {};
        void SetSettings(const Settings& settings);
        Settings GetSettings() const;
        void MakeGraph();
        std::optional<ReportRouter> GetReportRouter(const std::string_view from, const std::string_view to) const;

    private:
        template <typename It>
        void AddEdgesGraph(It begin, It end, const std::string_view name);
        void AddEdgeGraph(std::shared_ptr<Stop> stop);
        graph::VertexId MakeVertexId(std::shared_ptr<Stop> stop);
        double CalculateWeightEdge(std::shared_ptr<Stop> from, std::shared_ptr<Stop> to) const;

    };

    template<typename It>
    inline void TransportRouter::AddEdgesGraph(It begin, It end,
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