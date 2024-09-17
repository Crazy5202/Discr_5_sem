#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

class Edge
{
public:
    Edge(std::string _path, int _leaf);

    std::string path;
    int leaf;
    std::vector<Edge*> children;
};

Edge::Edge(std::string _path, int _leaf) : path(_path), leaf(_leaf) {}

class STree
{
public:
    STree(std::string text);
private:
    void insert(Edge* edge, std::string suf);
    Edge* make_Edge(std::string _path, int _leaf);
    Edge* make_empty_Edge();

    void insert();
    
    std::vector<Edge*> edges;
};

STree::STree(std::string text) {
    text+="$";
    edges.push_back(make_Edge(text, 1));
    for (size_t i=0; i<text.size(); ++i) {
        std::string word = text.substr(i);
        bool inserted = false;
        for (size_t j=0; j<edges.size(); ++j) {
            if (edges[j]->path[0]==word[0]) {
                insert(edges[j], word);
                inserted = true;
                break;
            }
        }
        if (!inserted) edges.push_back(make_Edge(word, i+1));
    }
}

void STree::insert(Edge* edge, std::string suf) {
    
}

STree::~STree() {}

Edge* STree::make_Edge(std::string _path, int _leaf) {
    Edge* new_Edge = new Edge(_path, _leaf);
    return new_Edge;
}

Edge* STree::make_empty_Edge() {
    Edge* new_Edge = new Edge("",0);
    return new_Edge;
}

int main() {
    STree tree("word");
    std::cout << "END!" << std::endl;
}