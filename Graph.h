#ifndef GRAPH_H
#define GRAPH_H

#include "Station.h"

#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>

struct Connection {
    int destination;
    double distanceKm;
    int travelTimeMinutes;
};

class Graph {
public:
    bool addStation(const std::string& name);
    bool hasStation(const std::string& name) const;
    int getStationId(const std::string& name) const;

    bool addConnection(const std::string& source,
                       const std::string& destination,
                       double distanceKm,
                       int travelTimeMinutes,
                       bool bidirectional = true);

    void clear();

    const std::vector<Station>& getStations() const;
    const std::vector<std::vector<Connection>>& getAdjacencyList() const;
    std::size_t stationCount() const;
    std::size_t connectionCount() const;
    void display() const;

private:
    std::vector<Station> stations_;
    std::vector<std::vector<Connection>> adjacencyList_;
    std::unordered_map<std::string, int> stationIds_;

    bool connectionExists(int sourceId, int destinationId) const;
};

#endif