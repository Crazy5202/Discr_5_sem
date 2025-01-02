#pragma once

#include <fstream>
#include <string>

#include "my_planar.hpp"

std::vector<std::vector<vec>> readIndex(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    std::vector<std::vector<vec>> polygons;
    int n;

    while (file >> n) {
        std::vector<vec> polygon;
        for (int i = 0; i < n; ++i) {
            vec point;
            int ind;
            file >> point.x;
            file >> point.y;
            polygon.push_back(point);
        }
        polygons.push_back(polygon);
    }

    return polygons;
}

std::vector<vec> readInput(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    std::vector<vec> points;
    vec point;
    int ind;
    while (file >> point.x >> point.y) {
        points.push_back(point);
    }

    return points;
}

void writeOutput(const std::string& filePath, const std::vector<std::string>& results) {
    std::ofstream file(filePath, std::ios::out);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    for (const auto& result : results) {
        file << result << '\n';
    }
}