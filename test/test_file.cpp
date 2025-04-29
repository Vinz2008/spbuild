#include <gtest/gtest.h>
#include "../src/file.h"


TEST(file, get_extension){
    ASSERT_EQ(get_extension("filename.c"), ".c");
    ASSERT_EQ(get_extension("filename.S"), ".S");
    ASSERT_EQ(get_extension("filename.rs"), ".rs");
    ASSERT_EQ(get_extension(".gitignore"), "");
}