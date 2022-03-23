#include "request_handler.h"

using namespace std;


RequestHandler::RequestHandler(const TransportCatalogue& db, const InputLoader& input) : db_(db), input_(input) {}


double LengthCalcCircle(std::vector<Stop*> stops){
    double tot_dist = 0;     
    for(size_t i = 0; i < stops.size(); ++i){   
        auto comparator = [&] (auto& stop_dist){return stop_dist.first == (*stops[i]).name;};
        auto comparator2 = [&] (auto& stop_dist){return stop_dist.first == (*stops[i+1]).name;};         
        if(i == stops.size()-1) {
            if(count_if((*stops[i]).dist_to_stops.begin(), (*stops[i]).dist_to_stops.end(), comparator)>0){
                tot_dist += (*stops[i]).dist_to_stops [(*stops[i]).name];
            }
            continue;
        }
        if(count_if((*stops[i]).dist_to_stops.begin(), (*stops[i]).dist_to_stops.end(), comparator2)>0){
            tot_dist += (*stops[i]).dist_to_stops [(*stops[i+1]).name];
        }
        else if(count_if((*stops[i+1]).dist_to_stops.begin(), (*stops[i+1]).dist_to_stops.end(), comparator)>0){
            tot_dist += (*stops[i+1]).dist_to_stops [(*stops[i]).name];
        }
    }
    return tot_dist;       
}

double LengthCalcLine(std::vector<Stop*> stops){
    double tot_dist = 0; 
    for(size_t i = 0; i < stops.size()-1; ++i){
        auto comparator = [&] (auto& stop_dist){return stop_dist.first == (*stops[i]).name;};
        auto comparator2 = [&] (auto& stop_dist){return stop_dist.first == (*stops[i+1]).name;};
        if(count_if((*stops[i]).dist_to_stops.begin(), (*stops[i]).dist_to_stops.end(), comparator2)>0){
            tot_dist += (*stops[i]).dist_to_stops [(*stops[i+1]).name];
        }
        else if(count_if((*stops[i+1]).dist_to_stops.begin(), (*stops[i+1]).dist_to_stops.end(), comparator)>0){
            tot_dist += (*stops[i+1]).dist_to_stops [(*stops[i]).name];
        }            
    }
    for(int i = stops.size()-1; i > 0; --i){
        auto comparator = [&] (auto& stop_dist){return stop_dist.first == (*stops[i]).name;};
        auto comparator3 = [&] (auto& stop_dist){return stop_dist.first == (*stops[i-1]).name;}; 
        if(count_if((*stops[i]).dist_to_stops.begin(), (*stops[i]).dist_to_stops.end(), comparator3)>0){
            tot_dist += (*stops[i]).dist_to_stops [(*stops[i-1]).name];
        }
        else if(count_if((*stops[i-1]).dist_to_stops.begin(), (*stops[i-1]).dist_to_stops.end(), comparator)>0){
            tot_dist += (*stops[i-1]).dist_to_stops [(*stops[i]).name];
        }               
    }   
    return tot_dist; 
}  

double LengthCalcCircleGeo(std::vector<Stop*> stops){
    double tot_dist1 = 0; //for procesing of the first part of vector stops with async
    double tot_dist2 = 0; //for procesing of the second part of vector stops with async
    auto future_part = async([&stops, &tot_dist1]{
        for(size_t i = 0; i < stops.size()/2; ++i){
            if(i == stops.size()-1){
                continue;
            }
            tot_dist1 += geo::ComputeDistance({(*stops[i]).latitude, (*stops[i]).longitude}, {(*stops[i == stops.size()-1?0:i+1]).latitude, (*stops[i == stops.size()-1?0:i+1]).longitude});
        }
    });
    for(size_t i = stops.size()/2; i < stops.size(); ++i){
        if(i == stops.size()-1){
            continue;
        }			
        tot_dist2 += geo::ComputeDistance({(*stops[i]).latitude, (*stops[i]).longitude}, {(*stops[i == stops.size()-1?0:i+1]).latitude, (*stops[i == stops.size()-1?0:i+1]).longitude});
    }
    future_part.get();
    return tot_dist1 + tot_dist2;    
}

double LengthCalcLineGeo(std::vector<Stop*> stops){
    double tot_dist1 = 0; //for procesing of the first part of vector stops with async
    double tot_dist2 = 0; //for procesing of the second part of vector stops with async    
    auto future_part = async([&stops, &tot_dist1]{
        for(size_t i = 0; i < stops.size()-1; ++i){
            tot_dist1 += geo::ComputeDistance({(*stops[i]).latitude, (*stops[i]).longitude}, {(*stops[i+1]).latitude, (*stops[i+1]).longitude});
        }
    });
    for(size_t i = stops.size()-1; i > 0; --i){
        tot_dist2 += geo::ComputeDistance({(*stops[i]).latitude, (*stops[i]).longitude}, {(*stops[i-1]).latitude, (*stops[i-1]).longitude});
    }
    future_part.get();        
    return tot_dist1 + tot_dist2; 
}

