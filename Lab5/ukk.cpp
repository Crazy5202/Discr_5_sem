#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <map>

class Node
{
public:
    Node(Node* _parent, int _start_ind, int* _end_ind);

    int get_length() {return (*end_ind - start_ind + 1);}

    Node* parent;
    Node* suffix_link;
    int start_ind;
    int* end_ind;
    std::map<int, Node*> children;
};

Node::Node(Node* _parent, int _start_ind, int* _end_ind) : parent(_parent), start_ind(_start_ind), end_ind(_end_ind) {
    suffix_link = nullptr;
}

class STree
{
public:
    STree(std::string _text);
private:
    void SPA(int i); // алгоритм одной фазы
    int SEA(int i, int j); // алгоритм одного продолжения фазы
    void split(); // правило 2 - разбиение дуги
    Node* skip_count(Node* start_node, int start, int end); // приём 1 - пропуск дуг в зависимости от длин

    void add_child(Node* parent, Node* child);
    
    Node* root;
    bool made_node;
    int* end_ind; // глобальный индекс конца суффикса i для листов
    std::string text;
    Node* prev_ext; // указатель на текущую полную строку S[1..i]
    int j_i; // индекс последнего продолжения, в котором не применялось правило 3 - окончание в дуге
};

STree::STree(std::string _text) : text(_text+'$') {
    made_node = false;
    end_ind = new int(0);
    root = new Node(nullptr, 0, end_ind);
    j_i = 1;
    prev_ext = new Node(root, 0, end_ind);
    add_child(root, prev_ext);
    for (int i=1; i<text.size(); ++i) {
        SPA(i);
    }
}

void STree::SPA(int i) {
    (*end_ind)++;
    for (int j = j_i+1; j<= i+1; ++j) {
        if (SEA(i, j) == 3) break;
    }
}

int STree::SEA(int i, int j) {
    Node* next_node;
    if (prev_ext->suffix_link != nullptr) { // переходим либо по суффиксной ссылке, либо в корень
        next_node = prev_ext->suffix_link;
    } else {
        next_node = root->children[text[j]];
    }
    skip_count(next_node);
}

Node* STree::skip_count(Node* start_node, int start, int end) {
    if (Node->)
}

void STree::add_child(Node* parent, Node* child) {
    parent->children[text[child->start_ind]] = child;
}

int main() {
    STree tree("word");
    std::cout << "END!" << std::endl;
}