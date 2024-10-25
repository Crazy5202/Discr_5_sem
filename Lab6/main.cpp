#include <iostream>
#include <vector>
#include <list>

class Graph {
public:
    Graph(size_t);
    void add_edge(size_t, size_t);
    std::list<size_t> get_edge(size_t);
    std::vector<size_t> top_search();
private:
    size_t size;
    std::vector<std::list<size_t>> edges;
};

Graph::Graph(size_t _size): size(_size) {
    edges.resize(size);
}

void Graph::add_edge(size_t a, size_t b) {
    edges[a-1].push_back(b);
}

std::list<size_t> Graph::get_edge(size_t a) {
    return edges[a-1];
}

int main() {
    size_t n, m;
    std::cin >> n >> m;
    Graph graph(m);
    for (size_t i=0; i<m; ++i) {
        size_t a, b;
        std::cin >> a >> b;
        graph.add_edge(a,b);
    }
    std::cout << "END!" << std::endl;
}