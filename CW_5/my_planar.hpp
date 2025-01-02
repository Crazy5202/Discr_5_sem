#pragma once

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <set>

using val_type = double;
val_type n, m, x, y, k;

struct vec {
    val_type x, y;
    val_type operator% (const vec& v) { return x*v.y - y*v.x; }
    vec operator- (const vec& v) const { return { x-v.x, y-v.y }; }
    bool operator== (const vec& v) const { return x == v.x && y == v.y; }
};

enum Type { Left, Right, Point, End };
int c[4] = { 0, 2, 1, 3 }; // The sort order of the above types used for tiebreaking

class Event {
public:
    vec u, v; // Top and bottom points of the event, if applicable
    Type t;   // The type of the event
    int index;  // The associated segment, if not a point

    Event(vec u, vec v, Type t, int i) : u(u), v(v), t(t), index(i) {}

    bool operator<(const Event& other) const {
        if (t != Point && other.t != Point) { // Compare two segments
            if (u == other.u) // If their top points are the same, check rotation
                return (other.v - other.u) % (v - other.u) < 0;
            if (u.y == other.u.y) // If the top points are on the same level, compare x coordinate
                return u.x < other.u.x;
            else if (other.u.y > u.y)
                // If segment other is above, the highest point of segment
                // this needs to be in the left halfspace it defines, for this to be smaller
                return (other.v - other.u) % (u - other.u) < 0;
            return !(other < *this);
        } else if (t == Point && other.t != Point) { // Compare a point and a segment
            // A point compares less if it's in the left halfspace of the segment it compares to
            if ((u - other.u) % (other.v - other.u) == 0) // If it straddles, check if segment is horizontal
                return (other.u.y == other.v.y) ? (u.x <= other.v.x) : (c[t] < c[other.t]);
            else
                return (other.v - other.u) % (u - other.u) < 0;
        }
        return !(other < *this);
    }

    bool operator>(const Event& other) const {
        return !(*this < other);
    }
};