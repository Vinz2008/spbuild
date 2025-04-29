#ifndef _LEXER_HEADER_
#define _LEXER_HEADER_

#include <vector>
#include <string>

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

    friend bool operator==(const Token& lhs, const Token& rhs){
        if (lhs.tokenType != rhs.tokenType){
            return false;
        }
        switch (lhs.tokenType){
            case TOKEN_IDENTIFIER:
            case TOKEN_STRING:
                return *lhs.data.str == *rhs.data.str;
            case TOKEN_NB:
                return lhs.data.nb == rhs.data.nb;
            default:
                return true;
        }
    }


    // constructors
    static Token number(long nb){
        Token tok = Token(TOKEN_NB);
        tok.data.nb = nb;
        return tok;
    }

private:
    static Token token_with_string_data(TokenType tokenType, std::string s){
        Token tok = Token(tokenType);
        tok.data.str = new std::string(s);
        return tok;
    }

public:

    static Token identifier(std::string i){
        return token_with_string_data(TOKEN_IDENTIFIER, i);
    }

    static Token string(std::string s){
        return token_with_string_data(TOKEN_STRING, s);
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

#endif