#ifndef ROUTE_OPTIMIZER_H
#define ROUTE_OPTIMIZER_H

#include "Graph.h"

#include <vector>

struct RouteResult {
    bool found = false;
    std::vector<int> stationIds;
    double totalDistanceKm = 0.0;
    int totalTravelTimeMinutes = 0;

    int stopCount() const {
        return stationIds.empty() ? 0 : static_cast<int>(stationIds.size()) - 1;
    }
};

class RouteOptimizer {
public:
    explicit RouteOptimizer(const Graph& graph);

    RouteResult shortestDistanceRoute(const std::string& source,
                                      const std::string& destination) const;
    RouteResult fastestRoute(const std::string& source,
                             const std::string& destination) const;
    RouteResult minimumStopsRoute(const std::string& source,
                                  const std::string& destination) const;
    bool isReachable(const std::string& source,
                     const std::string& destination) const;

private:
    const Graph& graph_;

    RouteResult buildRoute(int sourceId,
                           int destinationId,
                           const std::vector<int>& parent) const;
};

#endif