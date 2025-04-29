#include "parser.h"
#include "lexer.h"
#include "build.h"


[[noreturn]] static void unexpected_token(std::vector<Token>& tokens, size_t pos){
    fprintf(stderr, "ERROR : unexpected token %d\n", tokens[pos].tokenType);
    exit(1);
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


static std::unique_ptr<Expr> parse_expr(Build& build, std::vector<Token>& tokens, size_t& pos);

static std::vector<std::unique_ptr<Expr>> parse_function_args(Build& build, std::vector<Token>& tokens, size_t& pos){
    std::vector<std::unique_ptr<Expr>> args;
    while (pos < tokens.size() && tokens[pos].tokenType != ')'){
        std::unique_ptr<Expr> expr = parse_expr(build, tokens, pos);
        args.push_back(std::move(expr));
        if (tokens[pos].tokenType == ','){
            pass_token(TOKEN_COMMA, tokens, pos);
        }
    }
    return args;
}

static std::unique_ptr<Expr> parse_expr(Build& build, std::vector<Token>& tokens, size_t& pos){
    //fprintf(stdout, "parsing expr\n");
    if (tokens[pos].tokenType == '['){
        pass_token(TOKEN_OPEN_SQUARE_BRACK, tokens, pos);
        // array
        std::vector<std::string> arr;
        while (pos < tokens.size() && tokens[pos].tokenType != ']'){
            Token string_tok = eat_token(TOKEN_STRING, tokens, pos);
            arr.push_back(*string_tok.data.str);
            if (pos < tokens.size() && tokens[pos].tokenType == ','){
                pass_token(TOKEN_COMMA, tokens, pos);
            }
        }
        pass_token(TOKEN_CLOSE_SQUARE_BRACK, tokens, pos);
        return std::make_unique<Array>(arr);
    } else if (tokens[pos].tokenType == TOKEN_STRING){
        Token string_tok = pass_token(TOKEN_STRING, tokens, pos);
        return std::make_unique<String>(*string_tok.data.str);
    } else if (tokens[pos].tokenType == TOKEN_IDENTIFIER){
        Token tok_identifier = pass_token(TOKEN_IDENTIFIER, tokens, pos);
        std::string identifier_str = *tok_identifier.data.str;
        eat_token(TOKEN_OPEN_PAREN, tokens, pos);
        std::vector<std::unique_ptr<Expr>> args = parse_function_args(build, tokens, pos);
        pass_token(TOKEN_CLOSE_PAREN, tokens, pos);
        return interpret_expr_function_call(identifier_str, std::move(args));
    } else {
        unexpected_token(tokens, pos);
    }
}

static void parse_identifier(Build& build, std::vector<Token>& tokens, size_t& pos){
    Token tok_identifier = pass_token(TOKEN_IDENTIFIER, tokens, pos);
    std::string identifier_str = *tok_identifier.data.str;
    if (tokens[pos].tokenType == '='){
        pass_token(TOKEN_EQUAL, tokens, pos);
        std::unique_ptr<Expr> expr = parse_expr(build, tokens, pos);
        if (!dynamic_cast<Array*>(expr.get())){
            fprintf(stderr, "ERROR : expected array of string after equal");
        }

        build.vars[identifier_str] = std::make_unique<Var>(identifier_str, std::move(expr));
    } else if (tokens[pos].tokenType == '('){
        fprintf(stdout, "got toplevel function call : %s\n", identifier_str.c_str());
        pass_token(TOKEN_OPEN_PAREN, tokens, pos);

        std::vector<std::unique_ptr<Expr>> args = parse_function_args(build, tokens, pos);

        pass_token(TOKEN_CLOSE_PAREN, tokens, pos);
        interpret_toplevel_function_call(build, identifier_str, std::move(args));
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