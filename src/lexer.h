#include <vector>
#include <string>
#include <variant>

#include <iostream>

enum TokenType {
    TOKEN_NB,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_OPEN_PAREN = '(',
    TOKEN_CLOSE_PAREN = ')',
    TOKEN_EQUAL = '=',
    TOKEN_OPEN_SQUARE_BRACK = '[',
    TOKEN_CLOSE_SQUARE_BRACK = ']',
    TOKEN_COMMA = ',',
};



// TODO : string interning ?

class Token {
public:
    TokenType tokenType;


    union DataUnion {
        std::string* str;
        long nb;
    } data;
    
protected:

    Token(TokenType tokenType, union DataUnion data) : tokenType(tokenType), data(data) {}
public:
    Token(TokenType tokenType) : tokenType(tokenType) {}


    // constructors
    static Token number(long nb){
        Token tok = Token(TOKEN_NB);
        tok.data.nb = nb;
        return tok;
    }

    static Token identifier(std::string i){
        Token tok = Token(TOKEN_IDENTIFIER);
        tok.data.str = new std::string(i);
        return tok;
    }

    static Token string(std::string s){
        Token tok = Token(TOKEN_STRING);
        tok.data.str = new std::string(s);
        return tok;
    }

    // if auto destructor, problem with vecs
    void destroy(){
        switch (tokenType){
            case TOKEN_IDENTIFIER:
            case TOKEN_STRING:
                delete this->data.str;
                break;
            default:
                break;
        }
    }
};

std::vector<Token> lex(std::string_view file_content);

void destroy_tokens(std::vector<Token> tokens);