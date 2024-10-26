#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <chrono>

class Graph {
public:
    Graph(size_t);
    void add_edge(size_t, size_t);
    std::vector<size_t> top_sort();
private:
    void dfs_visit(size_t);

    std::list<size_t> get_edges(size_t);

    int get_state(size_t);
    void set_state(size_t, int);

    bool is_white(size_t);
    bool is_grey(size_t);
    bool is_black(size_t);

    void set_white(size_t);
    void set_grey(size_t);
    void set_black(size_t);

    bool has_cycle;
    size_t size;
    std::vector<int> states;
    std::vector<std::list<size_t>> edges;
    std::vector<size_t> result;
};

Graph::Graph(size_t _size): size(_size) {
    edges.resize(size);
    states.resize(size);
}

void Graph::add_edge(size_t a, size_t b) {
    edges[a-1].push_back(b);
}

std::list<size_t> Graph::get_edges(size_t a) {
    return edges[a-1];
}

void Graph::set_state(size_t n, int state) {
    states[n-1]=state;
}

int Graph::get_state(size_t n) {
    return states[n-1];
}

void Graph::set_white(size_t n) {
    set_state(n,0);
}

bool Graph::is_white(size_t n) {
    return get_state(n) == 0;
}

void Graph::set_grey(size_t n) {
    set_state(n,1);
}

bool Graph::is_grey(size_t n) {
    return get_state(n)==1;
}

void Graph::set_black(size_t n) {
    set_state(n,2);
}

bool Graph::is_black(size_t n) {
    return get_state(n)==2;
}

void Graph::dfs_visit(size_t u) {
    set_grey(u);

    auto vert_edges = get_edges(u);

    for (auto it = vert_edges.begin(); it!=vert_edges.end(); ++it) {
        if (is_white(*it)) {
            dfs_visit(*it);
        } else if (is_grey(*it)) {
            has_cycle = true;
        }
    }
    set_black(u);
    result.push_back(u);
}

std::vector<size_t> Graph::top_sort() {
    for (int i=1; i<=size; ++i) {
        set_white(i);
    }
    has_cycle = false;
    result.clear();
    for (int i=1; i<=size; ++i) {
        if (is_white(i)) {
            dfs_visit(i);
        }
    }

    if (has_cycle) return {};
    std::reverse(result.begin(), result.end());
    return result;
}

int main() {
    //auto start = std::chrono::steady_clock::now();
    size_t n, m;
    std::cin >> n >> m;
    Graph graph(n);
    for (size_t i=0; i<m; ++i) {
        size_t a, b;
        std::cin >> a >> b;
        graph.add_edge(a,b);
    }
    auto res = graph.top_sort();
    if (res.empty()) std::cout << -1 << '\n';
    else {
        for (int i=1; i<res.size(); ++i) {
            std::cout << res[i-1] << ' ' << res[i] << '\n';
        }
    }
    //auto end = std::chrono::steady_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    //std::cout << "Program took " << duration.count() << " microseconds to execute" << std::endl;
}