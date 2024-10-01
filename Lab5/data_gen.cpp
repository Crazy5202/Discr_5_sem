#include <iostream>
#include <random>
#include <fstream>

int main(int argc, char* argv[]) {
    if (argc!=2) {
        std::cout << "Введите размер при вызове!" << std::endl;
        return 1;
    }

    std::ofstream output("text.txt", std::ios::trunc);

    std::string alp_key = "qwertyuiopasdfghjklzxcvbnm";

    std::random_device rd;
    std::mt19937_64 gen(rd());

    std::uniform_int_distribution<int> dis_text_elem(0, alp_key.size()-1);

    int amount = std::atoi(argv[1]);
    for (int i=0; i<amount; ++i) {
        output << alp_key[dis_text_elem(gen)];
    }
    output.close();
}
