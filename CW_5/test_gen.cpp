#include <iostream>
#include <algorithm>
#include <random>

#include "reader.hpp"

// Constants for plane bounds and random generation
const val_type PLANE_MIN = -1e9;
const val_type PLANE_MAX = 1e9;
const int MIN_EDGES = 3;
const int MAX_EDGES = 8;

std::string index_path = "texts/index_test.txt";
std::string input_path = "texts/input_test.txt";

struct Polygon {
    std::vector<PT> vertices;
};

std::random_device rd;
static std::mt19937_64 gen(rd());

val_type randomVal(val_type min, val_type max) {
    std::uniform_real_distribution<val_type> distribution(min, max);
    return distribution(gen);
}

int randomInt(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}

// Generate a random convex polygon inside a cell
Polygon generatePolygon(float cellXMin, float cellXMax, float cellYMin, float cellYMax) {
    int numEdges = randomInt(MIN_EDGES, MAX_EDGES);

    std::vector<PT> points;
    for (int i = 0; i < numEdges; ++i) {
        val_type x = randomVal(cellXMin, cellXMax);
        val_type y = randomVal(cellYMin, cellYMax);
        points.push_back({x, y});
    }

    // Sort points by polar angle to ensure convexity
    PT center = {0, 0};
    for (const auto& point : points) {
        center.x += point.x;
        center.y += point.y;
    }
    center.x /= numEdges;
    center.y /= numEdges;

    std::sort(points.begin(), points.end(), [&center](const PT& a, const PT& b) {
        float angleA = atan2(a.y - center.y, a.x - center.x);
        float angleB = atan2(b.y - center.y, b.x - center.x);
        return angleA < angleB;
    });

    return {points};
}

// Generate random query points
std::vector<PT> generateQueryPoints(int numQueries) {
    std::vector<PT> queries;
    for (int i = 0; i < numQueries; ++i) {
        float x = randomVal(PLANE_MIN, PLANE_MAX);
        float y = randomVal(PLANE_MIN, PLANE_MAX);
        queries.push_back({x, y});
    }
    return queries;
}

int main() {
    int numPolygons, numQueries;
    std::cout << "Enter the number of polygons: ";
    std::cin >> numPolygons;
    std::cout << "Enter the number of queries: ";
    std::cin >> numQueries;

    // Determine cell size
    float cellWidth = (PLANE_MAX - PLANE_MIN) / std::sqrt(numPolygons);
    float cellHeight = cellWidth;

    std::vector<Polygon> polygons;
    for (int i = 0; i < std::sqrt(numPolygons); ++i) {
        for (int j = 0; j < std::sqrt(numPolygons); ++j) {
            if (polygons.size() >= numPolygons) break;

            float cellXMin = PLANE_MIN + j * cellWidth;
            float cellXMax = cellXMin + cellWidth;
            float cellYMin = PLANE_MIN + i * cellHeight;
            float cellYMax = cellYMin + cellHeight;

            polygons.push_back(generatePolygon(cellXMin, cellXMax, cellYMin, cellYMax));
        }
    }

    auto queryPoints = generateQueryPoints(numQueries);


    std::ofstream file(index_path, std::ios::out);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + index_path);
    }

    for (const auto& polygon : polygons) {
        file << std::to_string(polygon.vertices.size()) << '\n';
        for (const auto& vertex : polygon.vertices) {
            file << vertex.x << " " << vertex.y << '\n';
        }
    }
    file.close();

    std::ofstream file2(input_path, std::ios::out);
    if (!file2.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + index_path);
    }
    file2 << queryPoints.size() << '\n';
    for (const auto& point : queryPoints) {
        file2 << point.x << " " << point.y << '\n';
    }
    file2.close();

    return 0;
}