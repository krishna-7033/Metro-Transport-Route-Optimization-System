# Metro/Transport Route Optimization System

A C++17 console project for demonstrating graph data structures and route
optimization algorithms. The sample network is fictional and is intended only
for academic demonstration.

## Phase 1 architecture

`Graph` is the owning model. Each station receives a stable integer ID when it
is inserted. Names are mapped to IDs with `std::unordered_map`, while the
network itself is stored as a `std::vector<std::vector<Connection>>`
adjacency list. A connection stores its destination ID, distance in kilometres,
and estimated travel time in minutes.

Connections are bidirectional by default. The `addConnection` API already has
a `bidirectional` parameter so one-way routes can be supported later without
changing the graph representation.

The current milestone covers station and connection management, route
optimization, persistence, fare calculation, and the interactive menu.

## Phase 2 algorithms

- Dijkstra with distance weights finds the minimum-distance route.
- Dijkstra with time weights finds the fastest route.
- BFS treats every connection as having cost 1 and finds the route with the
  fewest graph edges, not necessarily the shortest or fastest route.
- DFS checks reachability only; it does not promise an optimal route.

All route searches return the complete station-ID path and calculate its actual
distance, travel time, and number of edges. Unknown or unreachable stations
return a result with `found == false`; a station routed to itself is valid with
zero cost and zero stops.

## Data files

- `data/stations.txt`: one station name per line.
- `data/connections.txt`: `source|destination|distance_km|travel_time_minutes`.
  Saved files may include a fifth `one_way` flag (`1` for one-way, `0` for
  bidirectional). Four-field files remain bidirectional for compatibility.

The files describe the same fictional network used by the Phase 1 smoke test.
Invalid records are rejected without replacing the active in-memory network.
Saving writes temporary files before replacing the configured data files.

## Fare rule

The sample fare is `INR 10` base fare plus `INR 5` per travelled kilometre.
Fare calculation uses the shortest-distance route and is an academic example,
not a real transport authority tariff.

## Build and run

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Graph.cpp RouteOptimizer.cpp FareCalculator.cpp FileManager.cpp -o metro_optimizer
./metro_optimizer
```

## Automated tests

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -I. \
  tests/test_runner.cpp Graph.cpp RouteOptimizer.cpp \
  FareCalculator.cpp FileManager.cpp -o test_runner
./test_runner
```

The test runner covers graph validation, all route algorithms, unreachable and
same-station cases, fares, one-way connections, persistence round-trips, and
corrupted-file handling.

## Academic documentation

- [Project report](docs/project-report.md)
- [Viva questions](docs/viva-questions.md)
- [Architecture and complexity](docs/project-architecture.md)
