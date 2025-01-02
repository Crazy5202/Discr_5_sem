#include <iostream>

#include "tree.hpp"
#include "reader.hpp"

bool func(const Event& a, const Event& b) {
    // The horizontal ordering of the events of the sweepline. Events that intersect to the left
    // are considered less than points on the right on the scanline, and as a tiebreaker,
    // left segments are smaller than points are smaller than right segments
    if(a.t != Point && b.t != Point) { // We compare two segments
        if(a.u == b.u) // If their top points are the same, check rotation
            return (b.v-b.u)%(a.v-b.u) < 0;
        if(a.u.y == b.u.y) // If the top points are on the same level, compare x coordinate
            return a.u.x < b.u.x;
        else if(b.u.y > a.u.y)
            // If segment b is above a, the highest point of segment
            // a needs to be in the left halfspace it defines, for a to be smaller
            return (b.v-b.u)%(a.u-b.u) < 0;
        return !func(b, a);
    } else if(a.t == Point && b.t != Point) { // We compare a point and segment
        // A point compares less if it's in the left halfspace of the segment it compares to
        if((a.u-b.u)%(b.v-b.u) == 0) // If it straddles, check if segment is horizontal
            return (b.u.y == b.v.y) ? (a.u.x <= b.v.x) : (c[a.t] < c[b.t]);
        else
            return (b.v-b.u)%(a.u-b.u) < 0;
    }
    return !func(b, a);
}

int main(int argc, char* argv[]) {
    if (argc !=8) {
        std::cout << argc << std::endl;
        std::cerr << "Использование: ./prog search --index <index file> --input <input file> --output <output file>\n";
        return 1;
    }

    std::string indexFile, inputFile, outputFile;

    for (int i = 2; i < argc; i += 2) {
        std::string key = argv[i];
        if (key == "--index") {
            indexFile = argv[i + 1];
        } else if (key == "--input") {
            inputFile = argv[i + 1];
        } else if (key == "--output") {
            outputFile = argv[i + 1];
        } else {
            std::cerr << "Неизвестный ключ: " << key << '\n';
            return 1;
        }
    }

    std::vector<std::vector<vec>> polygons;
    std::vector<vec> queries;

    try {
        polygons = readIndex(indexFile);
        queries = readInput(inputFile);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
        return 1;
    }

    std::vector<Event> events;

    int cur_polygon = 0;
    for (auto& polygon: polygons) {
        for (int i=0; i<polygon.size(); ++i) {
            vec a = polygon[i], b = polygon[(i+1)%polygon.size()];
            // Make sure point a is above a, and check if the segment is part of the left or right
            // side of a polygon
            if(a.y <= b.y) {
                if(a.y == b.y && a.x < b.x) // Horizontal segments are considered right
                    std::swap(a, b);
                events.push_back({ b, a, Right, cur_polygon });
                events.push_back({ a, b, End, cur_polygon });
            } else if(a.y > b.y) {
                events.push_back({ a, b, Left, cur_polygon });
                events.push_back({ b, a, End, cur_polygon });
            }
        }
        ++cur_polygon;
    }

    std::sort(events.begin(), events.end(), [] (const Event& a, const Event& b) {
        return (a.u.y == b.u.y) ? (a.t < b.t) : (a.u.y > b.u.y);
    });

    Tree<Event, Event> t;
    std::vector<Tree<Event, Event>> versions(1, t);
    for (auto& event: events) {
        auto last_ver = versions.back();
        if(event.t == End)
            last_ver = last_ver.remove(event);
        else
            last_ver = last_ver.insert(event, event);
        versions.push_back(last_ver);
    }

    std::vector<std::string> results;

    for (auto query: queries) {
        Event query_event(query, query, Point, 0);
        auto it_ver = std::upper_bound(events.begin(), events.end(), query_event, [] (const Event& a, const Event& b) {
            return (a.u.y == b.u.y) ? (a.t < b.t) : (a.u.y > b.u.y);
        });
        if (it_ver == events.begin() or it_ver == events.end()) {
            results.push_back("-1");
            continue;
        }
        auto pos = it_ver - events.begin();
        auto items = versions[pos].items();
        std::vector<Event> current_events;
        for (auto& pair: items) {
            current_events.push_back(pair.first);
        }
        auto it_event = std::upper_bound(current_events.begin(), current_events.end(), query_event, func);
        if(it_event != current_events.end() && it_event->t == Right) results.push_back(std::to_string(it_event->index));
        else {
            results.push_back("-1");
        }
    }

    try {
        writeOutput(outputFile, results);

        std::cout << "Программа успешно завершена.\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
        return 1;
    }

    return 0;
}