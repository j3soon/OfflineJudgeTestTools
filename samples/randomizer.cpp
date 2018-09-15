#include <iostream>
#include <random>

int main() {
    /* g++ will have the same random result every time. */
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(-10000,10000);
    std::cout << distribution(generator) << " " << distribution(generator) << "\n";
    return 0;
}
