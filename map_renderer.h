#pragma once
#include "svg.h"
#include "transport_catalogue.h"


template <typename T>
void DrawAssist(svg::Document& doc, const std::vector<T>& obj) {    
    for(const auto& i : obj){
            i.Draw(doc);
    }     
}


class RouteLines : public svg::Drawable {
public:

    RouteLines(std::vector<std::pair<std::string*, svg::Point>>& coordinates, RenderSettings& settings, int color_counter) : coordinates_(coordinates),
                                                                                                  stroke_width_(settings.line_width),
                                                                                                  stroke_(settings.color_palette[color_counter]) {                                                                                          
                                                                                                  }

    void Draw(svg::ObjectContainer& container) const override;

private:
    std::vector<std::pair<std::string*, svg::Point>>& coordinates_;
    double stroke_width_;
    svg::Color stroke_;    
};


class RouteNames : public svg::Drawable {
public:

    RouteNames(const std::string& bus_name, svg::Point& coordinate, RenderSettings& settings, int color_counter) : bus_name_(bus_name), 
                                                                                                           stop_coordinates_(coordinate),
                                                                                                           offset_({settings.bus_label_offset.first, settings.bus_label_offset.second}),
                                                                                                           font_size_(settings.bus_label_font_size),
                                                                                                           fill_(settings.color_palette[color_counter]),
                                                                                                           underlayer_fill_(settings.underlayer_color),
                                                                                                           underlayer_stroke_(settings.underlayer_color),
                                                                                                           stroke_width_(settings.underlayer_width)                                                                                                           
                                                                                                           {} 
    void Draw(svg::ObjectContainer& container) const override;
    
    std::pair<const std::string&, svg::Point> GetName() const;

private:
    std::string bus_name_;
    svg::Point stop_coordinates_;
    svg::Point offset_;
    int font_size_;
    svg::Color fill_;
    svg::Color underlayer_fill_;
    svg::Color underlayer_stroke_;
    double stroke_width_;
};

class StopCircle : public svg::Drawable {
public:

    StopCircle(svg::Point* coordinates, RenderSettings& settings) : stop_coordinates_(coordinates), stop_radius_(settings.stop_radius) {}

    void Draw(svg::ObjectContainer& container) const override;

private:
    svg::Point* stop_coordinates_;
    double stop_radius_;
};

class StopName : public svg::Drawable {
public:
    StopName(const std::string& stop_name, svg::Point& coordinate, RenderSettings& settings) : stop_name_(stop_name), 
                                                                                                           stop_coordinates_(coordinate),                                                                                                           
                                                                                                           offset_({settings.stop_label_offset.first, settings.stop_label_offset.second}),
                                                                                                           font_size_(settings.stop_label_font_size),
                                                                                                           fill_("black"),
                                                                                                           underlayer_fill_(settings.underlayer_color),
                                                                                                           underlayer_stroke_(settings.underlayer_color),
                                                                                                           stroke_width_(settings.underlayer_width)                                                                                                           
                                                                                                           {} 
    void Draw(svg::ObjectContainer& container) const override;
private:
    std::string stop_name_;
    svg::Point stop_coordinates_;
    svg::Point offset_;
    int font_size_;
    svg::Color fill_;
    svg::Color underlayer_fill_;
    svg::Color underlayer_stroke_;
    double stroke_width_;
};



class MapRenderer {
public:
    MapRenderer(RenderSettings settings, const std::map<std::string_view, std::pair<Bus*, std::vector<Stop*>>>& routes) : settings_(settings),
                                                                                                        routes_(routes) 
    {
        all_stops_coordinates_ = NormalizeCoordinates(settings_);
    }

    void RenderMap(std::ostream& out, int id);


private:
    std::vector<std::vector<std::pair<std::string*, svg::Point>>> all_stops_coordinates_;
    RenderSettings settings_;
    const std::map<std::string_view, std::pair<Bus*, std::vector<Stop*>>>& routes_;
    std::vector<std::vector<std::pair<std::string*, svg::Point>>> NormalizeCoordinates(RenderSettings settings);
};