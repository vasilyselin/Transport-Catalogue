#include "json_reader.h"

using namespace std;
using namespace literals;



InputLoader::InputLoader(istream& input, TransportCatalogue& catalogue) : doc_(json::Load(input)),
                                                                          catalogue_(catalogue)   {
    InputLoader::LoadStops();
    InputLoader::LoadBusesAndDistances();
}
    
void InputLoader::LoadStops() const {
    const json::Array& data_in = doc_.GetRoot().AsDict().at("base_requests").AsArray();    
    for(auto& request : data_in){
        if(request.AsDict().at("type").AsString() == "Stop"sv){            
            unordered_map<string_view, double> dist_to_stops;
            catalogue_.AddStop({request.AsDict().at("name").AsString(), 
                               request.AsDict().at("latitude").AsDouble(), 
                               request.AsDict().at("longitude").AsDouble(), move(dist_to_stops)});
        }
    }
}

void InputLoader::LoadBusesAndDistances() const {
    const json::Array& data_in = doc_.GetRoot().AsDict().at("base_requests").AsArray();
    for(auto& request : data_in){
        if(request.AsDict().at("type").AsString() == "Stop"sv && !request.AsDict().at("road_distances").AsDict().empty()){            
            auto orig_stop = catalogue_.GetStop(request.AsDict().at("name").AsString());
            for(const auto& dest_stop_dist : request.AsDict().at("road_distances").AsDict()){
                auto dest_stop = catalogue_.GetStop(dest_stop_dist.first);
                orig_stop->dist_to_stops.insert({dest_stop->name, dest_stop_dist.second.AsDouble()}); 
            }
        }
        if(request.AsDict().at("type").AsString() == "Bus"sv){
            vector<Stop*> stops;
            for(auto& stop : request.AsDict().at("stops").AsArray()){
                stops.push_back(catalogue_.GetStop(stop.AsString()));
            }
            catalogue_.AddRoute(request.AsDict().at("name").AsString(), request.AsDict().at("is_roundtrip").AsBool(), move(stops));
        }            
    }
}

const json::Array& InputLoader::GetStatRequests() const {
    return doc_.GetRoot().AsDict().at("stat_requests").AsArray();
}

RenderSettings InputLoader::GetRenderSettings() const {
    RenderSettings settings;
    auto& raw = doc_.GetRoot().AsDict().at("render_settings").AsDict();    
    settings.bus_label_font_size = raw.at("bus_label_font_size").AsDouble();
    settings.bus_label_offset = {raw.at("bus_label_offset").AsArray()[0].AsDouble(), 
                                 raw.at("bus_label_offset").AsArray()[1].AsDouble()};
    for(auto i : raw.at("color_palette").AsArray()){
        if(i.IsString()){
            settings.color_palette.push_back(i.AsString());
        }
        else if(i.IsArray() && i.AsArray().size() == 3){
            settings.color_palette.push_back(svg::Rgb{i.AsArray()[0].AsInt(), 
                                                      i.AsArray()[1].AsInt(), 
                                                      i.AsArray()[2].AsInt()});
        }
        else if(i.IsArray() && i.AsArray().size() == 4){
            settings.color_palette.push_back(svg::Rgba{static_cast<uint8_t>(i.AsArray()[0].AsInt()), 
                                                       static_cast<uint8_t>(i.AsArray()[1].AsInt()), 
                                                       static_cast<uint8_t>(i.AsArray()[2].AsInt()), 
                                                       i.AsArray()[3].AsDouble()});
        }
    }
    settings.height = raw.at("height").AsDouble();                                 
    settings.line_width = raw.at("line_width").AsDouble();
    settings.padding = raw.at("padding").AsDouble();
    settings.stop_label_font_size = raw.at("stop_label_font_size").AsDouble();
    settings.stop_label_offset = {raw.at("stop_label_offset").AsArray()[0].AsDouble(), 
                                  raw.at("stop_label_offset").AsArray()[1].AsDouble()};
    settings.stop_radius = raw.at("stop_radius").AsDouble();
    if(raw.at("underlayer_color").IsString()){
        settings.underlayer_color = raw.at("underlayer_color").AsString();
    }
    else if(raw.at("underlayer_color").IsArray() && raw.at("underlayer_color").AsArray().size() == 3){
        settings.underlayer_color = svg::Rgb{raw.at("underlayer_color").AsArray()[0].AsInt(),
                                             raw.at("underlayer_color").AsArray()[1].AsInt(), 
                                             raw.at("underlayer_color").AsArray()[2].AsInt()}; 
    }  
    else if(raw.at("underlayer_color").IsArray() && raw.at("underlayer_color").AsArray().size() == 4){
        settings.underlayer_color = svg::Rgba{static_cast<uint8_t>(raw.at("underlayer_color").AsArray()[0].AsDouble()),
                                             static_cast<uint8_t>(raw.at("underlayer_color").AsArray()[1].AsDouble()), 
                                             static_cast<uint8_t>(raw.at("underlayer_color").AsArray()[2].AsDouble()), 
                                             raw.at("underlayer_color").AsArray()[3].AsDouble()}; 
    }      
    settings.underlayer_width = raw.at("underlayer_width").AsDouble();
    settings.width = raw.at("width").AsDouble();
    return settings;
}