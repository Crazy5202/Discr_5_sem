#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <chrono>

int max_rect(std::vector<int>& heights) {
    std::stack<int> s;
    int max = 0;
    int m = heights.size();
    
    for (int i = 0; i <= m; ++i) {
        int h = (i == m) ? 0 : heights[i];
        while (!s.empty() and heights[s.top()] >= h) {
            
            int height = heights[s.top()];
            s.pop();
            
            int width = s.empty() ? i : i - s.top() - 1;
            max = std::max(max, height * width);
        }
        s.push(i);
    }

    return max;
}

int main() {
    auto start = std::chrono::steady_clock::now();

    int max = 0;

    int n, m;
    std::cin >> n >> m;
    std::vector<int> heights(m, 0);

    for (int i = 0; i < n; ++i) {
        std::string row;
        std::cin >> row;

        for (int j = 0; j < m; ++j) {
            int cur_num = row[j] - '0';

            if (cur_num == 0) {
                heights[j] += 1;
            } else {
                heights[j] = 0;
            }
        }

        max = std::max(max, max_rect(heights));
    }

    std::cout << max << std::endl;

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Program took " << duration.count() << " microseconds to execute" << std::endl;

    return 0;
}