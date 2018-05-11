#ifndef HOUSEGUEST_BOUNDED_VALUE_HPP
#define HOUSEGUEST_BOUNDED_VALUE_HPP 1

#include <system_error>
#include <type_traits>

namespace houseguest
{
    enum class bounded_value_error
    {
        success = 0,
        below_min,
        above_max
    };

    struct bounded_value_error_category : std::error_category
    {
        const char * name() const noexcept override
        {
            return "houseguest::bounded_value";
        }

        std::string message(int ev) const override
        {
            switch(static_cast<bounded_value_error>(ev))
            {
            case bounded_value_error::success:
                return "success";
                break;

            case bounded_value_error::below_min:
                return "value is below the minimum allowed";
                break;

            case bounded_value_error::above_max:
                return "value is above the maximum allowed";
                break;

            default:
                return "(unknown error)";
                break;
            }
        }
    };

    std::error_code make_error_code(bounded_value_error error)
    {
        static bounded_value_error_category const category_instance{};
        return std::error_code{static_cast<int>(error), category_instance};
    }

    template <typename T, T MIN, T MAX>
    struct bounded_value
    {
        static_assert(MIN <= MAX, "Out of range");
        static_assert(std::is_integral<T>::value, "T must be integral");

        using underlying_type = T;

        explicit constexpr bounded_value(T value)
          : _value{validate(value)}
        {
        }

        template <T OTHER_MIN, T OTHER_MAX>
        explicit constexpr bounded_value(
            bounded_value<T, OTHER_MIN, OTHER_MAX> const & other)
          : _value{validate(static_cast<T>(other))}
        {
            static_assert(MIN <= OTHER_MAX, "Out of range");
            static_assert(MAX >= OTHER_MIN, "Out of range");
        }

        constexpr operator T() const noexcept
        {
            return _value;
        }

        friend constexpr bool operator==(bounded_value<T, MIN, MAX> const & lhs,
                                         bounded_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value == rhs._value;
        }

        friend constexpr bool operator!=(bounded_value<T, MIN, MAX> const & lhs,
                                         bounded_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value != rhs._value;
        }

        friend constexpr bool operator<(bounded_value<T, MIN, MAX> const & lhs,
                                        bounded_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value < rhs._value;
        }

        friend constexpr bool operator<=(bounded_value<T, MIN, MAX> const & lhs,
                                         bounded_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value <= rhs._value;
        }

        friend constexpr bool operator>(bounded_value<T, MIN, MAX> const & lhs,
                                        bounded_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value > rhs._value;
        }

        friend constexpr bool operator>=(bounded_value<T, MIN, MAX> const & lhs,
                                         bounded_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value >= rhs._value;
        }

    private:
        T _value;

        T validate(T value)
        {
            validate_min(value);
            validate_max(value);
            return value;
        }

        void validate_min(T value)
        {
            if(value < MIN)
            {
                throw std::system_error{
                    make_error_code(bounded_value_error::below_min)};
            }
        }

        void validate_max(T value)
        {
            if(value > MAX)
            {
                throw std::system_error{
                    make_error_code(bounded_value_error::above_max)};
            }
        }
    };
} // namespace houseguest

namespace std
{
    template <>
    struct is_error_code_enum<houseguest::bounded_value_error> : true_type
    {
    };
} // namespace std

#endif
