#include "transport_catalogue.h"

using namespace std;


void TransportCatalogue::AddStop(Stop&& stop){
    stops_.push_back(move(stop));
    std::set<std::string_view> buses_dummy;
    buses_for_stop_.insert({stops_.back().name, {&stops_.back(), buses_dummy}});
}
    
void TransportCatalogue::AddRoute (const std::string& bus_name, bool is_circle, std::vector<Stop*> stops_){
    buses_.push_back({bus_name, is_circle});
    assert(buses_.size() != 0);    
    if(routes_.count(buses_.back().busnum) == 0){
        routes_.insert({buses_.back().busnum, {&buses_.back(), stops_}});
    }
    for(auto i : routes_.at(buses_.back().busnum).second){
        buses_for_stop_.at(i->name).second.insert(buses_.back().busnum);
    }
    return;
}

const std::pair<Bus*, std::vector<Stop*>>& TransportCatalogue::GetRoute (const std::string& num_) const {        
    if(routes_.count(num_) == 0){
        throw invalid_argument("Bus does not exist");
    }
    return routes_.at(num_);    
}

const std::map<std::string_view, std::pair<Bus*, std::vector<Stop*>>>& TransportCatalogue::GetRoutes() const{
    return routes_;
}

const std::set<std::string_view>& TransportCatalogue::GetBuses (const std::string& stop_name) const {
    if(buses_for_stop_.count(stop_name) == 0){
        throw invalid_argument("Stop does not exist");
    }    
    return buses_for_stop_.at(stop_name).second;
}

Stop* TransportCatalogue::GetStop(const string& stop_name)  {
    if(buses_for_stop_.count(stop_name) == 0){
        throw invalid_argument("Stop does not exist");
    } 
    auto res = buses_for_stop_.at(stop_name).first;              
    return res;
}