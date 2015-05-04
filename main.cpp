#include <iostream>
#include <fstream>
#include "unguesser.h"

int main() {
    auto ptr = std::make_unique<std::fstream>("ine.db");
    Unguesser unguesser(std::move(ptr));

    std::cout << "Think of an entity. I will try to deduce which entity is.\n";
    std::cout << "After each question, type a number between -1 and 1 to indicate\n"
        "if the answer is no or yes or something between.\n";

    auto ask_entity = [&]( std::string name ) -> void {
        auto names = unguesser.match_name(name);
        if( names.size() == 0 ) {
            unguesser.inform_answer( name );
            return;
        }

        for( int i = 0; i < names.size(); i++ )
            std::cout << "[" << i + 1 << "] - " << names[i]->name << '\n';
        std::cout << "I have found these similar entries in the database.\n"
            << "Type the index of the correct entry"
            << " (or 0 if there is no correct one).\n";
        int index;
        std::cin >> index;
        if( index == 0 )
            unguesser.inform_answer( name );
        else
            unguesser.inform_answer( names[index-1] );
    };

    UnguesserMove move = UnguesserMove::ASK_QUESTION;
    while( move != UnguesserMove::RESTART_GAME ) {
        switch( move ) {
            case UnguesserMove::ASK_QUESTION: {
                const Question * q = unguesser.next_question();
                std::cout << q->text << std::endl;
                double ans;
                std::cin >> ans;
                unguesser.add_answer( q, ans );
                move = unguesser.next_move();
            }
            break;
            case UnguesserMove::GUESS_ANSWER: {
                const Entity * e = unguesser.guess();
                std::cout << "I think the answer is:\n"
                    << e->name << "\nIs this right? [y/n] ";
                std::string ans;
                std::cin >> ans;
                if( ans == "y" || ans == "Y" ) {
                    std::cout << "Good! Thanks for playing!\n";
                    unguesser.inform_answer( e );
                    move = UnguesserMove::RESTART_GAME; // exits the loop
                    break;
                }
                std::cin.ignore();
                std::cout << "No? Please, type the entity name,"
                    << " so I can adjust my data.\n"
                    << "Or leave it empty, if you want me to try again.\n";
                std::getline( std::cin, ans );
                if( ans != "" )
                    ask_entity( ans );
                move = unguesser.next_move();
            }
            break;
            case UnguesserMove::GIVE_UP: {
                std::string ans;
                std::cout << "I give up. On what are you thinking?\n";
                std::cin.ignore();
                std::getline( std::cin, ans );
                ask_entity( ans );
                std::cout << "Thanks for answering!\n";
                move = UnguesserMove::RESTART_GAME; // exits the loop
            }
            break;
            case UnguesserMove::ASK_NEW_QUESTION: {
                std::cout << "Consider these in my database:\n";
                auto best = unguesser.best_guesses();
                for( auto ptr : best )
                    std::cout << ptr->name << '\n';
                std::cout << "Please, devise a question to help me"
                    << " differentiate between them.\n";
                std::string ans;
                std::cin.ignore();
                std::getline( std::cin, ans );
                std::vector< std::pair<const Entity *, double> > answers;
                for( auto ptr : best ) {
                    std::cout << "What is the answer for the entity "
                        << ptr->name << "?\n";
                    double v;
                    std::cin >> v;
                    answers.push_back( std::make_pair(ptr, v) );
                }
                std::cout << "Thanks for helping!\n";
            }
            break;
            case UnguesserMove::RESTART_GAME:
                // should never happen
            break;
        }
    }
    return 0;
}
