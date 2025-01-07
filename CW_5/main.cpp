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
    
}

// Function to perform the sweep line algorithm
std::vector<int> sweepline(std::vector<edge> planar, std::vector<PT> queries) {
    // Collect all x-coordinates from the queries and planar edges
    std::set<val_type> all_x; // Set to store unique x-coordinates
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