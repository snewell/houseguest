#ifndef HOUSEGUEST_RANGED_VALUE_HPP
#define HOUSEGUEST_RANGED_VALUE_HPP 1

#include <stdexcept>
#include <type_traits>

namespace houseguest
{
    template <typename T, T MIN, T MAX>
    class ranged_value
    {
        static_assert(MIN < MAX, "Out of range");
        static_assert(std::is_integral<T>::value, "T must be integral");

    public:
        using underlying_type = T;

        explicit ranged_value(T value)
          : _value{value}
        {
            validate_min(_value);
            validate_max(_value);
        }

        template <T OTHER_MIN, T OTHER_MAX>
        explicit ranged_value(ranged_value<T, OTHER_MIN, OTHER_MAX> const &other)
          : _value{static_cast<T>(other)}
        {
            static_assert(MIN < OTHER_MAX, "Out of range");
            static_assert(MAX > OTHER_MIN, "Out of range");

            validate_min(_value);
            validate_max(_value);
        }

        constexpr operator T () const noexcept
        {
            return _value;
        }

        friend constexpr bool operator == (ranged_value<T, MIN, MAX> const &lhs,
                                           ranged_value<T, MIN, MAX> const &rhs)
        {
            return lhs._value == rhs._value;
        }

        friend constexpr bool operator != (ranged_value<T, MIN, MAX> const &lhs,
                                           ranged_value<T, MIN, MAX> const &rhs)
        {
            return lhs._value != rhs._value;
        }

        friend constexpr bool operator < (ranged_value<T, MIN, MAX> const &lhs,
                                          ranged_value<T, MIN, MAX> const &rhs)
        {
            return lhs._value < rhs._value;
        }

        friend constexpr bool operator <= (ranged_value<T, MIN, MAX> const &lhs,
                                           ranged_value<T, MIN, MAX> const &rhs)
        {
            return lhs._value <= rhs._value;
        }

        friend constexpr bool operator > (ranged_value<T, MIN, MAX> const &lhs,
                                          ranged_value<T, MIN, MAX> const &rhs)
        {
            return lhs._value > rhs._value;
        }

        friend constexpr bool operator >= (ranged_value<T, MIN, MAX> const &lhs,
                                           ranged_value<T, MIN, MAX> const &rhs)
        {
            return lhs._value >= rhs._value;
        }

    private:
        T _value;

        void validate_min(T value)
        {
            if(value < MIN)
            {
                throw std::out_of_range{"too small"};
            }
        }

        void validate_max(T value)
        {
            if(value >= MAX)
            {
                throw std::out_of_range{"too large"};
            }
        }
    };
}

#endif