void RequestHandler::PrintStopRequest (std::ostream& out, const json::Dict& req) const {
    try
    {    
        const auto buses = db_.GetBuses(req.at("name").AsString());
        out << '\n' << '\t' << '{' << '\n' << '\t' << '\t' << "\"buses\": [";
        size_t i = 1;
        for(const auto& bus : buses){
            out << '\n' << '\t' << '\t' << '\t';
            out << '"' << bus << '"';
            if(i<buses.size()){
                out << ", ";
            }
            else{
                out << '\n' << '\t' << '\t';
            }
            ++i;
        }
        out << "]," << '\n' <<  '\t' << '\t' << "\"request_id\": " << req.at("id").AsInt();
        out << '\n' << '\t' << '}';
    }
    catch(const std::exception& e)
    {
        out << '\n' << '\t' << '{' << '\n' << '\t' << '\t' << "\"error_message\": \"not found\"," << '\n';
        out << '\t' << '\t' << "\"request_id\": " << req.at("id").AsInt();
        out << '\n' << '\t' << '}';
    }

}

json::Node ProcessStopRequest(const json::Dict& req) {
    return json::Node{json::Dict{{"buses", json::Node{json::Array{req.at("name")}}},{"request_id", json::Node{req.at("id")}}}};
}

void RequestHandler::PrintBusRequest (std::ostream& out, const json::Dict& req) const {
    const auto& bus_name = req.at("name").AsString();
    try
    {
        const std::pair<Bus*, std::vector<Stop*>>& route = db_.GetRoute(bus_name);
        unordered_set<Stop*> clones;	
        for(auto k : route.second){ //selection of unique stops
            clones.insert(k);
        }
        if(route.first->type){
            double tot_dist = LengthCalcCircle(route.second);           
            double tot_dist_geo = LengthCalcCircleGeo(route.second); 

            json::Print(
                json::Document{
                    json::Builder{}
                    .StartDict()
                        .Key("curvature"s).Value(tot_dist/tot_dist_geo)
                        .Key("request_id"s).Value(req.at("id").AsInt())
                        .Key("route_length"s).Value(tot_dist)
                        .Key("stop_count"s).Value(static_cast<int>(route.second.size()))
                        .Key("unique_stop_count"s).Value(static_cast<int>(clones.size()))
                    .EndDict()
                    .Build()
                },
                out
            );
        }
        else{
            double tot_dist = LengthCalcLine(route.second);
            double tot_dist_geo = LengthCalcLineGeo(route.second);   
            json::Print(
                json::Document{
                    json::Builder{}
                    .StartDict()
                        .Key("curvature"s).Value(tot_dist/tot_dist_geo)
                        .Key("request_id"s).Value(req.at("id").AsInt())
                        .Key("route_length"s).Value(tot_dist)
                        .Key("stop_count"s).Value(static_cast<int>(route.second.size() * 2 - 1))
                        .Key("unique_stop_count"s).Value(static_cast<int>(clones.size()))
                    .EndDict()
                    .Build()
                },
                out
            );               
        }        
    }
    catch(const std::exception& e)
    {
        json::Print(
            json::Document{
                json::Builder{}
                .StartDict()
                    .Key("request_id"s).Value(req.at("id").AsInt())
                    .Key("error_message"s).Value("not found"s)
                .EndDict()
                .Build()
            },
            out
        ); 
    }
}

void RequestHandler::RequestExecution(std::ostream& out) {
    out << std::defaultfloat;
    out << "[";
    size_t i = 1;
    for(const auto& request : input_.GetStatRequests()){
        if(request.AsDict().at("type").AsString() == "Stop"){
            PrintStopRequest(out, request.AsDict());
        }
        else if(request.AsDict().at("type").AsString() == "Bus"){
            PrintBusRequest(out, request.AsDict());
        }
        else if(request.AsDict().at("type").AsString() == "Map"){
            MapRenderer renderer(input_.GetRenderSettings(), db_.GetRoutes());
            renderer.RenderMap(out, request.AsDict().at("id").AsInt());
        }
        
        if(i<input_.GetStatRequests().size()){
            out << ','; 
        }
        ++i;
    }
    out << '\n' << "]";
}
