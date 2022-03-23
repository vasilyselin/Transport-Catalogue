#include "transport_catalogue.h"
#include "map_renderer.h"
#include "geo.h"
#include "json_reader.h"
#include "request_handler.h"
#include <fstream>
#include <sstream>
#include <cstdio>

using namespace std;


int main() {
    TransportCatalogue catalogue;
    InputLoader loader(cin, catalogue);
    RequestHandler processor(catalogue, loader);
    processor.RequestExecution(cout);
}