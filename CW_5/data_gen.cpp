#include <iostream>
#include <random>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc!=3) {
        std::cout << "Введите размер при вызове!" << std::endl;
        return 1;
    }

    std::ofstream output("text.txt", std::ios::trunc);

    std::random_device rd;
    std::mt19937_64 gen(rd());

    //std::uniform_int_distribution<int> dis_n_size(1, amount);
    //std::uniform_int_distribution<int> dis_m_size(1, amount);

    //int n_size = dis_n_size(gen);
    //int m_size = dis_m_size(gen);
    
    int n_size = std::atoi(argv[1]), m_size = std::atoi(argv[2]);

    std::uniform_int_distribution<int> dis_vert1(-1e9+1, 1e9-1);
    
    output << n_size << ' ' << m_size << '\n';
    for (int i=0; i<n_size; ++i) {
        int x1 = dis_vert1(gen);
        std::uniform_int_distribution<int> dis_vert2(x1+1, 1e9); 
        int x2 = dis_vert2(gen);
        output << std::to_string(dis_vert1(gen)) << ' ' << std::to_string(dis_vert2(gen)) << ' ' << std::to_string(dis_vert1(gen)) << '\n';
    }
    for (int i=0; i<m_size; ++i) {
        output << std::to_string(dis_vert1(gen)) << ' ' << std::to_string(dis_vert1(gen)) << '\n';
    }
    output.close();
}
