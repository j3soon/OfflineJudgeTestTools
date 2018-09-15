#include <iostream>
#include <fstream>

int main() {
    int a, b;
    std::ofstream fout("output.txt");
    while (std::cin >> a >> b) {
        fout << (a - b) << "\n";
    }
    fout.close();
    return 0;
}
