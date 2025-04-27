#include "parser.h"
#include "lexer.h"
#include "build.h"


[[noreturn]] static void unexpected_token(std::vector<Token>& tokens, size_t pos){
    fprintf(stderr, "ERROR : unexpected token %d\n", tokens[pos].tokenType);
    exit(1);
}

template <class T>
std::unique_ptr<T> downcast_expr(std::unique_ptr<Expr> E){
    std::unique_ptr<T> SubClass;
    T* SubClassPtr = dynamic_cast<T*>(E.get());
    if (!SubClassPtr){
        return nullptr;
    }
    E.release();
    SubClass.reset(SubClassPtr);
    return SubClass;
}

static Token eat_token(TokenType tokenType, std::vector<Token>& tokens, size_t& pos){
    if (tokens[pos].tokenType != tokenType){
        unexpected_token(tokens, pos);
    }
    Token tok = tokens[pos];
    pos++;
    return tok;
}

static Token pass_token(TokenType tokenType, std::vector<Token>& tokens, size_t& pos){
    (void)tokenType; // just for code clarity
    Token tok = tokens[pos];
    pos++;
    return tok;
}

static std::unique_ptr<Expr> parse_expr(Build& build, std::vector<Token>& tokens, size_t& pos){
    if (tokens[pos].tokenType == '['){
        pass_token(TOKEN_OPEN_SQUARE_BRACK, tokens, pos);
        // array
        std::vector<std::string> arr;
        while (pos < tokens.size() && tokens[pos].tokenType != ']'){
            Token string_tok = eat_token(TOKEN_STRING, tokens, pos);
            arr.push_back(*string_tok.data.str);
            pos++;
            if (pos < tokens.size() && tokens[pos].tokenType == ','){
                pass_token(TOKEN_COMMA, tokens, pos);
            }
        }
        pass_token(TOKEN_CLOSE_SQUARE_BRACK, tokens, pos);
        return std::make_unique<Array>(arr);
    } else {
        unexpected_token(tokens, pos);
    }
}

static void parse_identifier(Build& build, std::vector<Token>& tokens, size_t& pos){
    Token tok_identifier = pass_token(TOKEN_IDENTIFIER, tokens, pos);
    if (tokens[pos].tokenType == '='){
        pass_token(TOKEN_EQUAL, tokens, pos);
        std::unique_ptr<Expr> expr = parse_expr(build, tokens, pos);
        if (!dynamic_cast<Array*>(expr.get())){
            fprintf(stderr, "ERROR : expected array of string after equal");
        }

        build.vars[*tok_identifier.data.str] = std::make_unique<Var>(*tok_identifier.data.str, std::move(expr));
    } else if (tokens[pos].tokenType == '('){
        pass_token(TOKEN_OPEN_PAREN, tokens, pos);

        while (pos < tokens.size() && tokens[pos].tokenType != ')'){
            // TODO
            if (tokens[pos].tokenType == ','){
                pass_token(TOKEN_COMMA, tokens, pos);
            }
            pos++;
        }

        pass_token(TOKEN_CLOSE_PAREN, tokens, pos);
    } else {
        unexpected_token(tokens, pos);
    }
}

Build parse(std::vector<Token> tokens){
    Build build;
    size_t pos = 0;
    while (pos < tokens.size()){
        if (tokens[pos].tokenType == TOKEN_IDENTIFIER){
            parse_identifier(build, tokens, pos);
        } else {
            unexpected_token(tokens, pos);
        }
    }
    return build;
}