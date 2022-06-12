#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "geo.h"

namespace router::tc {

    //--------------Stop----------------    

    struct Stop {
        std::string             name;
        geo::Coordinates        coords;
        bool                    free = true;
    };

    //--------------Bus----------------    

    struct Bus {
        bool                                     is_ring = true;
        std::string                              number;
        std::vector<std::shared_ptr<Stop>>       stops;
    };

    //--------------BusComparator----------------    

    struct BusComparator {
        bool               operator()(std::shared_ptr<Bus> lhs,
            std::shared_ptr<Bus> rhs) const;
    };

    using Buses = std::set<std::shared_ptr<Bus>, BusComparator>;

    //--------------StopComparator----------------    

    struct StopComparator {
        bool               operator()(std::shared_ptr<Stop> lhs,
            std::shared_ptr<Stop> rhs) const;
    };

    using Stops = std::set<std::shared_ptr<Stop>, StopComparator>;

    //--------------BusStat----------------    

    struct BusStat {
        uint64_t           count_stops = 0;
        uint64_t           count_unique_stops = 0;
        uint64_t           route_length = 0;
        double             curvature = 1;
    };

    //--------------HashPairStops----------------    

    struct HashPairStops {
    public:
        size_t                              operator() (const std::pair<std::shared_ptr<Stop>, std::shared_ptr<Stop>>& pair) const;
    private:
        std::hash<void*>                    p_hasher;
    };

}

namespace router::tr {

    //--------------Road----------------

    struct Road {
                           Road() = default;
        double             minutes = 0;
        std::string        name;
        int                span_count = 0;
    };

    Road operator+(const Road& lhs, const Road& rhs);

    bool operator<(const Road& lhs, const Road& rhs);

    bool operator>(const Road& lhs, const Road& rhs);

    //--------------Settings----------------

    struct Settings {
        int            bus_wait_time = 1;
        double         bus_velocity = 1;
    };

    struct Info {
        struct Wait {
            double         minutes = 0;
            std::string    stop_name;
        };

        struct Bus {
            double         minutes = 0;
            std::string    number;
            int            span_count = 0;
        };

        Wait       wait;
        Bus        bus;
    };

    struct ReportRouter {
        std::vector<Info>  information;
        double             total_minutes = 0;
    };
}