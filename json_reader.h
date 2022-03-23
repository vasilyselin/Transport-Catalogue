#pragma once
#include "json.h"
#include "transport_catalogue.h"

class InputLoader {
public:
    InputLoader(std::istream& input, TransportCatalogue& catalogue);
    const json::Array& GetStatRequests() const;
    RenderSettings GetRenderSettings() const;    
private:    
    json::Document doc_;
    TransportCatalogue& catalogue_;
    void LoadStops() const;
    void LoadBusesAndDistances() const;

};