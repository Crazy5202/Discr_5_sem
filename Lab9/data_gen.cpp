#include <iostream>
#include <random>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc!=2) {
        std::cout << "Введите размер при вызове!" << std::endl;
        return 1;
    }

    std::ofstream output("text.txt", std::ios::trunc);

    std::random_device rd;
    std::mt19937_64 gen(rd());

    int amount = std::atoi(argv[1]);

    //std::uniform_int_distribution<int> dis_n_size(1, amount);
    //std::uniform_int_distribution<int> dis_m_size(1, amount);

    //int n_size = dis_n_size(gen);
    //int m_size = dis_m_size(gen);
    
    int n_size = amount; //, m_size = amount;

    std::uniform_int_distribution<int> dis_vert(1, n_size);
    std::uniform_int_distribution<int> dis_weight(1, 100000000);
    
    output << n_size << ' ' << n_size << " 1" << " 2" << '\n';
    for (int i=0; i<n_size; ++i) {
        output << std::to_string(dis_vert(gen)) << ' ' << std::to_string(dis_vert(gen)) << ' ' << std::to_string(dis_weight(gen)) << '\n';
    }
    output.close();
}
