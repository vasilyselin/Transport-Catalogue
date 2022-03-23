#include "domain.h"

bool operator==(Bus& lhs, Bus& rhs) {
    return lhs.busnum == rhs.busnum;
}

bool operator==(const Bus& lhs, const Bus& rhs) {
    return lhs.busnum == rhs.busnum;
} 

bool operator<(Bus& lhs, Bus& rhs) {
    return lhs.busnum < rhs.busnum;
}

bool operator<(const Bus& lhs, const Bus& rhs){
    return lhs.busnum < rhs.busnum;
}

size_t BusHasher::operator()(const Bus& bus) const {
    return hasher(bus.busnum);
}

size_t BusPtrHasher::operator()(const Bus* bus) const {
    return hasher((*bus).busnum);
}

bool BusComp::operator()(const Bus* lhs, const Bus* rhs) const {
    return lhs->busnum < rhs->busnum ;
}

bool BusMapComp::operator()(const Bus& lhs, const Bus& rhs) const {
    return lhs.busnum < rhs.busnum ;
}

bool StopMapComp::operator()(const std::string* lhs, const std::string* rhs) const {
    return *lhs < *rhs ;
}
