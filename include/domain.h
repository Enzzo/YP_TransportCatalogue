#pragma once
#include <string>
#include <vector>
#include <memory>
#include <set>

#include "geo.h"

struct Stop {
	std::string name;
	geo::Coordinates coord;
	bool free = true;
};

struct Bus {
	bool is_ring = true;
	std::string number;	
	std::vector<std::shared_ptr<Stop>> stops;
};

struct BusInfo {
	uint64_t stops = 0;
	uint64_t unique_stops = 0;
	uint64_t route_length = 0;
	double curvature = 1.0;
};

struct BusComparator {
	bool operator()(std::shared_ptr<Bus> lhs,
		std::shared_ptr<Bus> rhs) const {
		return lexicographical_compare(
			lhs->number.begin(), lhs->number.end(),
			rhs->number.begin(), rhs->number.end());
	}
};

struct StopComparator {
	bool operator()(std::shared_ptr<Stop> lhs,
		std::shared_ptr<Stop> rhs) const {
		return lexicographical_compare(
			lhs->name.begin(), lhs->name.end(),
			rhs->name.begin(), rhs->name.end());
	}
};

using Buses = std::set<std::shared_ptr<Bus>, BusComparator>;
using Stops = std::set<std::shared_ptr<Stop>, StopComparator>;

namespace detail {
	class DistanceHasher {
		std::hash<void*> p_hasher;
	public:
		size_t operator()(const std::pair<std::shared_ptr<Stop>, std::shared_ptr<Stop>>& p) const {
			size_t p_hash_first = p_hasher(p.first.get());
			size_t p_hash_second = p_hasher(p.second.get());
			return p_hash_first + p_hash_second * 10;
		}
	};
}

namespace tr {
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