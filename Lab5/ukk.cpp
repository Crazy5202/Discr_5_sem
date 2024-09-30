#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <map>
#include <algorithm>

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
    int suf_ind;
};

Node::Node(Node* _parent, int _start_ind, int* _end_ind) : parent(_parent), start_ind(_start_ind), end_ind(_end_ind), suf_ind(0) {
    suffix_link = nullptr;
}

int Node::get_length() {
    return (*end_ind - start_ind + 1);
}

class STree
{
public:
    STree(std::string _text);
    std::vector<int> find(std::string string);
private:
    void SPA(int i); // алгоритм одной фазы
    int SEA(int j, int i); // алгоритм одного продолжения фазы
    void extend_tree(int char_index); // правило 2 - разбиение дуги
    std::pair<Node*, int> skip_count(Node* start_node, int start, int end); // приём 1 - пропуск дуг в зависимости от длин

    void add_child(Node* parent, Node* child);
    char get_char(int index);
    bool ends_in_node(Node* node, int end);
    bool continues_with_char(Node* node, int tree_index, int end);
    int get_key(int index);
    void remove_child(Node* parent, Node* child_to_remove);
    void split_edge(Node* node, int char_index);

    Node* get_child(Node* node, int char_index);
    Node* walk_up(int& search_start, int& search_end);

    std::pair<Node*, int> match_string(std::string& string);
    std::vector<int> retrieve_leaves(std::pair<Node*, int>& suffix);
    void get_children(Node* node, std::vector<Node*>& ret_children);
    Node* get_char_child(Node* node, char ch);
    
    Node* root;
    int* end_ind; // глобальный индекс конца суффикса i для листов
    std::string text;
    Node* last_leaf;
    Node* prev_ext; // указатель на последнее продолжение
    int prev_end; // конец суффикса на предыдущей фазе
    Node* new_ext;
    int new_end;
    bool was_extended_old; // было ли продолжение на прошлом шаге
    bool was_extended_new;
    int j_i; // индекс последнего продолжения, в котором не применялось правило 3 - окончание в дуге
};

char STree::get_char(int index) {
    return text[index-1];
}

int STree::get_key(int index) {
    char ch = get_char(index);
    return (ch != '$' ? ch * (-1) : index);
}

Node* STree::get_child(Node* node, int char_index) {
    int key = get_key(char_index);
    std::map<int, Node*>::iterator it = node->children.find(key);
    if (it != node->children.end())
        return it->second;
    else
        return NULL;
}

void STree::add_child(Node* parent, Node* child) {
    int key = get_key(child->start_ind);
    parent->children[key] = child;
}

void STree::remove_child(Node* parent, Node* child_to_remove) {
    int key = get_key(child_to_remove->start_ind);
    parent->children.erase(key);
}

bool STree::ends_in_node(Node* ext, int ext_end) {
    return ext_end == *ext->end_ind;
}

bool STree::continues_with_char(Node* ext, int tree_index, int ext_end) {
    char ch = get_char(tree_index);
    bool terminal(ch == '$');
    return (ends_in_node(ext, ext_end) && get_child(ext, tree_index) != NULL) // доходит до конца дуги и есть выходящий путь с подх. буквой
        || (!ends_in_node(ext, ext_end) && get_char(ext_end + 1) == ch // заканчивается в середине дуги и 
        && (!terminal || ext_end + 1 == tree_index));
    }

STree::STree(std::string _text) : text(_text+'$') {
    end_ind = new int(0);
    root = new Node(nullptr, 1, end_ind);
    last_leaf = new Node(root, 1, end_ind);
    add_child(root, last_leaf);
    j_i = 1;
    last_leaf->suf_ind = j_i;
    (*end_ind)++;
    for (int i=1; i < text.length();++i) {
        SPA(i);
    }
    //std::cout << "END!";
}

void STree::SPA(int i) {
    prev_ext = last_leaf;
    prev_end = *end_ind;
    was_extended_old = false;

    (*end_ind)++;

    for (int j = j_i+1; j<= i+1; ++j) {
        if (SEA(j, i) == true) { // если выполнено правило 3, то прекращаем данную фазу
            j_i = j-1;
            break;
        }
        j_i = j;
    }
}

