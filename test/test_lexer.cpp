#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/lexer.h"


TEST(lexer, lex){
    std::vector<Token> tokens_string = lex("\"this is a string.\"");
    ASSERT_THAT(tokens_string, testing::ElementsAre(Token::string("this is a string.")));
    
    std::vector<Token> tokens_comment = lex("// this is a comment\n");
    ASSERT_EQ(tokens_comment.size(), 0);
    std::vector<Token> tokens_comment_no_newline = lex("// this is a comment");
    ASSERT_EQ(tokens_comment_no_newline.size(), 0);
    
    std::vector<Token> tokens_function_call = lex("function_call(\"a\")");
    ASSERT_THAT(tokens_function_call, testing::ElementsAre(Token::identifier("function_call"), Token(TOKEN_OPEN_PAREN), Token::string("a"), Token(TOKEN_CLOSE_PAREN)));
}