#include <houseguest/ranged_value.hpp>

#include <gtest/gtest.h>

namespace
{
    using two_digit_int = houseguest::ranged_value<int, 10, 99>;

    using fifties_int = houseguest::ranged_value<int, 50, 59>;
} // namespace

TEST(RangedValue, good_min) // NOLINT
{
    two_digit_int tdi{10};
    ASSERT_EQ(10, tdi);
}

TEST(RangedValue, good_max) // NOLINT
{
    two_digit_int tdi{99};
    ASSERT_EQ(99, tdi);
}

TEST(RangedValue, min_edge) // NOLINT
{
    try
    {
        two_digit_int{9};
        FAIL();
    }
    catch(std::system_error const & se)
    {
        ASSERT_EQ(static_cast<int>(houseguest::ranged_value_error::below_min),
                  se.code().value());
    }
}

TEST(RangedValue, max_edge) // NOLINT
{
    try
    {
        two_digit_int{100};
        FAIL();
    }
    catch(std::system_error const & se)
    {
        ASSERT_EQ(static_cast<int>(houseguest::ranged_value_error::above_max),
                  se.code().value());
    }
}

TEST(RangedValue, good_convert) // NOLINT
{
    two_digit_int tdi{55};
    fifties_int fi{tdi};

    ASSERT_EQ(tdi, fi);
}

TEST(RangedValue, bad_convert) // NOLINT
{
    try
    {
        two_digit_int tdi{65};
        fifties_int fi{tdi};
        FAIL();
    }
    catch(std::system_error const & se)
    {
        ASSERT_EQ(static_cast<int>(houseguest::ranged_value_error::above_max),
                  se.code().value());
    }
}

TEST(RangedValue, equal_min_max) // NOLINT
{
    houseguest::ranged_value<int, 10, 10> boring{10};
}
