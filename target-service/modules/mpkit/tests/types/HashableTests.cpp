#if __cplusplus >= 202002L

#include "types/strong_typedef/Hash.hpp"

#include <gtest/gtest.h>

#include <QHash>
#include <QString>

namespace mpk::types::test
{

class StdHashable : public StrongTypedef<StdHashable, TypeIdentity<std::string>>,
                    public Hashable<StdHashable>
{
    using StrongTypedef::StrongTypedef;
};

TEST(HashableTest, testStdHashable)
{
    auto value = StdHashable{"Test string"};
    EXPECT_EQ(std::hash<decltype(value)>{}(value), std::hash<std::string>{}("Test string"));
}

class QtHashable : public StrongTypedef<QtHashable, TypeIdentity<QString>>,
                   public Hashable<QtHashable>
{
    using StrongTypedef::StrongTypedef;
};

TEST(HashableTest, testQtHashable)
{
    auto value = QtHashable{"Test string"};
    EXPECT_EQ(std::hash<decltype(value)>{}(value), qHash(QString{"Test string"}));
}

} // namespace mpk::types::test

#endif
