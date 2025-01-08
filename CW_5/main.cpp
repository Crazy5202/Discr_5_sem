#include <iostream>

#include "tree.hpp"
#include "reader.hpp"

void create_versions(const std::vector<edge>& planar, std::vector<Tree<edge>>& versions, std::vector<val_type>& all_x) {
    val_type small = 1e-6;
    if (!(small>0)) {
        small = 1;
    }
    for (auto& e : planar) { // Insert x-coordinates from planar edges
        all_x.push_back(e.left.x);
        all_x.push_back(e.right.x+small);
    }
    std::sort(all_x.begin(), all_x.end());
    auto unique_x = all_x;
    unique_x.erase(std::unique(unique_x.begin(), unique_x.end()), unique_x.end());
    std::unordered_map<val_type, int> mapped_x;
    for (int i=0; i<unique_x.size(); ++i) {
        mapped_x[unique_x[i]] = i;
    }
    Tree<edge> t;
    versions.push_back(t);
    std::vector<std::vector<Event>> events(unique_x.size());
    for (int i = 0; i < planar.size(); i++) { // Create ADD, DEL, and VERT events for planar edges
        int lx = mapped_x[planar[i].left.x], rx = mapped_x[planar[i].right.x+small]; // Get IDs of the edge endpoints
        events[lx].push_back(Event{ADD, i}); // Add ADD event at the left endpoint
        events[rx].push_back(Event{DEL, i}); // Add DEL event at the right endpoint
    }
    for (auto& current_events: events) {
        std::sort(current_events.begin(), current_events.end()); // Sort events by type
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
std::vector<int> sweepline(std::vector<PT>& queries, std::vector<Tree<edge>>& versions, std::vector<val_type>& all_x) {
    // Perform the sweep line algorithm
    std::vector<int> results; // Vector to store the results

    for (int i=0; i<queries.size(); ++i) {
        auto it_ver = std::upper_bound(all_x.begin(), all_x.end(), queries[i].x);
        if (it_ver == all_x.begin() or it_ver == all_x.end()) {
            results.push_back(-1);
            continue;
        }
        auto pos = it_ver - all_x.begin();
        edge dummy_edge; // Create a dummy edge for the query point
        dummy_edge.left = dummy_edge.right = queries[i];

        auto it_elem = versions[pos].upper_bound(dummy_edge);

        if (it_elem != versions[pos].begin() and it_elem.is_initialized()) {
            auto prev = versions[pos].previous(it_elem.get());
            results.push_back(prev.get().face);
        } else {
            results.push_back(-1);
        }
        
        // auto items = versions[pos].items();
        // auto it_elem = std::upper_bound(items.begin(), items.end(), dummy_edge);
        // if(it_elem != items.begin()) {
        //     --it_elem;
        //     results.push_back(it_elem->face);
        // } else {
        //     results.push_back(-1);
        // }
    }
    
    return results; // Return the results
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
    std::vector<val_type> all_x;

    create_versions(planar, versions, all_x);

    planar.clear();

    auto results = sweepline(queries, versions, all_x);

    try {
        writeOutput(outputFile, results);

        std::cout << "Программа успешно завершена.\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
        return 1;
    }

    return 0;
}