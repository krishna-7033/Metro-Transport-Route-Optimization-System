#include "../FareCalculator.h"
#include "../FileManager.h"
#include "../RouteOptimizer.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

namespace {
int failures = 0;

void expect(bool condition, const std::string& description) {
    if (condition) {
        std::cout << "PASS: " << description << '\n';
    } else {
        std::cout << "FAIL: " << description << '\n';
        ++failures;
    }
}

bool closeTo(double actual, double expected) {
    return std::fabs(actual - expected) < 0.0001;
}

Graph makeRouteTestGraph() {
    Graph graph;
    for (const std::string& name : {"A", "B", "C", "D", "Island"}) {
        graph.addStation(name);
    }
    graph.addConnection("A", "B", 2.0, 20);
    graph.addConnection("B", "D", 2.0, 20);
    graph.addConnection("A", "C", 3.0, 5);
    graph.addConnection("C", "D", 3.0, 5);
    graph.addConnection("A", "D", 10.0, 60);
    return graph;
}

void testGraphValidation() {
    Graph graph;
    expect(graph.addStation("Alpha"), "add a station");
    expect(!graph.addStation("Alpha"), "reject duplicate station");
    expect(!graph.addStation(""), "reject empty station name");
    expect(!graph.addConnection("Alpha", "Missing", 1.0, 2),
           "reject connection with unknown endpoint");
    expect(graph.addStation("Beta"), "add second station");
    expect(!graph.addConnection("Alpha", "Beta", 0.0, 2),
           "reject zero distance");
    expect(!graph.addConnection("Alpha", "Beta", 1.0, 0),
           "reject zero travel time");
    expect(graph.addConnection("Alpha", "Beta", 1.0, 2),
           "add valid bidirectional connection");
    expect(!graph.addConnection("Alpha", "Beta", 1.0, 2),
           "reject duplicate connection");
    expect(graph.connectionCount() == 2,
           "bidirectional connection creates two adjacency entries");

    Graph directed;
    directed.addStation("One");
    directed.addStation("Two");
    expect(directed.addConnection("One", "Two", 1.0, 2, false),
           "add one-way connection");
    expect(directed.connectionCount() == 1,
           "one-way connection creates one adjacency entry");
}

void testRouteAlgorithms() {
    Graph graph = makeRouteTestGraph();
    RouteOptimizer optimizer(graph);

    const RouteResult distance = optimizer.shortestDistanceRoute("A", "D");
    expect(distance.found && distance.stopCount() == 2 && closeTo(distance.totalDistanceKm, 4.0),
           "Dijkstra selects minimum-distance route A-B-D");
    expect(distance.totalTravelTimeMinutes == 40,
           "distance route reports its actual travel time");

    const RouteResult fastest = optimizer.fastestRoute("A", "D");
    expect(fastest.found && fastest.stopCount() == 2 &&
               closeTo(fastest.totalDistanceKm, 6.0) &&
               fastest.totalTravelTimeMinutes == 10,
           "time-weighted Dijkstra selects fastest route A-C-D");

    const RouteResult stops = optimizer.minimumStopsRoute("A", "D");
    expect(stops.found && stops.stopCount() == 1 &&
               closeTo(stops.totalDistanceKm, 10.0),
           "BFS selects minimum-stop direct route A-D");

    expect(optimizer.isReachable("A", "D"), "DFS finds reachable destination");
    expect(!optimizer.isReachable("A", "Island"),
           "DFS rejects unreachable destination");

    const RouteResult sameStation = optimizer.fastestRoute("A", "A");
    expect(sameStation.found && sameStation.stopCount() == 0 &&
               closeTo(sameStation.totalDistanceKm, 0.0),
           "same source and destination returns zero-cost route");
    expect(!optimizer.fastestRoute("A", "Missing").found,
           "invalid station returns no route");
}

void testFareCalculator() {
    Graph graph = makeRouteTestGraph();
    RouteOptimizer optimizer(graph);
    const RouteResult route = optimizer.shortestDistanceRoute("A", "D");
    expect(closeTo(FareCalculator::calculateFare(route), 30.0),
           "fare uses INR 10 base plus INR 5 per kilometre");
    expect(closeTo(FareCalculator::calculateFare({}), 0.0),
           "unavailable route has zero fare");
}

void testFileManager() {
    const std::string stationFile = "/tmp/metro_route_test_stations.txt";
    const std::string connectionFile = "/tmp/metro_route_test_connections.txt";
    const std::string badStationFile = "/tmp/metro_route_bad_stations.txt";
    const std::string badConnectionFile = "/tmp/metro_route_bad_connections.txt";

    Graph original = makeRouteTestGraph();
    std::string error;
    expect(FileManager::saveNetwork(stationFile, connectionFile, original, error),
           "save valid network");

    Graph loaded;
    expect(FileManager::loadNetwork(stationFile, connectionFile, loaded, error),
           "load saved network");
    expect(loaded.stationCount() == original.stationCount() &&
               loaded.connectionCount() == original.connectionCount(),
           "saved network preserves station and connection counts");
    expect(RouteOptimizer(loaded).fastestRoute("A", "D").found,
           "saved network remains routable after loading");

    {
        std::FILE* stationOutput = std::fopen(badStationFile.c_str(), "w");
        std::FILE* connectionOutput = std::fopen(badConnectionFile.c_str(), "w");
        if (stationOutput != nullptr && connectionOutput != nullptr) {
            std::fputs("A\nB\n", stationOutput);
            std::fputs("A|B|not-a-number|5\n", connectionOutput);
        }
        if (stationOutput != nullptr) {
            std::fclose(stationOutput);
        }
        if (connectionOutput != nullptr) {
            std::fclose(connectionOutput);
        }
    }

    Graph preserved = original;
    expect(!FileManager::loadNetwork(badStationFile, badConnectionFile,
                                      preserved, error),
           "reject corrupted connection file");
    expect(preserved.stationCount() == original.stationCount() &&
               preserved.connectionCount() == original.connectionCount(),
           "corrupt load does not partially replace active graph");

    std::remove(stationFile.c_str());
    std::remove(connectionFile.c_str());
    std::remove(badStationFile.c_str());
    std::remove(badConnectionFile.c_str());
}
}

int main() {
    testGraphValidation();
    testRouteAlgorithms();
    testFareCalculator();
    testFileManager();

    if (failures == 0) {
        std::cout << "\nAll tests passed.\n";
        return 0;
    }
    std::cout << "\n" << failures << " test(s) failed.\n";
    return 1;
}
