#include <houseguest/bounded_value.hpp>

#include <gtest/gtest.h>

namespace
{
    using two_digit_int = houseguest::bounded_value<int, 10, 99>;

    using fifties_int = houseguest::bounded_value<int, 50, 59>;

    using tens = houseguest::bounded_value<int, 10, 19>;

    using teens = houseguest::bounded_value<int, 13, 19>;
} // namespace

TEST(BoundedValue, good_min) // NOLINT
{
    two_digit_int tdi{10};
    ASSERT_EQ(10, tdi);
}

TEST(BoundedValue, good_max) // NOLINT
{
    two_digit_int tdi{99};
    ASSERT_EQ(99, tdi);
}

TEST(BoundedValue, min_edge) // NOLINT
{
    try
    {
        two_digit_int{9};
        FAIL();
    }
    catch(std::system_error const & se)
    {
        ASSERT_EQ(static_cast<int>(houseguest::bounded_value_error::below_min),
                  se.code().value());
    }
}

TEST(BoundedValue, max_edge) // NOLINT
{
    try
    {
        two_digit_int{100};
        FAIL();
    }
    catch(std::system_error const & se)
    {
        ASSERT_EQ(static_cast<int>(houseguest::bounded_value_error::above_max),
                  se.code().value());
    }
}

TEST(BoundedValue, good_convert) // NOLINT
{
    two_digit_int tdi{55};
    fifties_int fi{tdi};

    ASSERT_EQ(tdi, fi);
}

TEST(BoundedValue, bad_convert) // NOLINT
{
    try
    {
        two_digit_int tdi{65};
        fifties_int fi{tdi};
        FAIL();
    }
    catch(std::system_error const & se)
    {
        ASSERT_EQ(static_cast<int>(houseguest::bounded_value_error::above_max),
                  se.code().value());
    }
}

TEST(BoundedValue, equal_min_max) // NOLINT
{
    houseguest::bounded_value<int, 10, 10> boring{10};
}

TEST(BoundedValue, less_than)
{
    two_digit_int a{10};
    two_digit_int b{20};

    ASSERT_LT(a, b);
}

TEST(BoundedValue, greater_than) // NOLINT
{
    two_digit_int a{10};
    two_digit_int b{20};

    ASSERT_GT(b, a);
}

TEST(BoundedValue, less_than_equal) // NOLINT
{
    two_digit_int a{10};
    two_digit_int b{20};
    two_digit_int c{20};

    ASSERT_LE(a, b);
    ASSERT_LE(b, c);
}

TEST(BoundedValue, greater_than_equal) // NOLINT
{
    two_digit_int a{10};
    two_digit_int b{20};
    two_digit_int c{20};

    ASSERT_GE(b, a);
    ASSERT_GE(b, c);
}

TEST(BoundedValue, equal) // NOLINT
{
    two_digit_int a{20};
    two_digit_int b{20};

    ASSERT_EQ(a, b);
}

TEST(BoundedValue, not_equal) // NOLINT
{
    two_digit_int a{10};
    two_digit_int b{20};

    ASSERT_NE(a, b);
}

TEST(BoundedValue, lower_boundary_convert) // NOLINT
{
    tens driving_age{16};
    teens voting_age{driving_age};
}

TEST(BoundedValue, upper_boundary_convert) // NOLINT
{
    teens voting_age{18};
    tens driving_age{voting_age};
}
