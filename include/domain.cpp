#include <algorithm>

#include "domain.h"

namespace router::tc {

    using namespace std;

    //--------------BusComparator----------------    

    bool BusComparator::operator()(shared_ptr<Bus> lhs,
        shared_ptr<Bus> rhs) const {
        return lexicographical_compare(
            lhs->number.begin(), lhs->number.end(),
            rhs->number.begin(), rhs->number.end());
    }

    //--------------StopComparator----------------    

    bool StopComparator::operator()(shared_ptr<Stop> lhs,
        shared_ptr<Stop> rhs) const {
        return lexicographical_compare(
            lhs->name.begin(), lhs->name.end(),
            rhs->name.begin(), rhs->name.end());
    }

    //--------------HashPairStops----------------    

    size_t HashPairStops::operator() (const pair<shared_ptr<Stop>, shared_ptr<Stop>>& pair) const {
        size_t p_hash_first = p_hasher(pair.first.get());
        size_t p_hash_second = p_hasher(pair.second.get());
        return p_hash_first + p_hash_second * 10;
    }

}

namespace router::tr {
    //--------------Road----------------

    Road operator+(const Road& lhs, const Road& rhs) {
        return { lhs.minutes + rhs.minutes, {}, {} };
    }

    bool operator<(const Road& lhs, const Road& rhs) {
        return lhs.minutes < rhs.minutes;
    }

    bool operator>(const Road& lhs, const Road& rhs) {
        return rhs < lhs;
    }

}