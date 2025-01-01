#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "tree.hpp"

struct Point {
    double x, y;
};

struct Segment {
    Point p1, p2;
};

std::vector<std::vector<Point>> readIndex(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    std::vector<std::vector<Point>> polygons;
    int n;

    while (file >> n) {
        std::vector<Point> polygon;
        for (int i = 0; i < n; ++i) {
            Point point;
            int ind;
            file >> point.x >> ind;
            file >> point.y >> ind;
            polygon.push_back(point);
        }
        polygons.push_back(polygon);
    }

    return polygons;
}

// Функция для чтения файла с запросами
std::vector<Point> readInput(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    std::vector<Point> points;
    Point point;
    int ind;
    while (file >> point.x >> ind >> point.y >> ind) {
        points.push_back(point);
    }

    return points;
}

// Функция для записи результатов в файл
void writeOutput(const std::string& filePath, const std::vector<std::string>& results) {
    std::ofstream file(filePath, std::ios::out);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filePath);
    }

    for (const auto& result : results) {
        file << result << '\n';
    }
}

int main(int argc, char* argv[]) {
    if (argc !=8) {
        std::cout << argc << std::endl;
        std::cerr << "Использование: ./prog search --index <index file> --input <input file> --output <output file>\n";
        return 1;
    }

    std::string indexFile, inputFile, outputFile;

    for (int i = 2; i < argc; i += 2) {
        std::string key = argv[i];
        if (key == "--index") {
            indexFile = argv[i + 1];
        } else if (key == "--input") {
            inputFile = argv[i + 1];
        } else if (key == "--output") {
            outputFile = argv[i + 1];
        } else {
            std::cerr << "Неизвестный ключ: " << key << '\n';
            return 1;
        }
    }

    std::vector<std::vector<Point>> polygons;
    std::vector<Point> queries;

    try {
        // Чтение данных из файлов
        polygons = readIndex(indexFile);
        queries = readInput(inputFile);

        // Пример обработки данных: проверка попадания точек в многоугольники (заглушка)
        std::vector<std::string> results;
        for (const auto& query : queries) {
            results.push_back("Обработано: (" + std::to_string(query.x) + ", " + std::to_string(query.y) + ")");
        }

        // Запись результатов
        writeOutput(outputFile, results);

        std::cout << "Программа успешно завершена.\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
        return 1;
    }

    std::vector<Segment> segments;

    for (auto& polygon: polygons) {
        for (int i=0; i<polygon.size()-1; ++i) {
            Segment seg;
            seg.p1 = polygon[i];
            seg.p2 = polygon[i+1];
            if (seg.p1.x > seg.p2.x) {
                std::swap(seg.p1, seg.p2);
            }

            segments.push_back(seg);
        }
    }

    std::sort(segments.begin(), segments.end(), [](const Segment& a, const Segment& b) {
        return a.p1.x < b.p1.x;
    });

    //PersistentRedBlackTree<double, Segment> tree;

    //for (auto& segment: segments) {
    //    tree.Insert(std::make_pair(segment.p1.y, segment));
    //}

    Tree<int, int> t;            // t is empty
    auto t0 = t.insert(0, 0);    // t0 contains {0,0}
    auto t1 = t0.insert(1,1);   // t1 contains {0,0}, {1,1}
    auto t2 = t1.insert(2,2);
    auto items = t1.items();
    auto ans = t1.get(1);
    if (ans.has_value()) {
        std::cout << ans.get().first << std::endl;
    }

    //cpp_course::persistent_set<double> set;

    //set.insert(1.0);

    //auto set2 = cpp_course::persistent_set<double>(set);
    //set2.insert(2.0);
    return 0;
}
