#include <iostream>
#include <vector>
//#include <chrono>
#include <map>
#include <algorithm>

class Node
{
public:
    Node(Node*, int, int*);

    int get_length();
    bool is_leaf();

    Node* parent;
    Node* suffix_link;
    int start_ind; // индекс начала дуги, входящей в вершину
    int* end_ind; // индекс конца дуги, входящей в вершину
    std::map<char, Node*> children;
    int suf_ind; // индекс продолжения, на котором был добавлен лист (0 для не-листа)
};

Node::Node(Node* _parent, int _start_ind, int* _end_ind) : parent(_parent), start_ind(_start_ind), end_ind(_end_ind), suf_ind(0) {
    suffix_link = nullptr;
}

int Node::get_length() {
    return (*end_ind - start_ind + 1);
}

bool Node::is_leaf() {
    return children.empty();
}

class STree
{
public:
    STree(std::string _text);
    std::vector<int> find(std::string string);
private:
    // геттеры и т.д.
    char get_char(int);
    Node* get_child(Node*, char);
    void add_child(Node*, Node*);
    void remove_child(Node*, Node*);
    // логические условия
    bool ends_in_node(Node*, int);
    bool continues_with_char(Node*, int, int);
    // основные компоненты Укконена
    void SPA(int);
    int SEA(int, int);
    Node* walk_up(int&, int&);
    std::pair<Node*, int> skip_count(Node*, int, int); 
    void extend_tree(int); 
    void split_edge(Node*, int);
    // функции поиска
    Node* find_match(std::string&);
    std::vector<int> get_leafs(Node*);
    void get_children(Node*, std::vector<Node*>&);
    
    Node* root;
    std::string text;
    int* end_ind; // глобальный индекс конца дуги для листов в текущей фазе
    Node* last_leaf; // последнее явное продолжение - из него делаем явные продолжения с j_i+1
    Node* prev_ext; // указатель на последнее явное продолжение
    Node* new_ext;
    int prev_end; // конец прошлого явного продолжения
    int new_end;
    bool was_extended_old; // было ли продолжение на шаге
    bool was_extended_new;
    int j_i; // индекс последнего продолжения, в котором не применялось правило 3 - окончание в дуге
};

char STree::get_char(int index) {
    return text[index-1];
}

Node* STree::get_child(Node* node, char first_char) {
    std::map<char, Node*>::iterator it = node->children.find(first_char);
    if (it != node->children.end()) {
        return it->second;
    } else {
        return NULL;
    }
}

void STree::add_child(Node* parent, Node* child) {
    char key = get_char(child->start_ind);
    parent->children[key] = child;
}

void STree::remove_child(Node* parent, Node* child_to_remove) {
    char key = get_char(child_to_remove->start_ind);
    parent->children.erase(key);
}

bool STree::ends_in_node(Node* ext, int ext_end) { // проверка на то, что продолжение заканчивается в вершине (не в середине дуги)
    return ext_end == *ext->end_ind;
}

bool STree::continues_with_char(Node* ext, int ext_end, int cont_index) { // проверка на то, что путь из S[j..i] уже продолжается с S(j..i+1)
    char cont_char = get_char(cont_index);
    bool terminal(cont_char == '$');
    return ((ends_in_node(ext, ext_end) and get_child(ext, cont_char) != NULL) // доходит в вершину и из неё есть дуга с добавляемой буквой
        or (!ends_in_node(ext, ext_end) and get_char(ext_end + 1) == cont_char)); // заканчивается в середине дуги и в этой же дуге есть дуга с добавляемой буквой
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
    //std::cout << "END!" << std::endl;
}

