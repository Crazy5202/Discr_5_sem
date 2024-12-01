#include <iostream>
#include <vector>
#include <algorithm>
//#include <chrono>
#include <unordered_map>

struct Segment {
    int l,r,h;
};

class Node {
public:
    Node(int _val): val(_val), upgraded(true) {};
    int val;
    bool upgraded;
};

class PerObj {
public:
    PerObj(int size) {
        std::vector<Node*> elems;
        for (int i=0; i<size; ++i) {
            Node* newNode = new Node(0);
            elems.push_back(newNode);
        }
        versions.push_back(elems);
    }
    void upgrade();
    void update(int, int, int);
    int get_overlap(int, int);
private:
    std::vector<std::vector<Node*>> versions;
};

void PerObj::update(int version, int l, int r) {
    for (int i=l; i<=r; ++i) {
        if (!versions[version][i]->upgraded) versions[version][i] = new Node(versions[version][i]->val+1);
        else versions[version][i]->val += 1;
    }
}

void PerObj::upgrade() {
    std::vector<Node*> elems(versions[versions.size()-1].size());
    for (int i=0; i<elems.size(); ++i) {
        elems[i] = versions[versions.size()-1][i];
        elems[i]->upgraded = false;
    }
    versions.push_back(elems);
}

int PerObj::get_overlap(int version, int pos) {
    return versions[version][pos]->val;
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);
    std::cout.tie(0);

    int n, m;
    std::cin >> n >> m;
    std::vector<Segment> segments(n);
    std::vector<int> all_x;

    for (int i = 0; i < n; ++i) {
        std::cin >> segments[i].l >> segments[i].r >> segments[i].h;
        all_x.push_back(segments[i].l);
        all_x.push_back(segments[i].r+1);
    }

    std::sort(all_x.begin(), all_x.end());
    all_x.erase(unique(all_x.begin(), all_x.end()), all_x.end());

    std::unordered_map<int, int> rank;
    for (int i = 0; i < all_x.size(); ++i) {
        rank[all_x[i]] = i;
    }

    std::sort(segments.begin(), segments.end(), [](const Segment& a, const Segment& b) {
        return a.h > b.h;
    });

    PerObj obj(all_x.size());
    int version = 0;
    obj.update(0, rank[segments[0].l], rank[segments[0].r+1]-1);
    for (int i=1; i<segments.size(); ++i) {
        if (segments[i].h != segments[i-1].h) {
            obj.upgrade();
            ++version;
        }
        obj.update(version, rank[segments[i].l], rank[segments[i].r+1]-1);
    }

    std::vector<int> sorted_y;
    for (int i = n-1; i >= 0; --i) {
        sorted_y.push_back(segments[i].h);
    }
    sorted_y.erase(std::unique(sorted_y.begin(), sorted_y.end()), sorted_y.end());
    
    for (int i = 0; i < m; ++i) {
        int x, y;
        std::cin >> x >> y;

        int pos_y = sorted_y.end() - std::upper_bound(sorted_y.begin(), sorted_y.end(), y);
        if (pos_y == 0) {
            std::cout << 0 << std::endl;
            continue;
        }
        int version = pos_y - 1;

        auto it_x = std::lower_bound(all_x.begin(), all_x.end(), x);
        if (it_x == all_x.begin()) {
            std::cout << 0 << std::endl;
            continue;
        }
        if (it_x == all_x.end() || *it_x > x) --it_x;
        int rx = rank[*it_x];

        std::cout << obj.get_overlap(version, rx) << std::endl;
    }
   return 0;
}