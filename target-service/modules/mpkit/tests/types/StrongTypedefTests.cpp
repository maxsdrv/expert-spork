#include "types/StrongTypedef.h"

#include <gtest/gtest.h>

#include <QString>

namespace mpk::types::test
{

class Id : public StrongTypedef<Id, TypeIdentity<int>>,
           public ConvertibleTo<Id, double>,
           public Incrementable<Id>
{
    using StrongTypedef::StrongTypedef;
};

TEST(StrongTypedefTest, testCreatingType)
{
    auto id = Id{100};
    EXPECT_EQ(100, int(id));

    auto sameId = Id{100};
    auto anotherId = Id{101};

    EXPECT_EQ(id, sameId);
    EXPECT_NE(id, anotherId);
}

TEST(StrongTypedefTest, testDefaultConvertor)
{
    auto id = Id{100};
    EXPECT_EQ(static_cast<double>(id), 100.0);
}

TEST(StrongTypedefTest, testOptionalIdTuple)
{
    using OptionalIdTuple = std::tuple<std::optional<Id>>;
    OptionalIdTuple id;
    OptionalIdTuple sameId = id;
    EXPECT_EQ(id, sameId);
}

TEST(StrongTypedefTest, testTupleIdTuple)
{
    using TupleIdTuple = std::tuple<std::tuple<Id>>;
    TupleIdTuple id;
    TupleIdTuple sameId = id;
    EXPECT_EQ(id, sameId);
}

struct CustomConverter
{
    static QString convert(const int& value)
    {
        return QString("VALUE: %1").arg(value);
    };
};

class Convertible : public StrongTypedef<Convertible, TypeIdentity<int>>,
                    public ConvertibleTo<Convertible, QString, CustomConverter>
{
    using StrongTypedef::StrongTypedef;
};

TEST(StrongTypedefTest, testCustomConverter)
{
    auto id = Convertible{100};
    EXPECT_EQ(100, weak_cast(id));

    EXPECT_EQ(static_cast<QString>(id), QString("VALUE: 100"));
}

class WithOrdering : public StrongTypedef<WithOrdering, TypeIdentity<int>>,
                     public Ordered<WithOrdering>
{
    using StrongTypedef::StrongTypedef;
};

TEST(StrongTypedefTest, testOrdering)
{
    auto id100 = WithOrdering{100};
    auto id200 = WithOrdering{200};
    EXPECT_GT(id200, id100);
    EXPECT_LT(id100, id200);
    EXPECT_GE(id100, id100);
    EXPECT_LE(id100, id100);
}

class WithAdditionAndMultiplication
  : public StrongTypedef<WithAdditionAndMultiplication, TypeIdentity<int>>,
    public Additive<WithAdditionAndMultiplication>,
    public Scalable<WithAdditionAndMultiplication>
{
    using StrongTypedef::StrongTypedef;
};

TEST(StrongTypedefTest, testAddition)
{
    auto value10 = WithAdditionAndMultiplication{10};
    auto value20 = WithAdditionAndMultiplication{20};

    EXPECT_EQ(WithAdditionAndMultiplication{30}, value10 + value20);
    EXPECT_EQ(WithAdditionAndMultiplication{-10}, value10 - value20);

    value10 += value20;
    EXPECT_EQ(value10, WithAdditionAndMultiplication{30});

    value10 -= value20;
    EXPECT_EQ(value10, WithAdditionAndMultiplication{10});
}

TEST(StrongTypedefTest, testMultiplication)
{
    auto value20 = WithAdditionAndMultiplication{20};
    auto value10 = WithAdditionAndMultiplication{10};

    EXPECT_EQ(WithAdditionAndMultiplication{200}, value10 * 20);
    EXPECT_EQ(WithAdditionAndMultiplication{2}, value20 / 10);

    value10 *= 20;
    EXPECT_EQ(value10, WithAdditionAndMultiplication{200});

    value10 /= 20;
    EXPECT_EQ(value10, WithAdditionAndMultiplication{10});

    EXPECT_EQ(value10 * 5, WithAdditionAndMultiplication{50});
    EXPECT_EQ(value10 / 2, WithAdditionAndMultiplication{5});

    EXPECT_EQ(2, value20 / value10);
}

class WithAdditionDifferenceType
  : public StrongTypedef<WithAdditionDifferenceType, TypeIdentity<int>>,
    public Additive<WithAdditionDifferenceType>,
    public Scalable<WithAdditionDifferenceType>
{
    using StrongTypedef::StrongTypedef;
};

class WithAdditionMainType : public StrongTypedef<WithAdditionMainType, TypeIdentity<float>>,
                             public Additive<WithAdditionMainType, WithAdditionDifferenceType>
{
    using StrongTypedef::StrongTypedef;
};

TEST(StrongTypedefTest, testAdditionDifferenceType)
{
    auto value10 = WithAdditionMainType{10.};
    auto value20 = WithAdditionDifferenceType{20};
    auto value30 = WithAdditionMainType{30.};

    EXPECT_EQ(WithAdditionMainType{30.}, value10 + value20);
    EXPECT_EQ(WithAdditionMainType{10.}, value30 - value20);
    EXPECT_EQ(WithAdditionDifferenceType{-20}, value10 - value30);
}

class Implicit : public StrongTypedef<Id, TypeIdentity<int, true>>
{
    using StrongTypedef::StrongTypedef;
};

TEST(StrongTypedefTest, testImplicitConstruction)
{
    // NOTE: first arg is Id that defined excplicit, so we are to put type name
    // here. It adds more type safety, but may be less convenient
    std::pair<Id, Implicit> values = {Id{100}, 200};
    EXPECT_EQ(Id{100}, values.first);
    EXPECT_EQ(Implicit{200}, values.second);
}

TEST(StrongTypedefTest, testIncrement)
{
    Id id1{100};
    EXPECT_EQ(Id{100}, id1);

    Id id2 = ++id1;
    EXPECT_EQ(Id{101}, id1);
    EXPECT_EQ(Id{101}, id2);

    Id id3 = id1++;
    EXPECT_EQ(Id{102}, id1);
    EXPECT_EQ(Id{101}, id3);

    Id id4 = --id1;
    EXPECT_EQ(Id{101}, id1);
    EXPECT_EQ(Id{101}, id4);

    Id id5 = id1--;
    EXPECT_EQ(Id{100}, id1);
    EXPECT_EQ(Id{101}, id5);
}

} // namespace mpk::types::test
