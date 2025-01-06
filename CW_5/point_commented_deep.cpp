#include <set> // Include the set library for using std::set
#include <iostream> // Include the iostream library for input/output operations
#include <vector> // Include the vector library for using std::vector
#include <functional> // Include the functional library for using std::function
#include <map> // Include the map library for using std::map

// Define a planar subdivision without vertices of degree one and zero,
// and handle multiple queries. Each query is a point, and we need to determine
// the face of the subdivision it belongs to. This solution is offline.
// For doubles, change the compare methods and the point type.
// Complexity: O(log n) per query
typedef double val_type; // Define val_type as double for coordinate values

// Function to determine the sign of a value
int sign(const val_type& x) { return (x <= 0) ? (x == 0) ? 0 : -1 : 1; }

// Define a structure for a 2D point
struct PT {
    val_type x, y; // Coordinates of the point
    PT() {} // Default constructor
    PT(val_type _x, val_type _y) : x(_x), y(_y) {} // Parameterized constructor
    PT operator-(const PT& a) const { return PT(x - a.x, y - a.y); } // Subtract two points
    val_type dot(const PT& a) const { return x * a.x + y * a.y; } // Dot product of two points
    val_type dot(const PT& a, const PT& b) const { return (a - *this).dot(b - *this); } // Dot product relative to this point
    val_type cross(const PT& a) const { return x * a.y - y * a.x; } // Cross product of two points
    val_type cross(const PT& a, const PT& b) const { return (a - *this).cross(b - *this); } // Cross product relative to this point
    bool operator == (const PT& a) const { return a.x == x && a.y == y; } // Equality operator for points
};

// Define a structure for an edge
struct edge {
    PT left, right; // Left and right endpoints of the edge
};

// Function to compare two edges based on their orientation
bool edge_cmp(edge* edge1, edge* edge2) {
    const PT a = edge1->left, b = edge1->right; // Endpoints of the first edge
    const PT c = edge2->left, d = edge2->right; // Endpoints of the second edge
    int val = sign(a.cross(b, c)) + sign(a.cross(b, d)); // Determine the relative orientation
    if (val != 0) return val > 0; // Return true if the first edge is above the second
    val = sign(c.cross(d, a)) + sign(c.cross(d, b)); // Determine the relative orientation
    return val < 0; // Return true if the first edge is below the second
}

// Define an enumeration for event types
enum EventType { DEL = 2, ADD = 3, GET = 1, VERT = 0 };

// Define a structure for an event
struct Event {
    EventType type; // Type of the event
    int pos; // Position associated with the event
    bool operator < (const Event& event) const { // Comparison operator for events
        return type < event.type;
    }
};

// Function to perform the sweep line algorithm
std::vector<edge*> sweepline(std::vector<edge*> planar, std::vector<PT> queries) {
    // Collect all x-coordinates from the queries and planar edges
    std::set<val_type> all_x; // Set to store unique x-coordinates
    for (PT p : queries) all_x.insert(p.x); // Insert x-coordinates from queries
    for (edge* e : planar) { // Insert x-coordinates from planar edges
        all_x.insert(e->left.x);
        all_x.insert(e->right.x);
    }
    // Map all x-coordinates to unique IDs
    int id_counter = 0; // Counter for assigning IDs
    std::map<val_type, int> mapped_x; // Map to store x-coordinates and their IDs
    for (auto x : all_x) mapped_x[x] = id_counter++; // Assign IDs to x-coordinates
    // Create events for the sweep line algorithm
    auto edges = std::set<edge*, decltype(*edge_cmp)>(edge_cmp); // Set to store active edges
    auto vert_cmp = [](const std::pair<val_type, int>& left,const std::pair<val_type, int>& right) { // Comparator for vertical edges
        if (!(left.first == right.first)) return left.first < right.first; // Compare y-coordinates
        return left.second < right.second; // Compare indices if y-coordinates are equal
    };
    auto vert = std::set<std::pair<val_type, int>, decltype(vert_cmp)>(vert_cmp); // Set to store vertical edges
    std::vector<std::vector<Event>> events(id_counter); // Vector to store events for each x-coordinate
    for (int i = 0; i < (int)queries.size(); i++) { // Create GET events for queries
        int x = mapped_x[queries[i].x]; // Get the ID of the x-coordinate
        events[x].push_back(Event{GET, i}); // Add GET event for the query
    }
    for (int i = 0; i < (int)planar.size(); i++) { // Create ADD, DEL, and VERT events for planar edges
        int lx = mapped_x[planar[i]->left.x], rx = mapped_x[planar[i]->right.x]; // Get IDs of the edge endpoints
        if (lx > rx) { // Ensure lx is the left endpoint
            std::swap(lx, rx);
            std::swap(planar[i]->left, planar[i]->right);
        }
        if (lx == rx) { // If the edge is vertical
            events[lx].push_back(Event{VERT, i}); // Add VERT event
        } 
        else { // If the edge is not vertical
            events[lx].push_back(Event{ADD, i}); // Add ADD event at the left endpoint
            events[rx].push_back(Event{DEL, i}); // Add DEL event at the right endpoint
        }
    }
    // Perform the sweep line algorithm
    std::vector<edge*> ans(queries.size(), nullptr); // Vector to store the results
    for (int x = 0; x < id_counter; x++) { // Process events in order of x-coordinates
        sort(events[x].begin(), events[x].end()); // Sort events by type
        vert.clear(); // Clear the set of vertical edges
        for (Event event : events[x]) { // Process each event
            if (event.type == DEL) { // If the event is a DEL event
                edges.erase(planar[event.pos]); // Remove the edge from the active set
            }
            if (event.type == VERT) { // If the event is a VERT event
                vert.insert(std::make_pair(std::min(planar[event.pos]->left.y, planar[event.pos]->right.y), event.pos)); // Add the vertical edge to the set
            }
            if (event.type == ADD) { // If the event is an ADD event
                edges.insert(planar[event.pos]); // Add the edge to the active set
            }
            if (event.type == GET) { // If the event is a GET event
                auto jt = vert.upper_bound(std::make_pair(queries[event.pos].y, planar.size())); // Find the vertical edge above the query point
                if (jt != vert.begin()) { // If there is a vertical edge below the query point
                    --jt;
                    int i = jt->second; // Get the index of the vertical edge
                    if (std::max(planar[i]->left.y, planar[i]->right.y) >= queries[event.pos].y) { // If the query point is on or below the vertical edge
                        ans[event.pos] = planar[i]; // Set the result to the vertical edge
                        continue;
                    }
                }
                edge* new_edge = new edge; // Create a dummy edge for the query point
                new_edge->left = new_edge->right = queries[event.pos]; // Set the endpoints of the dummy edge
                auto it = edges.upper_bound(new_edge); // Find the edge above the query point
                if (it != edges.begin()) ans[event.pos] = *(--it); // Set the result to the edge below the query point
                delete new_edge; // Delete the dummy edge
            }
        }
    }
    return ans; // Return the results
}

