#include "RouteOptimizer.h"

#include <functional>
#include <limits>
#include <queue>
#include <stack>

namespace {
constexpr double kInfinity = std::numeric_limits<double>::infinity();
}

RouteOptimizer::RouteOptimizer(const Graph& graph) : graph_(graph) {}

RouteResult RouteOptimizer::buildRoute(int sourceId,
                                       int destinationId,
                                       const std::vector<int>& parent) const {
    if (sourceId < 0 || destinationId < 0 ||
        sourceId >= static_cast<int>(graph_.stationCount()) ||
        destinationId >= static_cast<int>(graph_.stationCount())) {
        return {};
    }

    std::vector<int> reversedPath;
    for (int current = destinationId; current != -1; current = parent[current]) {
        reversedPath.push_back(current);
    }

    if (reversedPath.back() != sourceId) {
        return {};
    }

    RouteResult result;
    result.found = true;
    result.stationIds.assign(reversedPath.rbegin(), reversedPath.rend());

    const auto& adjacencyList = graph_.getAdjacencyList();
    for (std::size_t index = 1; index < result.stationIds.size(); ++index) {
        const int previous = result.stationIds[index - 1];
        const int current = result.stationIds[index];
        for (const Connection& connection : adjacencyList[previous]) {
            if (connection.destination == current) {
                result.totalDistanceKm += connection.distanceKm;
                result.totalTravelTimeMinutes += connection.travelTimeMinutes;
                break;
            }
        }
    }
    return result;
}

RouteResult RouteOptimizer::shortestDistanceRoute(
    const std::string& source, const std::string& destination) const {
    const int sourceId = graph_.getStationId(source);
    const int destinationId = graph_.getStationId(destination);
    if (sourceId == -1 || destinationId == -1) {
        return {};
    }

    const int stationCount = static_cast<int>(graph_.stationCount());
    std::vector<double> distance(stationCount, kInfinity);
    std::vector<int> parent(stationCount, -1);
    std::priority_queue<std::pair<double, int>,
                        std::vector<std::pair<double, int>>,
                        std::greater<std::pair<double, int>>>
        pending;

    distance[sourceId] = 0.0;
    pending.push({0.0, sourceId});

    while (!pending.empty()) {
        const auto [currentDistance, current] = pending.top();
        pending.pop();
        if (currentDistance > distance[current]) {
            continue;
        }
        if (current == destinationId) {
            break;
        }

        for (const Connection& connection : graph_.getAdjacencyList()[current]) {
            const double newDistance = currentDistance + connection.distanceKm;
            if (newDistance < distance[connection.destination]) {
                distance[connection.destination] = newDistance;
                parent[connection.destination] = current;
                pending.push({newDistance, connection.destination});
            }
        }
    }
    return buildRoute(sourceId, destinationId, parent);
}

RouteResult RouteOptimizer::fastestRoute(const std::string& source,
                                          const std::string& destination) const {
    const int sourceId = graph_.getStationId(source);
    const int destinationId = graph_.getStationId(destination);
    if (sourceId == -1 || destinationId == -1) {
        return {};
    }

    const int stationCount = static_cast<int>(graph_.stationCount());
    std::vector<int> travelTime(stationCount, std::numeric_limits<int>::max());
    std::vector<int> parent(stationCount, -1);
    std::priority_queue<std::pair<int, int>,
                        std::vector<std::pair<int, int>>,
                        std::greater<std::pair<int, int>>>
        pending;

    travelTime[sourceId] = 0;
    pending.push({0, sourceId});

    while (!pending.empty()) {
        const auto [currentTime, current] = pending.top();
        pending.pop();
        if (currentTime > travelTime[current]) {
            continue;
        }
        if (current == destinationId) {
            break;
        }

        for (const Connection& connection : graph_.getAdjacencyList()[current]) {
            const int newTime = currentTime + connection.travelTimeMinutes;
            if (newTime < travelTime[connection.destination]) {
                travelTime[connection.destination] = newTime;
                parent[connection.destination] = current;
                pending.push({newTime, connection.destination});
            }
        }
    }
    return buildRoute(sourceId, destinationId, parent);
}

RouteResult RouteOptimizer::minimumStopsRoute(
    const std::string& source, const std::string& destination) const {
    const int sourceId = graph_.getStationId(source);
    const int destinationId = graph_.getStationId(destination);
    if (sourceId == -1 || destinationId == -1) {
        return {};
    }

    std::vector<bool> visited(graph_.stationCount(), false);
    std::vector<int> parent(graph_.stationCount(), -1);
    std::queue<int> pending;
    visited[sourceId] = true;
    pending.push(sourceId);

    while (!pending.empty()) {
        const int current = pending.front();
        pending.pop();
        if (current == destinationId) {
            break;
        }

        for (const Connection& connection : graph_.getAdjacencyList()[current]) {
            if (!visited[connection.destination]) {
                visited[connection.destination] = true;
                parent[connection.destination] = current;
                pending.push(connection.destination);
            }
        }
    }
    return buildRoute(sourceId, destinationId, parent);
}

bool RouteOptimizer::isReachable(const std::string& source,
                                 const std::string& destination) const {
    const int sourceId = graph_.getStationId(source);
    const int destinationId = graph_.getStationId(destination);
    if (sourceId == -1 || destinationId == -1) {
        return false;
    }
    if (sourceId == destinationId) {
        return true;
    }

    std::vector<bool> visited(graph_.stationCount(), false);
    std::stack<int> pending;
    pending.push(sourceId);
    visited[sourceId] = true;

    while (!pending.empty()) {
        const int current = pending.top();
        pending.pop();
        if (current == destinationId) {
            return true;
        }
        for (const Connection& connection : graph_.getAdjacencyList()[current]) {
            if (!visited[connection.destination]) {
                visited[connection.destination] = true;
                pending.push(connection.destination);
            }
        }
    }
    return false;
}