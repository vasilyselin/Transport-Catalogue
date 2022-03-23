/*

In this file defined class which fulfills database role.
The methods of the class verifying the validity of input data.
 

*/
#pragma once
#include "domain.h"
#include <set>
#include <deque>
#include <iostream>
#include <vector>
#include <algorithm>
#include <future>
#include <cassert>
#include <stdexcept>


class TransportCatalogue {
public:
    
    void AddStop(Stop&& stop_);   
    
    void AddRoute(const std::string& bus_name, bool is_circle, std::vector<Stop*> stops_);  

    const std::map<std::string_view, std::pair<Bus*, std::vector<Stop*>>>& GetRoutes() const;       
    
    const std::pair<Bus*, std::vector<Stop*>>& GetRoute(const std::string& num_) const;
    
    const std::set<std::string_view>& GetBuses (const std::string& stop_name) const;
    
    Stop* GetStop(const std::string& stop_name);    

private:
    std::map<std::string_view, std::pair<Bus*, std::vector<Stop*>>> routes_;
    std::unordered_map<std::string_view, std::pair<Stop*, std::set<std::string_view>>> buses_for_stop_;
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;    
};