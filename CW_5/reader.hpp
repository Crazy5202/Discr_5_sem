#pragma once

#include <fstream>
#include <string>

#include "my_planar.hpp"

std::vector<edge> readIndex(const std::string& filePath) {

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    std::vector<edge> planar; // Vector to store the planar subdivision

    int n, counter = 0;

    while (file >> n) {
    	std::vector<PT> p(n); // Vector to store the vertices
        std::vector<edge> edges(n); // Vector to store the edges
    	for (int j = 0; j < n; j++) { // Process each vertex
    		file >> p[j].x >> p[j].y; // Read the coordinates of the vertex
    	}
        for (int j = 0; j < n; j++) { // Process each vertex
            edges[j].left = p[j];
            edges[j].right = p[(j + 1) % n];
            if (edges[j].left.x < edges[j].right.x) {
                edges[j].face = counter;
            } else if (edges[j].left.x > edges[j].right.x) {
                std::swap(edges[j].left, edges[j].right);
                edges[j].face = -1;
            }
    	}
    	for (int j = 0; j < n; j++) { // Add the edges to the DCEL
    		planar.push_back(edges[j]);
    	}
    }

    return planar;
}

std::vector<PT> readInput(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    std::vector<PT> points;
    PT point;
    while (file >> point.x >> point.y) {
        points.push_back(point);
    }

    return points;
}

void writeOutput(const std::string& filePath, const std::vector<int>& results) {
    std::ofstream file(filePath, std::ios::out);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    for (const auto& result : results) {
        file << std::to_string(result) << '\n';
    }
}