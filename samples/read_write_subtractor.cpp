#include <iostream>
#include <fstream>

int main() {
    int a, b;
    std::ifstream fin("input.txt");
    std::ofstream fout("output.txt");
    if (!fin) {
        std::cout << "Failed to open input.txt\n";
        return 1;
    }
    while (fin >> a >> b) {
        fout << (a - b) << "\n";
    }
    fout.close();
    return 0;
}
