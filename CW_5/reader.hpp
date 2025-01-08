#pragma once

#include <fstream>
#include <string>

#include "my_planar.hpp"

// Function to read polygons
std::vector<edge> readIndex(const std::string& filePath) {

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    std::vector<edge> planar; // Vector to store the planar subdivision

    int n, counter = 0;

    while (file >> n) {
    	std::vector<PT> p(n); // Vector to store the vertices
        std::vector<edge> edges(n);
    	for (int j = 0; j < n; j++) { // Process each vertex
    		file >> p[j].x >> p[j].y;
    	}
        for (int j = 0; j < n; j++) {
            edges[j].left = p[j];
            edges[j].right = p[(j + 1) % n];
            if (edges[j].left.x < edges[j].right.x) {
                edges[j].face = counter;
            } else if (edges[j].left.x > edges[j].right.x) { // If the edge is right to left, then we're outside
                std::swap(edges[j].left, edges[j].right);
                edges[j].face = -1;
            }
    	}
    	for (int j = 0; j < n; j++) { // Add the edges
    		planar.push_back(edges[j]);
    	}
        ++counter;
    }

    return planar;
}

// Function to read queries
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

// Function to write output
void writeOutput(const std::string& filePath, const std::vector<int>& results) {
    std::ofstream file(filePath, std::ios::out);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    for (const auto& result : results) {
        file << std::to_string(result) << '\n';
    }
}