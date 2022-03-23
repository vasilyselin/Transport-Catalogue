/*
In this file geographic related structure and
function for distance calculation between two stops

*/

#pragma once

namespace geo {

struct Coordinates {
    double lat; // latitude
    double lng; // longtitude
};

double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo