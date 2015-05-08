namespace command_line {
    const char help_message[] =
" <database>\n"
"Runs one round of the Unguesser using the given database.\n"
"\n"
"The unguesser is a program that tries to determine what are you thinking.\n"
"Think of an entity. The Unguesser will make several questions\n"
"and try to guess which entity you tought.\n"
"The Unguesser relies on a database with questions and entities.\n"
"Differents databases have different questions; for instance,\n"
"one database could be about people and other could be about courses.\n"
"\n"
"Options:"
"--seed <n>\n"
"    Choose the seed of the random number generator.\n"
"\n"
"--no-write\n"
"    Each time the program is ran it tries to adjust the database\n"
"    to the answers given by the user. This option disables this update.\n"
"\n"
"--help\n"
"    Show this help and quit.\n"
"\n"
"--debug\n"
"    Show some debug information\n"
;
} // namespace command_line
#include <iostream>
#include <fstream>
#include "cmdline/args.hpp"
#include "unguesser.h"

namespace command_line {
    std::string file_name;
    bool debug = false;

    long long unsigned seed;
    bool seed_set = false;

    bool no_write = false;

    void parse( cmdline::args && args ) {
        while( args.size() > 0 ) {
            std::string arg = args.next();
            if( arg == "--seed" ) {
                args >> seed;
                seed_set = true;
                continue;
            }
            if( arg == "--no-write" ) {
                no_write = true;
                continue;
            }
            if( arg == "--help" ) {
                std::cout << args.program_name() << help_message;
                std::exit(0);
            }
            if( arg == "--debug" ) {
                debug = true;
                continue;
            }
            if( file_name == "" ) {
                file_name = arg;
                continue;
            }
            std::cerr << "Error: Two databases provided: " << file_name
                << " and " << arg << '\n';
            std::exit(1);
        }
        if( file_name == "" ) {
            std::cerr << "Error: No database provided.\n";
            std::exit(1);
        }
    }
} // namespace command_line

int main( int argc, char ** argv ) {
    command_line::parse( cmdline::args(argc, argv) );

    auto ptr = std::make_unique<std::fstream>(command_line::file_name);
    Unguesser unguesser(std::move(ptr));

    if( command_line::no_write )
        unguesser.disable_write();

    if( command_line::seed_set )
        unguesser.seed( command_line::seed );

    if( command_line::debug )
        std::cout << "Seed: " << unguesser.seed() << '\n';

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
        std::cin.ignore();
        if( index == 0 )
            unguesser.inform_answer( name );
        else
            unguesser.inform_answer( names[index-1] );
    };

    auto print_entities = [&](){
        std::cout << "Remaining questions: "
            << unguesser.remaining_questions() << '\n'
            << "Threshold: " << unguesser.similarity_threshold()
            << "\n[score] entity\n";
        for( auto entity_ptr : unguesser.entities() )
            std::cout << "[" << entity_ptr->similarity << "] "
                << entity_ptr->name << std::endl;
    };


    UnguesserMove move = UnguesserMove::ASK_QUESTION;
    while( (move = unguesser.next_move() ) != UnguesserMove::RESTART_GAME ) {
        switch( move ) {
            case UnguesserMove::ASK_QUESTION: {
                const Question * q = unguesser.next_question();

                if( command_line::debug )
                    print_entities();

                std::cout << q->text << std::endl;
                double ans;
                std::cin >> ans;
                std::cin.ignore();
                unguesser.add_answer( q, ans );
            }
            break;
            case UnguesserMove::GUESS_ANSWER: {
                const Entity * e = unguesser.guess();
                std::cout << "I think the answer is:\n"
                    << e->name << "\nIs this right? [y/n] ";
                std::string ans;
                std::cin >> ans;
                std::cin.ignore();
                if( ans == "y" || ans == "Y" ) {
                    std::cout << "Good! Thanks for playing!\n";
                    unguesser.inform_answer( e );
                    move = UnguesserMove::RESTART_GAME; // exits the loop
                    break;
                }
                std::cout << "No? Please, type the entity name,"
                    << " so I can adjust my data.\n"
                    << "Or leave it empty, if you want me to try again.\n";
                std::getline( std::cin, ans );
                if( ans != "" )
                    ask_entity( ans );
            }
            break;
            case UnguesserMove::GIVE_UP: {
                std::string ans;
                std::cout << "I give up. On what are you thinking?\n";
                std::getline( std::cin, ans );
                ask_entity( ans );
                std::cout << "Thanks for answering!\n";
            }
            break;
            case UnguesserMove::ASK_NEW_QUESTION: {
                std::cout << "Consider these in my database:\n";
                auto best = unguesser.best_guesses();
                for( auto ptr : best )
                    std::cout << ptr->name << '\n';
                std::cout << "Please, devise a question to help me"
                    << " differentiate between them.\n"
                    << "Or leave it empty, if you do not want to provide a new question.\n";
                std::string ans;
                std::getline( std::cin, ans );

                /* Any 'break' statement I'd put here would break the switch,
                 * not the loop.
                 * The cleanest way I can think of is to use goto. (sorry, Melgarejo)
                 */
                if( ans == "" )
                    goto exit;

                std::vector< std::pair<const Entity *, double> > answers;
                for( auto ptr : best ) {
                    std::cout << "What is the answer for the entity "
                        << ptr->name << "?\n";
                    double v;
                    std::cin >> v;
                    std::cin.ignore();
                    answers.push_back( std::make_pair(ptr, v) );
                }
                unguesser.inform_new_question( ans, answers );
                std::cout << "Thanks for helping!\n";
            }
            break;
            case UnguesserMove::RESTART_GAME:
                // should never happen
            break;
        }
    }

exit:
    if( command_line::debug )
        print_entities();

    return 0;
}
