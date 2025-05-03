#include "lexer.h"
#include <functional>


static bool is_nb(char c){
    return c >= '0' && c <= '9';
}

static bool is_start_identifier(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_identifier(char c){
    return is_start_identifier(c) || is_nb(c);
}

static bool is_whitespace(char c){
    return c == ' ' || c == '\t';
}

static void pass_string_while(size_t& pos, std::string_view file_content, std::function<bool(char)> do_while){
    do {
        pos++;
    } while (pos < file_content.length() && do_while(file_content[pos]));
}


static std::string get_string_while(size_t& pos, std::string_view file_content, std::function<bool(char)> do_while){
    std::string s;
    do {
        s += file_content[pos];
        pos++;
    } while (pos < file_content.length() && do_while(file_content[pos]));
    return s;
}

static char special_chars[] = { '=', '(', ')', '[', ']', ',' };

static constexpr int special_chars_len = sizeof(special_chars);

static bool is_special_char(char c){
    for (unsigned i = 0; i < special_chars_len; i++){
        if (c == special_chars[i]){
            return true;
        }
    }

    return false;
}

std::vector<Token> lex(std::string_view file_content){
    std::vector<Token> tokens;
    size_t pos = 0;

    while (pos < file_content.length()){
        if (is_whitespace(file_content[pos])){
            pass_string_while(pos, file_content, [](char c){ return is_whitespace(c); });
        } else if (file_content[pos] == '\n'){
            pos++;
        } else if (file_content[pos] == '#'){
            pos++;
            while (pos < file_content.length() && file_content[pos] != '\n'){
                pos++;
            }
            pos++;
        } else if (is_nb(file_content[pos])){
            std::string nb_str = get_string_while(pos, file_content, [](char c){ return is_nb(c); });
            Token tok_nb = Token::number(std::stol(nb_str));
            tokens.push_back(tok_nb);
        } else if (file_content[pos] == '\'' || file_content[pos] == '\"'){
            pos++; // pass "
            std::string s_str = get_string_while(pos, file_content, [](char c){ return c != '\"' && c != '\''; }); // TODO : add support for escaping
            pos++; // pass "
            Token tok_string = Token::string(s_str);
            tokens.push_back(tok_string);
        } else if (is_start_identifier(file_content[pos])){
            std::string identifier_str = get_string_while(pos, file_content, [](char c){ return is_identifier(c); });
            Token tok_identifier = Token::identifier(identifier_str);
            tokens.push_back(tok_identifier);
        } else if (is_special_char(file_content[pos])){
            TokenType tokenType = (TokenType)file_content[pos];
            tokens.push_back(Token((tokenType)));
            pos++;
        } else {
            fprintf(stderr, "ERROR, unknown char : %c (code : %d)\n", file_content[pos], file_content[pos]);
            exit(1);
        }
        
    }

    return tokens;
}

void destroy_tokens(std::vector<Token> tokens){
    for (size_t i = 0; i < tokens.size(); i++){
        tokens[i].destroy();
    }
}