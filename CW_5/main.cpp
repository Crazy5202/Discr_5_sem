#include <iostream>
#include <chrono>

#include "tree.hpp"
#include "reader.hpp"

// Function to create tree versions
void create_versions(const std::vector<edge>& planar, std::vector<Tree<edge>>& versions, std::vector<val_type>& all_x) {
    val_type small = 1e-6; // For separating insert and delete
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
    for (int i = 0; i < planar.size(); i++) { // Create  events for planar edges
        int lx = mapped_x[planar[i].left.x], rx = mapped_x[planar[i].right.x+small];
        events[lx].push_back(Event{ADD, i}); // Add ADD event at the left endpoint
        events[rx].push_back(Event{DEL, i}); // Add DEL event at the right endpoint
    }
    for (auto& current_events: events) { // Process events and add versions
        for (auto& event: current_events) {
            auto last_ver = versions.back();
            if (event.type == DEL) { 
                last_ver = last_ver.remove(planar[event.pos]); 
            }
            if (event.type == ADD) { 
                last_ver = last_ver.insert(planar[event.pos]);
            }
            versions.push_back(last_ver);
        }
    }

}

// Function to perform the sweep line algorithm
std::vector<int> sweepline(std::vector<PT>& queries, std::vector<Tree<edge>>& versions, std::vector<val_type>& all_x) {
    std::vector<int> results; // Vector to store the results

    for (int i=0; i<queries.size(); ++i) {
        // Find the right version
        auto it_ver = std::upper_bound(all_x.begin(), all_x.end(), queries[i].x);
        if (it_ver == all_x.begin() or it_ver == all_x.end()) {
            results.push_back(-1);
            continue;
        }
        auto pos = it_ver - all_x.begin();
        edge dummy_edge; // Create a dummy edge for the query point
        dummy_edge.left = dummy_edge.right = queries[i];

        auto it_elem = versions[pos].upper_bound(dummy_edge); // Find the first edge above the point

        if (it_elem != versions[pos].begin() and it_elem != versions[pos].end()) { // Check that it's not begin() or end()
            auto val_elem = it_elem.get();
            auto begin_val = versions[pos].begin().get();
            auto prev = versions[pos].previous(it_elem.get());
            if (prev != versions[pos].end())
                results.push_back(prev.get().face);
            else 
                results.push_back(-1);
        } else
            results.push_back(-1);
    }
    
    return results; // Return the results
}

int main(int argc, char* argv[]) {
    auto begin = std::chrono::steady_clock::now();
    
    std::ios::sync_with_stdio(0);

    if (argc !=8) { // Check that input is right
        std::cout << argc << std::endl;
        std::cerr << "Использование: ./prog search --index <index file> --input <input file> --output <output file>\n";
        return 1;
    }

    std::string indexFile, inputFile, outputFile;

    for (int i = 2; i < argc; i += 2) { // Check that input is right
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

    try { // Try to read from files
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

    try { // Try to write results
        writeOutput(outputFile, results);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
        return 1;
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "Program took " << duration.count() << " milliseconds to execute" << std::endl;

    return 0;
}