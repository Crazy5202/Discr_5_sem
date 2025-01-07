#include <iostream>

#include "tree.hpp"
#include "reader.hpp"

void create_versions(const std::vector<edge>& planar, std::vector<Tree<edge>>& versions, std::map<val_type, int>& mapped_x) {
    std::vector<val_type> all_x; // Set to store unique x-coordinates
    for (auto& e : planar) { // Insert x-coordinates from planar edges
        all_x.push_back(e.left.x);
        all_x.push_back(e.right.x);
    }
    std::sort(all_x.begin(), all_x.end());
    all_x.erase(std::unique(all_x.begin(), all_x.end()), all_x.end());
    for (int i=0; i<all_x.size(); ++i) {
        mapped_x[all_x[i]] = i;
    }
    Tree<edge> t;
    versions.push_back(t);
    std::vector<std::vector<Event>> events(all_x.size());
    for (int i = 0; i < planar.size(); i++) { // Create ADD, DEL, and VERT events for planar edges
        int lx = mapped_x[planar[i].left.x], rx = mapped_x[planar[i].right.x]; // Get IDs of the edge endpoints
        events[lx].push_back(Event{ADD, i}); // Add ADD event at the left endpoint
        events[rx].push_back(Event{DEL, i}); // Add DEL event at the right endpoint
    }
    for (auto& current_events: events) {
        for (auto& event: current_events) {
            auto last_ver = versions.back();
            if (event.type == DEL) { // If the event is a DEL event
                last_ver = last_ver.remove(planar[event.pos]); // Remove the edge from the active set
            }
            if (event.type == ADD) { // If the event is an ADD event
                last_ver = last_ver.insert(planar[event.pos]); // Add the edge to the active set
            }
            versions.push_back(last_ver);
        }
    }
}

// Function to perform the sweep line algorithm
std::vector<int> sweepline(std::vector<edge> planar, std::vector<PT> queries) {
    // Perform the sweep line algorithm
    std::vector<int> ans(queries.size(), -1); // Vector to store the results
    // for (int x = 0; x < id_counter; x++) { // Process events in order of x-coordinates
    //     sort(events[x].begin(), events[x].end()); // Sort events by type
    //     for (Event event : events[x]) { // Process each event
    //         if (event.type == DEL) { // If the event is a DEL event
    //             edges.erase(planar[event.pos]); // Remove the edge from the active set
    //         }
    //         if (event.type == ADD) { // If the event is an ADD event
    //             edges.insert(planar[event.pos]); // Add the edge to the active set
    //         }
    //         if (event.type == GET) { // If the event is a GET event
    //             edge new_edge; // Create a dummy edge for the query point
    //             new_edge.left = new_edge.right = queries[event.pos]; // Set the endpoints of the dummy edge
    //             auto it = edges.upper_bound(new_edge); // Find the edge above the query point
    //             if (it != edges.begin()) ans[event.pos] = (*(--it)).face; // Set the result to the edge below the query point
    //         }
    //     }
    // }
    return ans; // Return the results
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

    std::vector<edge> planar;
    std::vector<PT> queries;

    try {
        planar = readIndex(indexFile);
        queries = readInput(inputFile);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
        return 1;
    }

    std::vector<Tree<edge>> versions;
    std::map<val_type, int> mapped_x;

    create_versions(planar, versions, mapped_x);

    // try {
    //     writeOutput(outputFile, results);

    //     std::cout << "Программа успешно завершена.\n";
    // } catch (const std::exception& e) {
    //     std::cerr << "Ошибка: " << e.what() << '\n';
    //     return 1;
    // }

    return 0;
}