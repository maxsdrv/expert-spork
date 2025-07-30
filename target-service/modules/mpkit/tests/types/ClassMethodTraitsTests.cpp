#include "types/ClassMethodTraits.hpp"

#include <gtest/gtest.h>

#include <string>

namespace mpk::types::tests
{

class Foo
{
public:
    int foo(std::string) const;
    void bar(int, std::string);
    std::string baz() const noexcept;
    static int boo(std::string);
};

TEST(ClassMethodTraitsTest, testConstMethod)
{
    using Traits = ClassMethodTraits<decltype(&Foo::foo)>;

    static_assert(std::is_same_v<int, Traits::ReturnType>);
    static_assert(std::is_same_v<Foo, Traits::Class>);
    static_assert(Traits::ArgCount == 1);
    static_assert(std::is_same_v<std::string, Traits::NthArg<0>>);
}

TEST(ClassMethodTraitsTest, testNonConstMethod)
{
    using Traits = ClassMethodTraits<decltype(&Foo::bar)>;

    static_assert(std::is_same_v<void, Traits::ReturnType>);
    static_assert(std::is_same_v<Foo, Traits::Class>);
    static_assert(Traits::ArgCount == 2);
    static_assert(std::is_same_v<int, Traits::NthArg<0>>);
    static_assert(std::is_same_v<std::string, Traits::NthArg<1>>);
}

TEST(ClassMethodTraitsTest, testNoexceptMethod)
{
    using Traits = ClassMethodTraits<decltype(&Foo::baz)>;

    static_assert(std::is_same_v<std::string, Traits::ReturnType>);
    static_assert(std::is_same_v<Foo, Traits::Class>);
    static_assert(Traits::ArgCount == 0);
}

} // namespace mpk::types::tests
