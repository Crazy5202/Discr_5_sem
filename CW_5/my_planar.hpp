#include <set>
#include <iostream> 
#include <vector>
#include <functional> 
#include <map> 

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
    bool operator == (const PT& a) const { return a.x == x and a.y == y; } // Equality operator for points
};

// Define a structure for an edge
// struct edge {
//     PT left, right; // Left and right endpoints of the edge
//     int face;
// };

// // Function to compare two edges based on their orientation
// bool edge_cmp(const edge& edge1, const edge& edge2) {
//     const PT a = edge1.left, b = edge1.right; // Endpoints of the first edge
//     const PT c = edge2.left, d = edge2.right; // Endpoints of the second edge
//     int val = sign(a.cross(b, c)) + sign(a.cross(b, d)); // Determine the relative orientation
//     if (val != 0) return val > 0; // Return true if the first edge is above the second
//     val = sign(c.cross(d, a)) + sign(c.cross(d, b)); // Determine the relative orientation
//     return val < 0; // Return true if the first edge is below the second
// }

class edge {
public:
    PT left, right; // Endpoints of the edge
    int face;

    // Equality operator for edges
    bool operator == (const edge& other) const { 
        return left == other.left and right == other.right and face == other.face; 
    } 
    // Overloaded < operator for comparing two edges
    bool operator<(const edge& other) const {
        const PT a = left, b = right; // Endpoints of the first edge
        const PT c = other.left, d = other.right; // Endpoints of the second edge
        int val = sign(a.cross(b, c)) + sign(a.cross(b, d)); // Determine the relative orientation
        if (val != 0) return val > 0; // Return true if the first edge is above the second
        val = sign(c.cross(d, a)) + sign(c.cross(d, b)); // Determine the relative orientation
        return val < 0; // Return true if the first edge is below the second
    }
    bool operator>(const edge& other) const {
        return !(*this == other) and !(*this < other);
    }
};

// Define an enumeration for event types
enum EventType {DEL = 1, ADD = 2, GET = 0};

// Define a structure for an event
class Event {
public:
    EventType type; // Type of the event
    int pos; // Position associated with the event
    bool operator < (const Event& event) const { // Comparison operator for events
        return type < event.type;
    }
};