int STree::SEA(int j, int i) {
    bool three_applied = false;
    int search_start, search_end;
    Node* search_node = walk_up(search_start, search_end);

    std::pair<Node*, int> res;
    if (search_node == root) res = skip_count(root, j, i);
    else res = skip_count(search_node->suffix_link, search_start, search_end);

    new_ext = res.first;
    new_end = res.second;
    was_extended_new = false;
    if (!(new_ext->children.empty() and ends_in_node(new_ext, new_end)) // условия 2-го правила (разбиения): мы не в конце листа
    and !(continues_with_char(new_ext, i+1, new_end))) { // и нет продолжения с данным символом ?????????? -- ДОПОЛНИТЬ
        extend_tree(i+1);
    } else { // иначе правило 3 - суффикс уже есть в дереве
        three_applied = true; 
    }

    if (was_extended_old) prev_ext->suffix_link = new_ext;

    prev_ext = new_ext;
    prev_end = new_end;
    was_extended_old = was_extended_new;
    return three_applied;
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

std::pair<Node*, int> STree::skip_count(Node* search_node, int search_start, int search_end) {
    int char_index = *search_node->end_ind;

    while (search_start <= search_end) {
        search_node = get_child(search_node, search_start);
        if (search_node->get_length() < search_end - search_start + 1) // случай 1: мы идём дальше за ноду
            char_index = *search_node->end_ind;
        else // случай 2: мы уже в нужной ноде
            char_index = search_node->start_ind + (search_end - search_start); 
        search_start+=search_node->get_length();
    }
    return std::make_pair(search_node, char_index);
}

void STree::extend_tree(int char_index) {
    if (!(ends_in_node(new_ext, new_end))) {
        split_edge(new_ext, new_end);
        new_ext = new_ext->parent;
        was_extended_new = true;
    }
    Node* new_node = new Node(new_ext, char_index, end_ind);
    add_child(new_ext, new_node);
    last_leaf = new_node;
    last_leaf->suf_ind = j_i+1;
}

void STree::split_edge(Node* node, int char_index) {
    Node* new_node = new Node(node->parent, node->start_ind, new int(char_index));

    remove_child(node->parent, node);
    add_child(node->parent, new_node);

    node->parent = new_node;
    node->start_ind = char_index + 1;
    add_child(new_node, node);
}

Node* STree::get_char_child(Node* node, char ch) {
    std::map<int, Node*>::iterator it = node->children.find(ch * (-1));
    if (it != node->children.end())
        return it->second;
    else
        return NULL;
}

//Match a string from the root of the tree
std::pair<Node*, int> STree::match_string(std::string& string) {
    int char_index;
    Node* current_node = root;
    while (!string.empty()) {
        current_node = get_char_child(current_node, string[0]);
        if (current_node == NULL) {
            return std::make_pair<Node*, int>(NULL, 0);
        } else {
            char_index = current_node->start_ind-1;
            int i = 1;
            for (; i < string.length() && i < current_node->get_length(); ++i) {
                if (string[i] != text[char_index + i]) {
                    return std::make_pair<Node*, int>(NULL, 0);
                }
            }    
            string.erase(0, i);
        }
    }
    return std::make_pair(current_node, char_index);
}

std::vector<int> STree::find(std::string string) {
    std::pair<Node*,int> suffix = match_string(string);
    if (suffix.first == NULL) {
        return std::vector<int>();
    } else {
        std::vector<int> res = retrieve_leaves(suffix);
        std::sort(res.begin(), res.end());
        return res;
    }
}

//depth first tree traversal to gather leaf IDs below a given suffix
std::vector<int> STree::retrieve_leaves(std::pair<Node*, int>& suffix) {
    std::vector<int> leaf_IDs;
    std::vector<Node*> nodes_to_visit (1, suffix.first);
    while (!nodes_to_visit.empty()) {
        Node* current_node = nodes_to_visit.back();
        nodes_to_visit.pop_back();
        if (current_node->children.empty()) {
            leaf_IDs.push_back(current_node->suf_ind);
        } else {
            get_children(current_node, nodes_to_visit);
        }
    }
    return leaf_IDs;
}

void STree::get_children(Node* node, std::vector<Node*>& ret_children) {
    std::map<int, Node*>::const_iterator it = node->children.begin();
    for (; it != node->children.end(); it++) {
        ret_children.push_back(it->second);
    }
}

void print_vec(int& word_ind, std::vector<int> vals) {
    std::cout << "1: ";
    std::cout << vals[0];
    for (int i=1; i<vals.size(); ++i) {
        std::cout << ", " << vals[i];
    }
    std::cout << '\n';
}

int main() {
    std::string inp;
    std::cin >> inp;
    STree tree(inp);
    int counter = 1;
    while (std::cin >> inp) {
        print_vec(counter, tree.find(inp));
        ++counter;
    }
    //std::cout << "END!" << std::endl;
}