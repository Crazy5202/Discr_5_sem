#include <set> // Include the set library for using std::set
#include <iostream> // Include the iostream library for input/output operations
#include <vector> // Include the vector library for using std::vector
#include <functional> // Include the functional library for using std::function
#include <map> // Include the map library for using std::map

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
    int face;
};

// Function to compare two edges based on their orientation
bool edge_cmp(const edge& edge1, const edge& edge2) {
    const PT a = edge1.left, b = edge1.right; // Endpoints of the first edge
    const PT c = edge2.left, d = edge2.right; // Endpoints of the second edge
    int val = sign(a.cross(b, c)) + sign(a.cross(b, d)); // Determine the relative orientation
    if (val != 0) return val > 0; // Return true if the first edge is above the second
    val = sign(c.cross(d, a)) + sign(c.cross(d, b)); // Determine the relative orientation
    return val < 0; // Return true if the first edge is below the second
}

// Define an enumeration for event types
enum EventType {DEL = 1, ADD = 2, GET = 0};

// Define a structure for an event
struct Event {
    EventType type; // Type of the event
    int pos; // Position associated with the event
    bool operator < (const Event& event) const { // Comparison operator for events
        return type < event.type;
    }
};

// Function to perform the sweep line algorithm
std::vector<int> sweepline(std::vector<edge> planar, std::vector<PT> queries) {
    // Collect all x-coordinates from the queries and planar edges
    std::set<val_type> all_x; // Set to store unique x-coordinates
    for (PT p : queries) 
        all_x.insert(p.x); // Insert x-coordinates from queries
    for (edge& e : planar) { // Insert x-coordinates from planar edges
        all_x.insert(e.left.x);
        all_x.insert(e.right.x);
    }
    // Map all x-coordinates to unique IDs
    int id_counter = 0; // Counter for assigning IDs
    std::map<val_type, int> mapped_x; // Map to store x-coordinates and their IDs
    for (auto x : all_x) mapped_x[x] = id_counter++; // Assign IDs to x-coordinates
    // Create events for the sweep line algorithm
    auto edges = std::set<edge, decltype(*edge_cmp)>(edge_cmp); // Set to store active edges
    std::vector<std::vector<Event>> events(id_counter); // Vector to store events for each x-coordinate
    for (int i = 0; i < (int)queries.size(); i++) { // Create GET events for queries
        int x = mapped_x[queries[i].x]; // Get the ID of the x-coordinate
        events[x].push_back(Event{GET, i}); // Add GET event for the query
    }
    for (int i = 0; i < (int)planar.size(); i++) { // Create ADD, DEL, and VERT events for planar edges
        int lx = mapped_x[planar[i].left.x], rx = mapped_x[planar[i].right.x]; // Get IDs of the edge endpoints
        if (lx > rx) { // Ensure lx is the left endpoint
            std::swap(lx, rx);
            std::swap(planar[i].left, planar[i].right);
        }
        if (lx == rx) { // If the edge is vertical
            continue;
        } 
        else { // If the edge is not vertical
            events[lx].push_back(Event{ADD, i}); // Add ADD event at the left endpoint
            events[rx].push_back(Event{DEL, i}); // Add DEL event at the right endpoint
        }
    }
    // Perform the sweep line algorithm
    std::vector<int> ans(queries.size(), -1); // Vector to store the results
    for (int x = 0; x < id_counter; x++) { // Process events in order of x-coordinates
        sort(events[x].begin(), events[x].end()); // Sort events by type
        for (Event event : events[x]) { // Process each event
            if (event.type == DEL) { // If the event is a DEL event
                edges.erase(planar[event.pos]); // Remove the edge from the active set
            }
            if (event.type == ADD) { // If the event is an ADD event
                edges.insert(planar[event.pos]); // Add the edge to the active set
            }
            if (event.type == GET) { // If the event is a GET event
                edge new_edge; // Create a dummy edge for the query point
                new_edge.left = new_edge.right = queries[event.pos]; // Set the endpoints of the dummy edge
                auto it = edges.upper_bound(new_edge); // Find the edge above the query point
                if (it != edges.begin()) ans[event.pos] = (*(--it)).face; // Set the result to the edge below the query point
            }
        }
    }
    return ans; // Return the results
}

// Main function
int main() {
    int q; std::cin >> q; // Read the number of polygons
    std::vector<edge> planar; // Vector to store the planar subdivision
    for (int i = 0; i < q; i++) { // Process each polygon
    	int n; std::cin >> n; // Read the number of vertices in the polygon
    	std::vector<PT> p(n); // Vector to store the vertices
        std::vector<edge> edges(n); // Vector to store the edges
    	for (int j = 0; j < n; j++) { // Process each vertex
    		std::cin >> p[j].x >> p[j].y; // Read the coordinates of the vertex
    	}
        for (int j = 0; j < n; j++) { // Process each vertex
            edges[j].left = p[j];
            edges[j].right = p[(j + 1) % n];
            if (edges[j].left.x < edges[j].right.x) {
                edges[j].face = i;
            } else if (edges[j].left.x > edges[j].right.x) {
                edges[j].face = -1;
            }
    	}
    	for (int i = 0; i < n; i++) { // Add the edges to the DCEL
    		planar.push_back(edges[i]);
    	}
    }
    int n; std::cin >> n; // Read the number of queries
    std::vector<PT> Q(n); // Vector to store the query points
    for (int i = 0; i < n; i++) { // Process each query
    	std::cin >> Q[i].x >> Q[i].y; // Read the coordinates of the query point
    }
    auto res = sweepline(planar, Q); // Perform point location queries
    for (int i = 0; i < res.size(); i++) { // Process the results
        std::cout << res[i] << '\n'; // Output the face associated with the result
    }
    return 0; // Return 0 to indicate successful execution
}