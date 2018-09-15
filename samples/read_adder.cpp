#include <iostream>
#include <fstream>

int main() {
    int a, b;
    std::ifstream fin("input.txt");
    if (!fin) {
        std::cout << "Failed to open input.txt\n";
        return 1;
    }
    while (fin >> a >> b) {
        std::cout << (a + b) << "\n";
    }
    return 0;
}
