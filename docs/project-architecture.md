# Project Architecture

## Modules

| Module                  | Responsibility                                                     |
| ----------------------- | ------------------------------------------------------------------ |
| `Station.h`             | Defines the station record and its integer ID.                     |
| `Graph.h/.cpp`          | Owns stations, adjacency-list connections, validation, and lookup. |
| `RouteOptimizer.h/.cpp` | Implements Dijkstra, BFS, DFS, and route reconstruction.           |
| `FareCalculator.h/.cpp` | Calculates the documented sample fare from route distance.         |
| `FileManager.h/.cpp`    | Validates and loads/saves the two text files.                      |
| `main.cpp`              | Provides the interactive menu and coordinates modules.             |

## Graph representation

The graph is represented as an adjacency list. For every station ID `u`,
`adjacencyList[u]` contains a vector of outgoing `Connection` records. In the
default bidirectional mode, adding `u -> v` also adds `v -> u`.

This representation uses $O(V + E)$ space for a directed graph and supports
efficient iteration over a station's neighbours. With the current name index,
station lookup is expected $O(1)$ average time. Adding a connection checks the
source adjacency list for duplicates, so it is $O(degree(u))$.

## Algorithm ownership

`RouteOptimizer` receives a read-only `const Graph&`. It does not modify the
network while searching. Each algorithm returns a route result with station
IDs, total distance, total travel time, and number of edges, allowing the menu
and fare calculator to reuse the same result.

## Phase 2 algorithms

### Dijkstra

For each Dijkstra variant, `distance[u]` stores the best known cost from the
source to station `u`, and `parent[u]` stores the previous station on that
best route. A min-priority queue always expands the unsettled station with the
smallest known cost. One version uses `distanceKm`; the other uses
`travelTimeMinutes`. Both require non-negative edge weights, which the graph
validation enforces.

With a binary heap, the time complexity is $O((V + E)\log V)$ and the extra
space complexity is $O(V)$. Dijkstra is not appropriate for negative edge
weights.

### BFS

BFS uses a queue and marks a station when it is first discovered. Since every
edge has unit cost in this interpretation, the first discovered path uses the
fewest graph edges. Its time complexity is $O(V + E)$ and extra space is
$O(V)$. It does not minimize distance or travel time.

### DFS

DFS uses an explicit stack to explore reachable stations. It answers only the
yes/no connectivity question. Its time complexity is $O(V + E)$ and extra
space is $O(V)$; the traversal order does not imply an optimal route.

### Route reconstruction

After a search, the destination is followed through `parent[]` until the
source is reached. The reversed sequence is then returned in source-to-
destination order. The actual edge records are summed afterward, which keeps
BFS's optimization criterion separate from passenger metrics.

| Operation         | Criterion            | Time               | Extra space |
| ----------------- | -------------------- | ------------------ | ----------- |
| Shortest distance | Dijkstra, kilometres | $O((V + E)\log V)$ | $O(V)$      |
| Fastest route     | Dijkstra, minutes    | $O((V + E)\log V)$ | $O(V)$      |
| Minimum stops     | BFS, unit edge cost  | $O(V + E)$         | $O(V)$      |
| Reachability      | DFS                  | $O(V + E)$         | $O(V)$      |

## Collaboration boundary

Student 1 owns `Graph` and `RouteOptimizer`. Student 2 owns `FileManager`,
`FareCalculator`, and the menu in `main.cpp`. Integration happens through the
public `Graph` accessors and the route-result type; both students should review
the complete flow before final submission.
