#include "FareCalculator.h"
#include "FileManager.h"
#include "RouteOptimizer.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {
const std::string kStationFile = "data/stations.txt";
const std::string kConnectionFile = "data/connections.txt";

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

bool parseInteger(const std::string& text, int& value) {
    std::stringstream stream(text);
    char extra = '\0';
    return (stream >> value) && !(stream >> extra);
}

bool parseDouble(const std::string& text, double& value) {
    std::stringstream stream(text);
    char extra = '\0';
    return (stream >> value) && !(stream >> extra);
}

int readInteger(const std::string& prompt) {
    while (true) {
        int value = 0;
        if (parseInteger(readLine(prompt), value)) {
            return value;
        }
        std::cout << "Please enter a valid whole number.\n";
    }
}

double readPositiveDouble(const std::string& prompt) {
    while (true) {
        double value = 0.0;
        if (parseDouble(readLine(prompt), value) && value > 0.0) {
            return value;
        }
        std::cout << "Please enter a positive number.\n";
    }
}

bool resolveStationInput(const Graph& graph,
                         const std::string& input,
                         std::string& stationName) {
    if (graph.hasStation(input)) {
        stationName = input;
        return true;
    }

    int stationId = -1;
    if (!parseInteger(input, stationId) || stationId < 0 ||
        stationId >= static_cast<int>(graph.stationCount())) {
        return false;
    }

    stationName = graph.getStations()[stationId].name;
    return true;
}

std::string readStation(const Graph& graph, const std::string& prompt) {
    while (true) {
        const std::string input = readLine(prompt);
        std::string stationName;
        if (resolveStationInput(graph, input, stationName)) {
            return stationName;
        }
        std::cout << "Station not found. Enter its exact name or listed ID.\n";
    }
}

void printStations(const Graph& graph) {
    if (graph.getStations().empty()) {
        std::cout << "No stations available.\n";
        return;
    }
    std::cout << "\nStations:\n";
    for (const Station& station : graph.getStations()) {
        std::cout << "  " << station.id << ". " << station.name << '\n';
    }
}

void printRoute(const Graph& graph, const RouteResult& result) {
    if (!result.found) {
        std::cout << "No route found. Check station names and connectivity.\n";
        return;
    }

    const auto& stations = graph.getStations();
    std::cout << "Route: ";
    for (std::size_t index = 0; index < result.stationIds.size(); ++index) {
        if (index > 0) {
            std::cout << " -> ";
        }
        std::cout << stations[result.stationIds[index]].name;
    }
    std::cout << "\nDistance: " << std::fixed << std::setprecision(1)
              << result.totalDistanceKm << " km\nTravel time: "
              << result.totalTravelTimeMinutes << " minutes\nConnections: "
              << result.stopCount() << '\n';
}

RouteResult chooseRoute(const RouteOptimizer& optimizer, int option,
                        const std::string& source,
                        const std::string& destination) {
    if (option == 5) {
        return optimizer.shortestDistanceRoute(source, destination);
    }
    if (option == 6) {
        return optimizer.fastestRoute(source, destination);
    }
    return optimizer.minimumStopsRoute(source, destination);
}

void showRoute(const Graph& graph, const RouteOptimizer& optimizer, int option) {
    const std::string source = readStation(graph, "Source station (name or ID): ");
    const std::string destination =
        readStation(graph, "Destination station (name or ID): ");
    printRoute(graph, chooseRoute(optimizer, option, source, destination));
}

void printMenu() {
    std::cout << "\n=== Metro Route Optimization System ===\n"
              << "1. View all stations\n"
              << "2. Search for a station\n"
              << "3. Add a station\n"
              << "4. Add a connection\n"
              << "5. Find shortest-distance route\n"
              << "6. Find fastest route\n"
              << "7. Find minimum-stop route\n"
              << "8. Display connectivity graph and check reachability\n"
              << "9. Calculate estimated fare\n"
              << "10. Display network statistics\n"
              << "11. Save/load network data\n"
              << "12. Exit\n";
}
}

