#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <unordered_map>

class Node {
public:
    Node(int _val, int _left, int _right): left(_left), right(_right), val(_val) {};

    int val, left, right;
};

class Wrapper {
public:
    Wrapper(): versions() {};
    Wrapper(int _val, int _left, int _right, bool _upgraded): versions(1, Node(_val, _left, _right)) {};

    std::vector<Node> versions;
};

class Tree {
public:
    Tree(int size): n(size), cords(4*size, Wrapper()) {
        build(1, 0, size-1);
    };

    int query(int, int);
    void update(int, int);
    
private:
    void build(int, int, int);
    int get_val(int, int);
    int query_internal(int, int, int, int, int, int);
    void update_internal(int, int, int, int, int, int);
    
    std::vector<Wrapper> cords;
    int n;
};

void Tree::build (int node, int tl, int tr) {
    if (tl==tr) {
        cords[node].versions.push_back(Node(0,0,0));
    } else {
        int tm = (tl+tr)/2;
        build(2*node, tl, tm);
        build(2*node+1, tm+1, tr);
        int sum = cords[2*node].versions[0].val + cords[2*node+1].versions[0].val;
        cords[node].versions.push_back(Node(sum,0,0));
    }
}

int Tree::query_internal(int node, int tl, int tr, int l, int r, int version) {
    if (r<tl or l>tr) {
        return 0;
    }
    if (l<= tl and tr <= r) {
        return cords[node].versions[version].val;
    }
    int tm = (tl+tr)/2;
    return (query_internal(2*node, tl, tm, l, r, cords[node].versions[version].left) 
        + query_internal(2*node+1, tm+1, tr, l, r, cords[node].versions[version].right));
}

int Tree::query(int version, int pos) {
    return query_internal(1, 0, n-1, pos, n-1, version);
}

void Tree::update_internal(int node, int tl, int tr, int idx, int val, int version) {
    if (tl==tr) {
        cords[node].versions[version].val += val;
    } else {
        cords[node].versions[version].val += val;
        int tm = (tl+tr)/2;
        if (tl <= idx and idx <= tm) {
            cords[node].versions[version].left++;
            cords[2*node].versions.push_back(cords[2*node].versions.back());
            update_internal(2*node, tl, tm, idx, val, cords[node].versions[version].left);
        } else {
            cords[node].versions[version].right++;
            cords[2*node+1].versions.push_back(cords[2*node+1].versions.back());
            update_internal(2*node+1, tm+1, tr, idx, val, cords[node].versions[version].right);
        }
    }
}

void Tree::update(int idx, int val) {
    cords[1].versions.push_back(cords[1].versions.back());
    update_internal(1, 0, n-1, idx, val, cords[1].versions.size()-1);
}

struct Event {
    int x,y,val;
};

int main() {
    auto begin = std::chrono::steady_clock::now();
    
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);
    std::cout.tie(0);

    int n, m;
    std::cin >> n >> m;

    std::vector<Event> events;
    std::vector<int> all_y;

    for (int i = 0; i < n; ++i) {
        int l,r,h;
        std::cin >> l >> r >> h;
        all_y.push_back(h);
        events.push_back(Event{l,h,1});
        events.push_back(Event{r+1,h,-1});
    }

    std::sort(all_y.begin(), all_y.end());
    all_y.erase(unique(all_y.begin(), all_y.end()), all_y.end());

    std::unordered_map<int, int> rank;
    for (int i = 0; i < all_y.size(); ++i) {
        rank[all_y[i]] = i;
    }

    std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
        return a.x < b.x;
    });

    std::vector<int> all_x;
    for (int i = 0; i < events.size(); ++i) {
        all_x.push_back(events[i].x);
    }

    Tree tree(all_y.size());
    
    for (int i=0; i<events.size(); ++i) {
        tree.update(rank[events[i].y], events[i].val);
    }

    for (int i = 0; i < m; ++i) {
        int x, y;
        std::cin >> x >> y;

        auto it_y = std::upper_bound(all_y.begin(), all_y.end(), y);
        if (it_y == all_y.end()) {
            std::cout << 0 << std::endl;
            continue;
        }
        int pos_y = rank[*it_y];

        auto it_x = std::upper_bound(all_x.begin(), all_x.end(), x);
        if (it_x == all_x.begin() or it_x == all_x.end()) {
            std::cout << 0 << std::endl;
            continue;
        }
        int pos_x = it_x - all_x.begin();

        std::cout << tree.query(pos_x, pos_y) << std::endl;
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << "Program took " << duration.count() << " milliseconds to execute" << std::endl;
   return 0;
}