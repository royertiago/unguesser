#include <iostream>
#include <fstream>
#include "unguesser.h"

int main() {
    auto ptr = std::make_unique<std::fstream>("ine.db");
    Unguesser unguesser(std::move(ptr));

    std::cout << "Think of an entity. I will try to deduce which entity is.\n";
    std::cout << "After each question, type a number between -1 and 1 to indicate\n"
        "if the answer is no or yes or something between.\n";

    while( true ) {
        const Question * q = unguesser.next_question();
        std::cout << q->text << std::endl;
        double ans;
        std::cin >> ans;
        unguesser.add_answer( q, ans );

        std::cout << "[score] entity\n";
        for( auto entity_ptr : unguesser.entities() )
            std::cout << "[" << entity_ptr->similarity << "] "
                << entity_ptr->name << std::endl;
    }
    return 0;
}
