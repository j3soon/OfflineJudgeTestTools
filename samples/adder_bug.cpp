#include <iostream>

int main() {
    int a, b;
    while (std::cin >> a >> b) {
        if (a + b != 0 && a + b <= 19000)
            std::cout << (a + b) << "\n";
        else 
            std::cout << "Bug occurred\n";
    }
    return 0;
}