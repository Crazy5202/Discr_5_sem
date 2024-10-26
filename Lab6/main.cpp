#include <iostream>
#include <vector>
#include <list>
#include <stack>

class Graph {
public:
    Graph(size_t);
    void add_edge(size_t, size_t);
    std::stack<size_t> topog_sort();
private:
    std::list<size_t> get_edges(size_t);
    int get_state(size_t);
    void set_state(size_t, int);
    bool is_white(size_t);
    bool is_grey(size_t);
    bool is_black(size_t);
    void set_white(size_t);
    void set_grey(size_t);
    void set_black(size_t);

    size_t size;
    std::vector<int> states;
    std::vector<std::list<size_t>> edges;
};

Graph::Graph(size_t _size): size(_size) {
    edges.resize(size);
    states.resize(size);
    for (size_t i=1; i<=size; ++i) {
        set_white(i);
    }
}

void Graph::add_edge(size_t a, size_t b) {
    edges[a-1].push_back(b);
}

std::list<size_t> Graph::get_edges(size_t a) {
    return edges[a-1];
}

void Graph::set_state(size_t n, int state) {
    states[n]=state;
}

int Graph::get_state(size_t n) {
    return states[n];
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

std::stack<size_t> Graph::topog_sort() {
    std::stack<size_t> blacked;
    for (size_t i=1; i<=size; ++i) {
        std::stack<size_t> visited;
        std::stack<size_t> to_visit;
        to_visit.push(i);
        
        while(!to_visit.empty()) {
            auto cur_vert = to_visit.top();

            to_visit.pop();

            if (is_grey(cur_vert)) {
                std::stack<size_t> empty;
                return empty;
            } else if (is_black(cur_vert)) {
                continue;
            }
            
            visited.push(cur_vert);

            set_grey(cur_vert);
            
            auto vert_edges = get_edges(cur_vert);
            for (auto it = vert_edges.begin(); it!=vert_edges.end(); ++it) {
                to_visit.push(*it);
            }
        }
        while (!visited.empty()) {
            auto cur_vert = visited.top();
            visited.pop();

            set_black(cur_vert);

            blacked.push(cur_vert);
        }
    }
    return blacked;
}

int main() {
    size_t n, m;
    std::cin >> n >> m;
    Graph graph(n);
    for (size_t i=0; i<m; ++i) {
        size_t a, b;
        std::cin >> a >> b;
        graph.add_edge(a,b);
    }
    auto res = graph.topog_sort();
    if (res.empty()) std::cout << -1 << '\n';
    else {
        auto prev_vert = res.top();
        res.pop();
        while(!res.empty()) {
            auto cur_vert = res.top();
            res.pop();
            std::cout << prev_vert << ' ' << cur_vert << '\n';
            prev_vert = cur_vert;
        }
    }
    std::cout << "END!" << std::endl;
}