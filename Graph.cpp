#include "Graph.h"

#include <iomanip>
#include <iostream>

bool Graph::addStation(const std::string& name) {
    if (name.empty() || hasStation(name)) {
        return false;
    }

    const int id = static_cast<int>(stations_.size());
    stations_.push_back({id, name});
    adjacencyList_.emplace_back();
    stationIds_[name] = id;
    return true;
}

void Graph::clear() {
    stations_.clear();
    adjacencyList_.clear();
    stationIds_.clear();
}

bool Graph::hasStation(const std::string& name) const {
    return stationIds_.find(name) != stationIds_.end();
}

int Graph::getStationId(const std::string& name) const {
    const auto iterator = stationIds_.find(name);
    return iterator == stationIds_.end() ? -1 : iterator->second;
}

bool Graph::connectionExists(int sourceId, int destinationId) const {
    for (const Connection& connection : adjacencyList_[sourceId]) {
        if (connection.destination == destinationId) {
            return true;
        }
    }
    return false;
}

bool Graph::addConnection(const std::string& source,
                          const std::string& destination,
                          double distanceKm,
                          int travelTimeMinutes,
                          bool bidirectional) {
    const int sourceId = getStationId(source);
    const int destinationId = getStationId(destination);

    if (sourceId == -1 || destinationId == -1 || sourceId == destinationId ||
        distanceKm <= 0.0 || travelTimeMinutes <= 0 ||
        connectionExists(sourceId, destinationId)) {
        return false;
    }

    adjacencyList_[sourceId].push_back(
        {destinationId, distanceKm, travelTimeMinutes});

    if (bidirectional) {
        adjacencyList_[destinationId].push_back(
            {sourceId, distanceKm, travelTimeMinutes});
    }
    return true;
}

const std::vector<Station>& Graph::getStations() const {
    return stations_;
}

const std::vector<std::vector<Connection>>& Graph::getAdjacencyList() const {
    return adjacencyList_;
}

std::size_t Graph::stationCount() const {
    return stations_.size();
}

std::size_t Graph::connectionCount() const {
    std::size_t directedConnectionCount = 0;
    for (const auto& connections : adjacencyList_) {
        directedConnectionCount += connections.size();
    }
    return directedConnectionCount;
}

void Graph::display() const {
    std::cout << "\nTransport network (adjacency list)\n";
    for (const Station& station : stations_) {
        std::cout << station.name << " -> ";
        for (const Connection& connection : adjacencyList_[station.id]) {
            std::cout << stations_[connection.destination].name
                      << " [" << std::fixed << std::setprecision(1)
                      << connection.distanceKm << " km, "
                      << connection.travelTimeMinutes << " min] ";
        }
        std::cout << '\n';
    }
}