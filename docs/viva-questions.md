# Viva Questions and Concise Answers

## Graph fundamentals

**1. What does a vertex represent?**

A vertex represents one transport station.

**2. What does an edge represent?**

An edge represents a direct connection between two stations. It stores distance
and estimated travel time as weights.

**3. Why use an adjacency list?**

Transport networks are usually sparse. An adjacency list uses $O(V+E)$ space
and makes neighbour traversal efficient, unlike an adjacency matrix's
$O(V^2)$ storage.

**4. Why are station IDs used in addition to names?**

Integer IDs provide stable vector indexes and faster array access. An
`unordered_map` converts user-facing names to those IDs.

**5. Why is a bidirectional connection stored twice?**

The graph stores directed adjacency entries. A bidirectional physical route
therefore needs one entry in each direction so both journeys can be searched.

## Algorithms

**6. Why use Dijkstra for distance?**

It finds a minimum-weight path when all edge weights are non-negative. Distance
is used as the edge weight in that version.

**7. How does the fastest route differ?**

It uses the same Dijkstra logic but assigns travel time, not distance, as the
edge weight. The selected path can therefore be physically longer.

**8. Why can BFS find minimum stops?**

BFS explores by increasing number of edges from the source. If each connection
has unit cost, the first path found uses the fewest graph edges.

**9. Why should BFS not use distance weights?**

BFS does not compare arbitrary weights. It only guarantees minimum edges. A
weighted shortest path requires Dijkstra here.

**10. What is DFS used for in this project?**

DFS checks whether a destination is reachable. Its traversal order does not
represent the shortest, fastest, or minimum-stop route.

**11. What is the purpose of the parent array?**

When a better or first path reaches a station, its predecessor is stored in
`parent`. Following these entries backward reconstructs the complete route.

**12. What data structure supports Dijkstra efficiently?**

A min-priority queue stores the next station with the smallest tentative cost.

**13. What is Dijkstra's complexity?**

With a binary heap and adjacency list it is $O((V+E)\log V)$ time and
$O(V)$ extra space.

**14. What are BFS and DFS complexities?**

Both run in $O(V+E)$ time and use $O(V)$ auxiliary space.

**15. Why are negative weights rejected?**

Dijkstra assumes non-negative edge weights. Negative distances or travel times
would violate the algorithm's correctness assumptions.

## Implementation and design

**16. Why separate `RouteOptimizer` from `Graph`?**

The graph owns data and validation, while the optimizer performs searches. This
separation makes each module easier to test and lets multiple algorithms reuse
the same graph.

**17. Why load into a temporary graph?**

If a file is malformed halfway through, the currently working graph remains
unchanged. The replacement happens only after all records pass validation.

**18. How is a one-way route represented?**

It is stored as one outgoing adjacency entry. A bidirectional route stores two
entries. The connection-file format accepts an optional direction field.

**19. What does the fare represent?**

It is an explicitly labelled academic estimate: INR 10 base fare plus INR 5 per
kilometre on the shortest-distance route. It is not a real tariff.

**20. Are graph edges the same as line changes?**

No. An edge is one direct station-to-station connection. A line change requires
line metadata, which this core version does not model.

**21. How would station closure be added?**

Add an active/closed state and make route algorithms skip closed stations and
connections, or build a filtered graph before searching.

**22. How did you test the system?**

The standalone test runner checks validation, all algorithms, edge cases, fares,
one-way routes, persistence round-trips, and corrupted-file rollback.

**23. How can two students work in parallel?**

One student owns `Graph` and `RouteOptimizer`; the other owns `FileManager`,
`FareCalculator`, and the menu. They integrate through the public graph and
route-result interfaces.

**24. What is the main limitation?**

The model has no transport-line metadata, so it cannot calculate actual line
transfers. It also uses a simplified fare rule.
