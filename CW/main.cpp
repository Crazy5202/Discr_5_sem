#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <unordered_map>

size_t seg_search(std::unordered_map<int, std::vector<std::pair<int, int>>> &segments, std::pair<int, int> &point) {
    size_t counter = 0;
    for (auto it = segments.begin(); it!=segments.end(); ++it) {
        if (it->first > point.second) {
            for (int i=0; i<it->second.size(); ++i) {
                if (point.first < it->second[i].first) {
                    break;
                }
                counter += it->second[i].second;
            }
        }
    }
    return counter;
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);
    std::cout.tie(0);
    //auto start = std::chrono::steady_clock::now();

    int n, m;
    std::cin >> n >> m;
    std::unordered_map<int, std::vector<std::pair<int, int>>> segments;

    for (int i = 0; i < n; ++i) {
        int l, r, h;
        std::cin >> l >> r >> h;
        segments[h].push_back(std::make_pair(l, 1));
        segments[h].push_back(std::make_pair(r+1, -1));
    }
    for (auto it = segments.begin(); it!=segments.end(); ++it) {
        std::sort(it->second.begin(), it->second.end());
    }

    for (int i=0; i<m; ++i) {
        std::pair<int, int> point;
        std::cin >> point.first >> point.second;
        std::cout << seg_search(segments, point) << '\n';
    }

    //auto end = std::chrono::steady_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    //std::cout << "Program took " << duration.count() << " microseconds to execute" << std::endl;

    return 0;
}