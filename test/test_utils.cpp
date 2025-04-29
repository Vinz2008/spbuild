#include <gtest/gtest.h>

#include "../src/utils.h"

class Base {
public:
    int a;
    int b;
    Base() : a(1), b(2) {}
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    char c;
    Derived() : Base(), c(0) {}
};

TEST(utils, downcast_expr){
    std::unique_ptr<Derived> derived = std::make_unique<Derived>();
    Derived* derived_ptr = derived.get();
    std::unique_ptr<Base> base = std::move(derived);
    ASSERT_EQ(derived_ptr, downcast_expr<Derived>(std::move(base)).get());
}