#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "Graph.h"

#include <string>

class FileManager {
public:
    static bool loadNetwork(const std::string& stationFile,
                            const std::string& connectionFile,
                            Graph& graph,
                            std::string& errorMessage);

    static bool saveNetwork(const std::string& stationFile,
                            const std::string& connectionFile,
                            const Graph& graph,
                            std::string& errorMessage);
};

#endif