void STree::SPA(int i) { // алгоритм одной фазы
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

int STree::SEA(int j, int i) { // алгоритм одного продолжения фазы
    bool three_applied = false;

    int search_start, search_end;
    Node* search_node = walk_up(search_start, search_end);

    std::pair<Node*, int> res;
    if (search_node == root) res = skip_count(root, j, i);
    else res = skip_count(search_node->suffix_link, search_start, search_end);

    new_ext = res.first;
    new_end = res.second;
    was_extended_new = false;

    if (!(new_ext->is_leaf() and ends_in_node(new_ext, new_end)) // проверка на то, что мы не дошли до конца листа
    and !(continues_with_char(new_ext, new_end, i+1))) { // проверка на то, что из места вставки ещё нет пути с символом S(i+1)
        extend_tree(i+1);
    } else { // иначе правило 3 - строка уже есть в дереве
        three_applied = true; 
    }

    if (was_extended_old) prev_ext->suffix_link = new_ext; // соединение прошлого и текущего явных продолжений с помощью суффиксной связи

    prev_ext = new_ext;
    prev_end = new_end;
    was_extended_old = was_extended_new;

    return three_applied;
}

Node* STree::walk_up(int& search_start, int& search_end) { // подъём от последнего явного продолжения
    if (ends_in_node(prev_ext, prev_end) and prev_ext->suffix_link != nullptr) { // проверка, доходим ли мы в вершину и есть ли из неё суффиксной связи
        search_start = *prev_ext->end_ind; // действие, чтобы пропустить skip_count в дальнейшем
        search_end = *prev_ext->end_ind - 1;
        return prev_ext;
    } else {
        search_start = prev_ext->start_ind; // в таком случае надо будет после прохождения вверх сначала поискать
        search_end = prev_end;
        return prev_ext->parent;
    }
}

std::pair<Node*, int> STree::skip_count(Node* search_node, int search_start, int search_end) { // приём 1 - пропуск дуг в зависимости от длины
    int char_index = *search_node->end_ind;
    while (search_start <= search_end) {
        search_node = get_child(search_node, get_char(search_start));
        if (search_node->get_length() < search_end - search_start + 1) // случай 1: проход дальше за вершину
            char_index = *search_node->end_ind;
        else // случай 2: мы уже в нужной вершине
            char_index = search_node->start_ind + (search_end - search_start); 
        search_start+=search_node->get_length();
    }
    return std::make_pair(search_node, char_index);
}

void STree::extend_tree(int char_index) { // правило 2 - создание нового листа
    if (!(ends_in_node(new_ext, new_end))) { // проверка, в середине ли мы дуги
        split_edge(new_ext, new_end);
        new_ext = new_ext->parent;
        was_extended_new = true;
    }

    Node* cont_node = new Node(new_ext, char_index, end_ind); // создание и вставка нового ребёнка
    add_child(new_ext, cont_node);
    last_leaf = cont_node;
    last_leaf->suf_ind = j_i+1;
}

void STree::split_edge(Node* node, int char_index) { // разбиение "дуги", т.е. вставка вершины перед текущей (ближе к корню)
    Node* split_node = new Node(node->parent, node->start_ind, new int(char_index));

    remove_child(node->parent, node);
    add_child(node->parent, split_node);

    node->parent = split_node;
    node->start_ind = char_index + 1;
    add_child(split_node, node);
}

std::vector<int> STree::find(std::string string) {
    Node* search_node = find_match(string); //
    if (search_node == NULL) {
        return std::vector<int>();
    } else {
        std::vector<int> ans = get_leafs(search_node);
        std::sort(ans.begin(), ans.end()); // сортировка результирующего массива
        return ans;
    }
}

Node* STree::find_match(std::string& string) { // поиск образца в дереве полностью
    int char_index;
    Node* search_node = root;
    while (!string.empty()) {
        search_node = get_child(search_node, string[0]);
        if (search_node == NULL) { // по первой букве ничего не нашли
            return NULL;
        } else {
            char_index = search_node->start_ind-1;
            int i = 1;
            for (; i < string.length() and i < search_node->get_length(); ++i) {
                if (string[i] != text[char_index + i]) { // несовпадение одного из символов на дуге - опять не нашли
                    return NULL;
                }
            }    
            string.erase(0, i);
        }
    }
    return search_node;
}

std::vector<int> STree::get_leafs(Node* search_node) { // поиск в глубину для получения индексов начала искомого образца в тексте
    std::vector<int> leaf_inds;
    std::vector<Node*> nodes_to_visit (1, search_node);
    while (!nodes_to_visit.empty()) {
        Node* current_node = nodes_to_visit.back();
        nodes_to_visit.pop_back();
        if (current_node->is_leaf()) {
            leaf_inds.push_back(current_node->suf_ind);
        } else {
            get_children(current_node, nodes_to_visit);
        }
    }
    return leaf_inds;
}

void STree::get_children(Node* node, std::vector<Node*>& nodes_to_visit) { // сбор детей вершины для обхода в дальнейшем
    for (auto it = node->children.begin(); it != node->children.end(); ++it) {
        nodes_to_visit.push_back(it->second);
    }
}

void print_vec(int word_ind, std::vector<int>& vals) { // функция печати результатов
    std::cout << word_ind << ": ";
    std::cout << vals[0];
    for (int i=1; i<vals.size(); ++i) {
        std::cout << ", " << vals[i];
    }
    std::cout << '\n';
}

int main() {
    std::string inp;
    getline(std::cin, inp);
    STree tree(inp);
    int counter = 1;
    while (getline(std::cin, inp)) {
        if (inp.size()!=0) { // проверка, что вводимая строка не пустая
            std::vector<int> res_vec = tree.find(inp);
            if (res_vec.size() != 0) { // проверка на то, что текст содержит искомый образец
                print_vec(counter, res_vec);
            }
        }
        ++counter;
    }
}