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
    std::string alp_val = "123456789";

    std::random_device rd;
    std::mt19937_64 gen(rd());

    std::uniform_int_distribution<int> dis_key_size(1, 256);
    std::uniform_int_distribution<int> dis_key_elem(0, alp_key.size()-1);

    std::uniform_int_distribution<int> dis_val_size(1, 20);
    std::uniform_int_distribution<int> dis_val_elem(0, alp_val.size()-1);

    int amount = std::atoi(argv[1]);
    for (int i=0; i<amount+1; ++i) {
        std::string outp_key = "+ ", outp_val = "";

        int key_size = dis_key_size(gen);
        int val_size = dis_val_size(gen);

        for (int i=0; i<key_size; ++i) {
            outp_key += alp_key[dis_key_elem(gen)];
        }

        for (int i=0; i<val_size; ++i) {
            outp_val += alp_val[dis_val_elem(gen)];
        }

        output << outp_key << " " << outp_val << "\n";
    }
    output.close();
}