// Define a structure for a Doubly Connected Edge List (DCEL)
struct DCEL {
    struct DCEL_EDGE { // Define a structure for an edge in the DCEL
        PT origin; // Origin vertex of the edge
        DCEL_EDGE* nxt = nullptr; // Pointer to the next edge in the face
        DCEL_EDGE* twin = nullptr; // Pointer to the twin edge
        int face; // Face associated with the edge
    };
    std::vector<DCEL_EDGE*> body; // Vector to store all edges in the DCEL
};

// Function to perform point location queries on a DCEL
std::vector<std::pair<int, int>> point_location(DCEL planar, std::vector<PT> queries) {
    std::vector<std::pair<int, int>> ans(queries.size()); // Vector to store the results
    std::vector<edge*> planar2; // Vector to store edges for the sweep line algorithm
    std::map<intptr_t, int> pos; // Map to store the face associated with each edge
    int n = planar.body.size(); // Number of edges in the DCEL
    for (int i = 0; i < n; i++) { // Process each edge in the DCEL
        if (planar.body[i]->face > planar.body[i]->twin->face) continue; // Skip if the edge is already processed
        edge* new_edge = new edge; // Create a new edge for the sweep line algorithm
        new_edge->left = planar.body[i]->origin; // Set the left endpoint of the edge
        new_edge->right = planar.body[i]->twin->origin; // Set the right endpoint of the edge
        pos[(intptr_t)new_edge] = // Map the edge to the appropriate face
            planar.body[i]->origin.x < planar.body[i]->twin->origin.x
            ? planar.body[i]->face
            : planar.body[i]->twin->face;
        planar2.push_back(new_edge); // Add the edge to the vector
    }
    auto res = sweepline(planar2, queries); // Perform the sweep line algorithm
    for (int i = 0; i < (int)queries.size(); i++) { // Process the results
        if (res[i] == nullptr) { // If no result was found
            std::cout << "-1" << '\n'; // Output -1
            continue;
        }
        std::cout << pos[(intptr_t)res[i]] << '\n'; // Output the face associated with the result
    }
    for (auto edge : planar.body) delete edge; // Free the memory allocated for the edges
    for (auto edge : planar2) delete edge; // Free the memory allocated for the edges
    return ans; // Return the results
}

// Main function
int32_t main() {
    int q; std::cin >> q; // Read the number of polygons
    DCEL planar; // Create a DCEL to store the planar subdivision
    for (int i = 0; i < q; i++) { // Process each polygon
    	int n; std::cin >> n; // Read the number of vertices in the polygon
    	std::vector<PT> p(n); // Vector to store the vertices
    	std::vector<DCEL::DCEL_EDGE*> edges(n); // Vector to store the edges
    	for (int j = 0; j < n; j++) { // Process each vertex
    		std::cin >> p[j].x >> p[j].y; // Read the coordinates of the vertex
    		edges[j] = new DCEL::DCEL_EDGE; // Create a new edge
    		edges[j]-> twin = new DCEL::DCEL_EDGE; // Create the twin edge
    	}
    	for (int j = 0; j < n; j++) { // Connect the edges
    		edges[j]->origin = p[j]; // Set the origin of the edge
    		edges[j]->nxt = edges[(j + 1) % n]; // Set the next edge in the face
    		edges[j]->face = i; // Set the face associated with the edge
    		auto rev = edges[j]->twin; // Get the twin edge
    		rev->origin = p[(j + 1) % n]; // Set the origin of the twin edge
    		rev->face = -1; // Set the face of the twin edge to the outer face
    		rev->nxt = edges[(j - 1 + n) % n] -> twin; // Set the next edge in the twin face
    		rev->twin = edges[j]; // Set the twin of the twin edge
    	}
    	for (int i = 0; i < n; i++) { // Add the edges to the DCEL
    		planar.body.push_back(edges[i]);
    		planar.body.push_back(edges[i]->twin);
    	}
    }
    int n; std::cin >> n; // Read the number of queries
    std::vector<PT> Q(n); // Vector to store the query points
    for (int i = 0; i < n; i++) { // Process each query
    	std::cin >> Q[i].x >> Q[i].y; // Read the coordinates of the query point
    }
    auto ret = point_location(planar, Q); // Perform point location queries
    return 0; // Return 0 to indicate successful execution
}