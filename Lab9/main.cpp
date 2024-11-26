#include <iostream>
#include <vector>
#include <list>
#include <chrono>
#include <queue>

size_t max_val = (size_t)-1;

class Graph {
public:
    Graph(size_t);
    void add_edge(size_t, size_t, size_t);
    size_t djikstra(size_t, size_t);
private:

    std::list<std::pair<size_t, size_t>> get_edges(size_t);

    int get_state(size_t);
    void set_state(size_t, int);

    bool is_visited(size_t);
    void set_visited(size_t);

    size_t get_distance(size_t);
    void set_distance(size_t, size_t);

    std::vector<int> states;
    std::vector<size_t> distances;
    std::vector<std::list<std::pair<size_t, size_t>>> edges;
    std::vector<size_t> result;
};

Graph::Graph(size_t size) {
    edges.resize(size);
    states.resize(size, 0);
    distances.resize(size, max_val);
}

void Graph::add_edge(size_t a, size_t b, size_t w) {
    edges[a-1].push_back(std::make_pair(b,w));
    edges[b-1].push_back(std::make_pair(a,w));
}

std::list<std::pair<size_t, size_t>> Graph::get_edges(size_t a) {
    return edges[a-1];
}

void Graph::set_state(size_t n, int state) {
    states[n-1]=state;
}

int Graph::get_state(size_t n) {
    return states[n-1];
}

void Graph::set_visited(size_t n) {
    set_state(n,1);
}

bool Graph::is_visited(size_t n) {
    return get_state(n) == 0;
}

void Graph::set_distance(size_t n, size_t distance) {
    distances[n-1] = distance;
}

size_t Graph::get_distance(size_t n) {
    return distances[n-1];
}

size_t Graph::djikstra(size_t start, size_t finish) {
    set_distance(start, 0);
    std::priority_queue<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>, std::greater<std::pair<size_t, size_t>>> heap;
    heap.push(std::make_pair(0, start));
    while (!heap.empty()) {
        auto elem = heap.top();
        heap.pop();
        auto dist = elem.first, vert = elem.second;
        if (dist > get_distance(vert)) {
            continue;
        }
        for (auto elem: get_edges(vert)) {
            auto adj_vert = elem.first, w = elem.second;
            auto new_dist = get_distance(vert) + w;
            if (get_distance(adj_vert) > new_dist) {
                set_distance(adj_vert, new_dist);
                heap.push(std::make_pair(new_dist, adj_vert));
            }
        }
    }
    return get_distance(finish);
}

int main() {
    auto begin = std::chrono::steady_clock::now();
    size_t n, m, start, finish;
    std::cin >> n >> m >> start >> finish;
    Graph graph(n);
    for (size_t i=0; i<m; ++i) {
        size_t a, b, w;
        std::cin >> a >> b >> w;
        graph.add_edge(a,b,w);
    }
    auto res = graph.djikstra(start, finish);
    if (res==max_val) std::cout << "No solution" << '\n';
    else {
        std::cout << res << '\n'; 
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Program took " << duration.count() << " microseconds to execute" << std::endl;
}