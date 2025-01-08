typedef double val_type; // Define val_type as double for coordinate values

// Function to determine the sign of a value
int sign(const val_type& x) { return (x <= 0) ? (x == 0) ? 0 : -1 : 1; }

// Define a structure for a 2D point
class PT {
public:
    val_type x, y; // Coordinates of the point
    PT() {}
    PT(val_type _x, val_type _y) : x(_x), y(_y) {} 
    PT operator-(const PT& a) const { return PT(x - a.x, y - a.y); } 
    val_type dot(const PT& a) const { return x * a.x + y * a.y; } // Dot product of two points
    val_type dot(const PT& a, const PT& b) const { return (a - *this).dot(b - *this); } // Dot product relative to this point
    val_type cross(const PT& a) const { return x * a.y - y * a.x; } // Cross product of two points
    val_type cross(const PT& a, const PT& b) const { return (a - *this).cross(b - *this); } // Cross product relative to this point
    bool operator == (const PT& a) const { return a.x == x and a.y == y; }
};

class edge {
public:
    PT left, right; // Endpoints of the edge
    int face; // Index of the polygon the edge belongs to

    // Equality operator for edges
    bool operator == (const edge& other) const { 
        return left == other.left and right == other.right; 
    } 
    // Overloaded < operator for comparing two edges
    bool operator<(const edge& other) const {
        const PT a = left, b = right;
        const PT c = other.left, d = other.right;
        int val = sign(a.cross(b, c)) + sign(a.cross(b, d)); // Determine the relative orientation
        if (val != 0) return val > 0; 
        val = sign(c.cross(d, a)) + sign(c.cross(d, b)); 
        return val < 0;
    }
    bool operator>(const edge& other) const {
        return !(*this == other) and !(*this < other);
    }
};

// Define an enumeration for event types
enum EventType {DEL = 0, ADD = 1};

// Define a structure for an event
class Event {
public:
    EventType type; // Type of the event
    int pos; // Position associated with the event
    // Comparison operator for events
    bool operator < (const Event& event) const {
        return type < event.type;
    }
};