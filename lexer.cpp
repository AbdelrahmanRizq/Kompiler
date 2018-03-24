#include "lexer.h"
#include "machine.h"
#include "rexparser.h"
#include "dfa.h"
#include <sstream>
#include <iostream>

lexer::lexer(machine &mac) : ttab(mac) {
    
}

// void lexer::lex_analyze() {
//     ttab = transition_table(mac);
// }


lexer::token lexer::next_token(std::ifstream &ifs) {
    std::ostringstream accum_ss;
    std::ostringstream token_ss;
    std::string token_class = "error";
    
    if (!ifs.is_open() || ifs.eof())
        return lexer::token("", "eof");

    char c = ifs.peek();
    if (c == EOF)
        return lexer::token("", "eof");
    
    while (isspace((char) ifs.peek()))
        ifs.get(c);

    // Recognize when starting state is also a final state
    if (ttab.is_accepting())
        token_class = ttab.get_token_class();
        
    while (ifs.peek()) {
        c = ifs.peek();
        sid_t st = ttab.move(c);

        if (st == -1)    // reached a deadend state
            break;
        
        ifs.get(c);
        accum_ss << c;

        if (ttab.is_accepting()) {
            // append accum_ss to token_ss, clear accum_ss afterwards
            token_ss << accum_ss.str() ;
            accum_ss.str(""); accum_ss.clear();

            token_class = ttab.get_token_class();
        }
    }
    
    ttab.reset();
    return lexer::token(token_class.empty() ? accum_ss.str() : token_ss.str(),
            token_class);
}