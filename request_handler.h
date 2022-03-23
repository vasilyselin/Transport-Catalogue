#pragma once
#include "json_reader.h"
#include "json_builder.h"
#include "map_renderer.h"
#include "geo.h"
#include <iostream>
#include <unordered_set>

class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& db, const InputLoader& input);

    void RequestExecution(std::ostream& out);

    void PrintStopRequest (std::ostream& out, const json::Dict& req) const;

    void PrintBusRequest (std::ostream& out, const json::Dict& req) const;

private:
    const TransportCatalogue& db_;
    const InputLoader& input_;
    json::Node output;
};
