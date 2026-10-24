#include "FileManager.h"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

namespace {
std::string trim(const std::string& value) {
    const std::size_t first = value.find_first_not_of(" \t\r");
    if (first == std::string::npos) {
        return {};
    }
    const std::size_t last = value.find_last_not_of(" \t\r");
    return value.substr(first, last - first + 1);
}

bool isIgnored(const std::string& line) {
    return line.empty() || line.front() == '#';
}

bool parseConnection(const std::string& line,
                     std::string& source,
                     std::string& destination,
                     double& distanceKm,
                     int& travelTimeMinutes,
                     bool& bidirectional) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, '|')) {
        fields.push_back(trim(field));
    }
    if (fields.size() != 4 && fields.size() != 5) {
        return false;
    }

    try {
        std::size_t distanceCharacters = 0;
        std::size_t timeCharacters = 0;
        source = fields[0];
        destination = fields[1];
        distanceKm = std::stod(fields[2], &distanceCharacters);
        travelTimeMinutes = std::stoi(fields[3], &timeCharacters);
        if (distanceCharacters != fields[2].size() ||
            timeCharacters != fields[3].size()) {
            return false;
        }
        bidirectional = fields.size() == 4 || fields[4] == "0";
    } catch (const std::exception&) {
        return false;
    }
    return !source.empty() && !destination.empty() && std::isfinite(distanceKm) &&
           distanceKm > 0.0 && travelTimeMinutes > 0 &&
           (fields.size() == 4 || fields[4] == "0" || fields[4] == "1");
}

bool writeFileSafely(const std::string& path,
                     const std::string& temporaryPath,
                     const std::string& content,
                     std::string& errorMessage) {
    {
        std::ofstream output(temporaryPath);
        if (!output) {
            errorMessage = "Could not open temporary file: " + temporaryPath;
            return false;
        }
        output << content;
        if (!output) {
            errorMessage = "Could not write file: " + path;
            return false;
        }
    }
    if (std::rename(temporaryPath.c_str(), path.c_str()) != 0) {
        std::remove(temporaryPath.c_str());
        errorMessage = "Could not replace file: " + path;
        return false;
    }
    return true;
}
}

bool FileManager::loadNetwork(const std::string& stationFile,
                               const std::string& connectionFile,
                               Graph& graph,
                               std::string& errorMessage) {
    std::ifstream stationsInput(stationFile);
    if (!stationsInput) {
        errorMessage = "Could not open station file: " + stationFile;
        return false;
    }

    Graph loadedGraph;
    std::string line;
    int lineNumber = 0;
    while (std::getline(stationsInput, line)) {
        ++lineNumber;
        line = trim(line);
        if (isIgnored(line)) {
            continue;
        }
        if (!loadedGraph.addStation(line)) {
            errorMessage = "Invalid or duplicate station at line " +
                           std::to_string(lineNumber);
            return false;
        }
    }

    std::ifstream connectionsInput(connectionFile);
    if (!connectionsInput) {
        errorMessage = "Could not open connection file: " + connectionFile;
        return false;
    }

    lineNumber = 0;
    while (std::getline(connectionsInput, line)) {
        ++lineNumber;
        line = trim(line);
        if (isIgnored(line)) {
            continue;
        }

        std::string source;
        std::string destination;
        double distanceKm = 0.0;
        int travelTimeMinutes = 0;
        bool bidirectional = true;
        if (!parseConnection(line, source, destination, distanceKm,
                             travelTimeMinutes, bidirectional) ||
            !loadedGraph.addConnection(source, destination, distanceKm,
                                        travelTimeMinutes, bidirectional)) {
            errorMessage = "Invalid connection at line " +
                           std::to_string(lineNumber);
            return false;
        }
    }

    graph = loadedGraph;
    return true;
}

bool FileManager::saveNetwork(const std::string& stationFile,
                              const std::string& connectionFile,
                              const Graph& graph,
                              std::string& errorMessage) {
    std::ostringstream stationsContent;
    stationsContent << "# One station name per line\n";
    for (const Station& station : graph.getStations()) {
        stationsContent << station.name << '\n';
    }

    std::ostringstream connectionsContent;
    connectionsContent << "# source|destination|distance_km|travel_time_minutes|one_way\n";
    const auto& stations = graph.getStations();
    const auto& adjacencyList = graph.getAdjacencyList();
    for (const Station& station : stations) {
        for (const Connection& connection : adjacencyList[station.id]) {
            connectionsContent << station.name << '|'
                               << stations[connection.destination].name << '|'
                               << connection.distanceKm << '|'
                               << connection.travelTimeMinutes << "|1\n";
        }
    }

    const std::string stationTemporary = stationFile + ".tmp";
    const std::string connectionTemporary = connectionFile + ".tmp";
    if (!writeFileSafely(stationFile, stationTemporary, stationsContent.str(),
                         errorMessage)) {
        return false;
    }
    if (!writeFileSafely(connectionFile, connectionTemporary,
                         connectionsContent.str(), errorMessage)) {
        return false;
    }
    return true;
}