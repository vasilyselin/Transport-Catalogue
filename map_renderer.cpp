#include "map_renderer.h"


void RouteLines::Draw(svg::ObjectContainer& container) const {
    svg::Polyline route;
    for(auto& i : coordinates_){  
        route.AddPoint(i.second);
    }
    svg::Color tmp = stroke_; 
    double width = stroke_width_;    
    route.SetFillColor("none").SetStrokeColor(std::move(tmp)).SetStrokeLineCap(svg::StrokeLineCap::ROUND)
         .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeWidth(std::move(width));
    container.Add(std::move(route));
}

void RouteNames::Draw(svg::ObjectContainer& container) const {
    svg::Text name, underlayer;
    svg::Color name_fill = fill_;     
    name.SetData(bus_name_).SetPosition(stop_coordinates_).SetOffset(offset_).SetFontSize(font_size_)
        .SetFontFamily("Verdana").SetFontWeight("bold").SetFillColor(std::move(name_fill));
    svg::Color underlayer_fill = underlayer_fill_;
    svg::Color underlayer_stroke = underlayer_stroke_;
    underlayer.SetData(bus_name_).SetPosition(stop_coordinates_).SetOffset(offset_).SetFontSize(font_size_)
        .SetFontFamily("Verdana").SetFontWeight("bold").SetFillColor(std::move(underlayer_fill)).SetStrokeColor(std::move(underlayer_stroke))
        .SetStrokeWidth(stroke_width_).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);  
    container.Add(std::move(underlayer));
    container.Add(std::move(name));
}

std::pair<const std::string&, svg::Point> RouteNames::GetName() const{
    return {bus_name_, stop_coordinates_};
}

void StopName::Draw(svg::ObjectContainer& container) const {
    svg::Text name, underlayer;
    svg::Color name_fill = fill_;     
    name.SetData(stop_name_).SetPosition(stop_coordinates_).SetOffset(offset_).SetFontSize(font_size_)
        .SetFontFamily("Verdana").SetFillColor(std::move(name_fill));
    svg::Color underlayer_fill = underlayer_fill_;
    svg::Color underlayer_stroke = underlayer_stroke_;
    underlayer.SetData(stop_name_).SetPosition(stop_coordinates_).SetOffset(offset_).SetFontSize(font_size_)
        .SetFontFamily("Verdana").SetFillColor(std::move(underlayer_fill)).SetStrokeColor(std::move(underlayer_stroke))
        .SetStrokeWidth(stroke_width_).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);  
    container.Add(std::move(underlayer));
    container.Add(std::move(name));
}

void StopCircle::Draw(svg::ObjectContainer& container) const {
    svg::Circle circle;
    circle.SetCenter(*stop_coordinates_).SetRadius(stop_radius_).SetFillColor("white");
    container.Add(std::move(circle));
}

std::vector<std::vector<std::pair<std::string*, svg::Point>>> MapRenderer::NormalizeCoordinates(RenderSettings settings) {
    std::vector<std::vector<std::pair<std::string*, svg::Point>>> res;
    double min_lat = 1e20, min_lon = 1e20, max_lat = -1e20, max_lon = -1e20;  
    for(const auto& i : routes_){
        for(const auto& j : i.second.second){
            min_lat = std::min(min_lat, j->latitude);
            min_lon = std::min(min_lon, j->longitude);
            max_lat = std::max(max_lat, j->latitude);
            max_lon = std::max(max_lon, j->longitude);
        }        
    }  
    double width_zoom_coef = 0, height_zoom_coef = 0, zoom_coef = 0;
    if(min_lat == max_lat) {
        zoom_coef = (settings.width - 2 * settings.padding) / (max_lon - min_lon);
    }
    else if(min_lon == max_lon){
        zoom_coef = (settings.height - 2 * settings.padding) / (max_lat - min_lat);
    }
    else if(min_lon == max_lon && min_lat == max_lat) {
        zoom_coef = 0;
    }
    else{
        width_zoom_coef  = (settings.width - 2 * settings.padding) / (max_lon - min_lon);
        height_zoom_coef = (settings.height - 2 * settings.padding) / (max_lat - min_lat);
        zoom_coef = std::min(width_zoom_coef, height_zoom_coef);
    }
    for(auto& i : routes_){
        std::vector<std::pair<std::string*, svg::Point>> tmp;
        for(auto& j : i.second.second){
            tmp.push_back({&(j->name), {(j->longitude - min_lon) * zoom_coef + settings.padding, (max_lat - j->latitude) * zoom_coef + settings.padding}});
        }
        if(!i.second.first->type){
            auto it = i.second.second.rbegin();
            ++it;
            while(it != i.second.second.rend()){
                tmp.push_back ({ &(*it)->name, {((*it)->longitude - min_lon) * zoom_coef + settings.padding, 
                                                (max_lat - (*it)->latitude) * zoom_coef + settings.padding}});
                ++it;
            }             
        }
        res.push_back(move(tmp));
    }
    return res;
}



void MapRenderer::RenderMap(std::ostream& out, int id) {    
    std::vector<RouteLines> routes_map;
    std::vector<RouteNames> route_names;
    std::vector<StopCircle> circles;
    std::vector<StopName> stop_names;
    std::map<std::string*, svg::Point*, StopMapComp> all_stops_lexic_map;
    std::vector<std::pair<std::string*, svg::Point*>> all_stops_lexic;
    int color_counter = 0;    
    auto it_bus = routes_.begin();
    for(auto it_coordinates = all_stops_coordinates_.begin(); it_coordinates != all_stops_coordinates_.end(); ++it_coordinates){
        routes_map.push_back(RouteLines(*it_coordinates, settings_, color_counter));
        route_names.push_back(RouteNames(it_bus->second.first->busnum, (*it_coordinates)[0].second, settings_, color_counter));
        if(find_if(route_names.begin(), route_names.end(), [&](const RouteNames& obj)
                                {
                                    return obj.GetName().first == it_bus->second.first->busnum &&
                                           obj.GetName().second.x == (*it_coordinates)[it_coordinates->size()/2].second.x &&
                                           obj.GetName().second.y == (*it_coordinates)[it_coordinates->size()/2].second.y;
                                }) == route_names.end())
        {
            if(!it_bus->second.first->type){
                route_names.push_back(RouteNames(it_bus->second.first->busnum, (*it_coordinates)[it_coordinates->size()/2].second, settings_, color_counter));
            }            
        }
        int limit = it_coordinates->size()-1;
        if(!it_bus->second.first->type){            
            limit = it_coordinates->size()/2+1;
        }
        for(int i = 0; i<limit; ++i){
            all_stops_lexic_map.insert({(*it_coordinates)[i].first, &((*it_coordinates)[i].second)});
            all_stops_lexic.push_back({(*it_coordinates)[i].first, &((*it_coordinates)[i].second)});
        }
        ++it_bus;
        if(color_counter == static_cast<int>(settings_.color_palette.size()-1)){
            color_counter = 0;
            continue;            
        }             
        ++color_counter;        
    }
    for(auto& stop : all_stops_lexic_map){
        circles.push_back(StopCircle(stop.second, settings_));
        stop_names.push_back(StopName(*(stop.first), *(stop.second), settings_));        

    }                                                         
    svg::Document doc(id);
    DrawAssist(doc, routes_map);
    DrawAssist(doc, route_names);
    DrawAssist(doc, circles);
    DrawAssist(doc, stop_names);    
    doc.Render(out);
}