int main() {
    Graph network;
    std::string errorMessage;
    if (!FileManager::loadNetwork(kStationFile, kConnectionFile, network,
                                  errorMessage)) {
        std::cout << "Network load warning: " << errorMessage
                  << "\nStarting with an empty network.\n";
    }

    while (true) {
        printMenu();
        const int option = readInteger("Choose an option: ");
        if (!std::cin) {
            std::cout << "\nInput ended. Exiting.\n";
            break;
        }

        if (option == 1) {
            printStations(network);
        } else if (option == 2) {
            const std::string name = readStation(network, "Station name or ID: ");
            if (network.hasStation(name)) {
                std::cout << "Station exists with ID " << network.getStationId(name)
                          << ".\n";
            }
        } else if (option == 3) {
            const std::string name = readLine("New station name: ");
            std::cout << (network.addStation(name) ? "Station added.\n"
                                                   : "Station rejected.\n");
        } else if (option == 4) {
            const std::string source =
                readStation(network, "Source station (name or ID): ");
            const std::string destination =
                readStation(network, "Destination station (name or ID): ");
            const double distanceKm = readPositiveDouble("Distance (km): ");
            const int travelTime = readInteger("Travel time (minutes): ");
            const std::string direction = readLine("Bidirectional? (y/n): ");
            const bool bidirectional = direction == "y" || direction == "Y";
            if (travelTime <= 0) {
                std::cout << "Travel time must be positive.\n";
            } else if (network.addConnection(source, destination, distanceKm,
                                             travelTime, bidirectional)) {
                std::cout << "Connection added.\n";
            } else {
                std::cout << "Connection rejected. Check names, weights, or duplicates.\n";
            }
        } else if (option >= 5 && option <= 7) {
            RouteOptimizer optimizer(network);
            showRoute(network, optimizer, option);
        } else if (option == 8) {
            network.display();
            RouteOptimizer optimizer(network);
            const std::string source =
                readStation(network, "Source station (name or ID): ");
            const std::string destination =
                readStation(network, "Destination station (name or ID): ");
            std::cout << (optimizer.isReachable(source, destination)
                              ? "Destination is reachable.\n"
                              : "Destination is not reachable.\n");
        } else if (option == 9) {
            RouteOptimizer optimizer(network);
            const std::string source =
                readStation(network, "Source station (name or ID): ");
            const std::string destination =
                readStation(network, "Destination station (name or ID): ");
            const RouteResult route =
                optimizer.shortestDistanceRoute(source, destination);
            if (!route.found) {
                std::cout << "Fare cannot be calculated because no route exists.\n";
            } else {
                std::cout << "Estimated fare: INR " << std::fixed
                          << std::setprecision(2)
                          << FareCalculator::calculateFare(route) << '\n';
            }
        } else if (option == 10) {
            std::cout << "Stations: " << network.stationCount() << '\n'
                      << "Stored directed connections: "
                      << network.connectionCount() << '\n';
        } else if (option == 11) {
            const std::string action = readLine("Enter 'save' or 'load': ");
            if (action == "save") {
                if (FileManager::saveNetwork(kStationFile, kConnectionFile,
                                             network, errorMessage)) {
                    std::cout << "Network saved.\n";
                } else {
                    std::cout << "Save failed: " << errorMessage << '\n';
                }
            } else if (action == "load") {
                if (FileManager::loadNetwork(kStationFile, kConnectionFile,
                                             network, errorMessage)) {
                    std::cout << "Network loaded.\n";
                } else {
                    std::cout << "Load failed: " << errorMessage << '\n';
                }
            } else {
                std::cout << "Unknown action.\n";
            }
        } else if (option == 12) {
            std::cout << "Goodbye.\n";
            break;
        } else {
            std::cout << "Please choose an option from 1 to 12.\n";
        }
    }
}
