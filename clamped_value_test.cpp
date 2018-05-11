#include <houseguest/bounded_value.hpp>

#include <gtest/gtest.h>

namespace
{
    using two_digit_int = houseguest::clamped_value<int, 10, 99>;
} // namespace

TEST(ClampedValue, below_min)
{
    two_digit_int tdi{9};

    ASSERT_EQ(10, tdi);
}

TEST(ClampedValue, above_max)
{
    two_digit_int tdi{100};

    ASSERT_EQ(99, tdi);
}

TEST(ClampedValue, in_range)
{
    two_digit_int tdi{27};

    ASSERT_EQ(27, tdi);
}
