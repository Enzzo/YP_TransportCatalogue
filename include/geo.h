#pragma once

namespace geo {

    struct Coordinates {
        double       lat = 0;
        double       lon = 0;
    };

    double ComputeDistance(Coordinates from, Coordinates to);

}

