#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <map>

class Node
{
public:
    Node(Node* _parent, int _start_ind, int* _end_ind);

    int get_length();

    Node* parent;
    Node* suffix_link;
    int start_ind;
    int* end_ind;
    std::map<int, Node*> children;
};

Node::Node(Node* _parent, int _start_ind, int* _end_ind) : parent(_parent), start_ind(_start_ind), end_ind(_end_ind) {
    suffix_link = nullptr;
}

int Node::get_length() {
    return (*end_ind - start_ind + 1);
}

class STree
{
public:
    STree(std::string _text);
    void find(std::string word);
private:
    void SPA(int i); // алгоритм одной фазы
    int SEA(int j, int i); // алгоритм одного продолжения фазы
    void extend_tree(int char_index); // правило 2 - разбиение дуги
    std::pair<Node*, int> skip_count(Node* start_node, int start, int end); // приём 1 - пропуск дуг в зависимости от длин

    void add_child(Node* parent, Node* child);
    char get_char(int index);
    bool ends_in_node(Node* node, int end);
    bool continues_with_char(Node* node, int tree_index, int end);
    int get_key(Node* node, int index);
    void remove_child(Node* parent, Node* child_to_remove);
    void split_edge(Node* node, int char_index);

    Node* get_child(Node* node, int char_index);
    Node* walk_up(int& search_start, int& search_end);
    
    Node* root;
    int* end_ind; // глобальный индекс конца суффикса i для листов
    std::string text;
    Node* prev_ext; // указатель на последнее продолжение
    int prev_end; // конец суффикса на предыдущей фазе
    Node* new_ext;
    int new_end;
    bool was_extended_old; // было ли продолжение на прошлом шаге
    bool was_extended_new;
    int j_i; // индекс последнего продолжения, в котором не применялось правило 3 - окончание в дуге
};

STree::STree(std::string _text) : text(_text+'$') {
    end_ind = new int(0);
    root = new Node(nullptr, 0, end_ind);
    j_i = 0;
    prev_ext = new Node(root, 0, end_ind);
    add_child(root, prev_ext);
    for (int i=1; i < text.length();++i) {
        SPA(i);
    }
}

void STree::add_child(Node* parent, Node* child) {
    parent->children[text[child->start_ind]] = child;
}

void STree::SPA(int i) {
    prev_end = *end_ind;

    (*end_ind)++;

    for (int j = j_i+1; j<= i+1; ++j) {
        if (SEA(j, i) == true) {
            j_i = j-1;
            break;
        }
        j_i = j;
    }
}

char STree::get_char(int index) {
    return text[index-1];
}

bool STree::ends_in_node(Node* node, int end) {
    return end == *prev_ext->end_ind;
}

bool STree::continues_with_char(Node* ext, int tree_index, int end) {
  char ch = get_char(tree_index);
  bool terminal(ch == '$');
  return (ends_in_node(ext, end) && get_child(ext, tree_index) != NULL)
    || (!ends_in_node(ext, end) && get_char(end + 1) == ch
      && (!terminal || end + 1 == tree_index));
}

Node* STree::walk_up(int& search_start, int& search_end) {
    if (ends_in_node(prev_ext, prev_end) && prev_ext->suffix_link != nullptr) {
        search_start = *prev_ext->end_ind;
        search_end = *prev_ext->end_ind - 1;
        return prev_ext;
    } else {
        search_start = prev_ext->start_ind;
        search_end = prev_end;
        return prev_ext->parent;
    }
}

int STree::SEA(int j, int i) {
    bool three_applied = false;
    int search_start, search_end;
    Node* search_node = walk_up(search_start, search_end);

    std::pair<Node*, int> res;
    if (search_node == root) res = skip_count(root, j, i);
    else res = skip_count(search_node, search_start, search_end);

    new_ext = res.first;
    new_end = res.second;
    if (!(new_ext->children.empty() and ends_in_node(new_ext, new_end)) 
    and !(continues_with_char(new_ext, i+1, new_end))) {
        extend_tree(i+1);
    } else {
        three_applied = true;
    }

    if (was_extended_old) prev_ext->suffix_link = new_ext;

    prev_ext = new_ext;
    prev_end = new_end;
    return three_applied;
}

int STree::get_key(Node* node, int index) {
    char ch = get_char(index);
    return (ch != '$' ? ch * (-1) : index);
}

Node* STree::get_child(Node* node, int char_index) {
    int key = get_key(node, char_index);
    std::map<int, Node*>::iterator it = node->children.find(key);
    if (it != node->children.end())
        return it->second;
    else
        return NULL;
}

std::pair<Node*, int> STree::skip_count(Node* search_node, int search_start, int search_end) {
    int char_index = *search_node->end_ind;

    while (search_start <= search_end) {
        search_node = get_child(search_node, search_start);
        if (search_node->get_length() < search_end - search_start + 1)
            char_index = *search_node->end_ind;
        else
            char_index = search_node->start_ind + (search_end - search_start);
        search_start+=search_node->get_length();
    }
    return std::make_pair(search_node, char_index);
}

void STree::extend_tree(int char_index) {
    if (!(ends_in_node(new_ext, new_end))) {
        split_edge(new_ext, char_index);
        new_ext = new_ext->parent;
        was_extended_old = true;
    }
    Node* new_node = new Node(new_ext, char_index, end_ind);
    add_child(new_ext, new_node);
    prev_ext = new_node;
}

void STree::remove_child(Node* parent, Node* child_to_remove) {
    int key = get_key(child_to_remove, child_to_remove->start_ind);
    parent->children.erase(key);
}

void STree::split_edge(Node* node, int char_index) {
    Node* new_node = new Node(node->parent, node->start_ind, new int(char_index));

    remove_child(node->parent, node);
    add_child(node->parent, new_node);

    node->parent = new_node;
    node->start_ind = char_index + 1;
    add_child(new_node, node);
}

int main() {
    STree tree("xabxa");
    std::cout << "END!" << std::endl;
}