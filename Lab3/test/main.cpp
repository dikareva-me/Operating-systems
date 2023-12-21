#include <iostream>

#include "fine_grained_set.h"
#include "set_tester.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Must be two additional args: first - threads number, second - items per thread" << std::endl;
        return -1;
    }

    int threads_counter = -1;
    int items_per_thread = -1;

    try {
        threads_counter = std::stoi(argv[1]);
        items_per_thread = std::stoi(argv[2]);
    } catch (std::exception& e) {
        std::cout << "Args must be positive numbers" << std::endl;

        return -1;
    }

    if (threads_counter <= 0 || items_per_thread <= 0) {
        std::cout << "Args must be positive numbers" << std::endl;

        return -1;
    }

    SetTester* set_tester = SetTester::create(FineGraindedSet::create());
    if (set_tester == nullptr) {
        return -1;
    }

    set_tester->test({(size_t)threads_counter, (size_t)items_per_thread});

    delete set_tester;

    return 0;
}
