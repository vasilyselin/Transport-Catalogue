/*
In this file defined main working objects:
    - RenderSettings
    - Stop
    - Bus

*/

#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <string_view>
#include <memory>
#include "svg.h"

struct RenderSettings {
    double width;
    double height;
    double padding;
    double line_width;
    double stop_radius;
    int bus_label_font_size;
    std::pair<double, double> bus_label_offset;
    int stop_label_font_size;
    std::pair<double, double> stop_label_offset;
    std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba> underlayer_color;
    double underlayer_width;
    std::vector<std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>> color_palette;
};

struct Stop {
    bool operator==(Stop& other){
        return name == other.name;
    }    
    std::string name;
    double latitude; // coordinates of stop "name"
    double longitude; // coordinates of stop "name"
    std::unordered_map<std::string_view, double> dist_to_stops;	// contain distances from the stop "name" to other stops.
};

struct Bus {       
    std::string busnum;   
    bool type; // if circle route - type = 1; type = 0 for non circle route
};

bool operator==(Bus& lhs, Bus& rhs);

bool operator==(const Bus& lhs, const Bus& rhs);

bool operator<(Bus& lhs, Bus& rhs);

bool operator<(const Bus& lhs, const Bus& rhs);



struct BusHasher {
    size_t operator()(const Bus& bus) const;
    std::hash<std::string> hasher;
};

struct BusPtrHasher {
    size_t operator()(const Bus* bus) const;
    std::hash<std::string> hasher;
};

struct BusComp {
    bool operator()(const Bus* lhs, const Bus* rhs) const;
};

struct BusMapComp {
    bool operator()(const Bus& lhs, const Bus& rhs) const;
};

struct StopMapComp {
    bool operator()(const std::string* lhs, const std::string* rhs) const;
};