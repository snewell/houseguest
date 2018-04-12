#include <houseguest/ranged_value.hpp>

#include <gtest/gtest.h>

namespace
{
    using two_digit_int = houseguest::ranged_value<int, 10, 100>;

    using fifties_int = houseguest::ranged_value<int, 50, 60>;
}

TEST(RangedValue, good_min)
{
    two_digit_int tdi{10};
    ASSERT_EQ(10, tdi);
}

TEST(RangedValue, good_max)
{
    two_digit_int tdi{99};
    ASSERT_EQ(99, tdi);
}

TEST(RangedValue, min_edge)
{
    ASSERT_THROW(two_digit_int{9}, std::out_of_range);
}

TEST(RangedValue, max_edge)
{
    ASSERT_THROW(two_digit_int{100}, std::out_of_range);
}

TEST(RangedValue, good_convert)
{
    two_digit_int tdi{55};
    fifties_int fi{tdi};

    ASSERT_EQ(tdi, fi);
}

TEST(RangedValue, bad_convert)
{
    two_digit_int tdi{65};
    ASSERT_THROW(fifties_int fi{tdi}, std::out_of_range);
}

// everything after this should fail to compile due to static_asserts
#if 0
TEST(RangedValue, invalid_range)
{
    houseguest::ranged_value<int, 10, 10> invalid{10};
}
#endif

#if 0
TEST(RangedValue, invalid_conversion_max)
{
    houseguest::ranged_value<int, 1, 100> positive{52};
    houseguest::ranged_value<int, -100, 0> negative{positive};
}
#endif

#if 0
TEST(RangedValue, invalid_conversion_min)
{
    houseguest::ranged_value<int, -100, 0> negative{-52};
    houseguest::ranged_value<int, 1, 100> positive{negative};
}
#endif
