#ifndef HOUSEGUEST_RANGED_VALUE_HPP
#define HOUSEGUEST_RANGED_VALUE_HPP 1

#include <system_error>
#include <type_traits>

namespace houseguest
{
    enum class ranged_value_error
    {
        success = 0,
        below_min,
        above_max
    };

    struct ranged_value_error_category : std::error_category
    {
        const char * name() const noexcept override
        {
            return "houseguest::ranged_value";
        }

        std::string message(int ev) const override
        {
            switch(static_cast<ranged_value_error>(ev))
            {
            case ranged_value_error::success:
                return "success";
                break;

            case ranged_value_error::below_min:
                return "value is below the minimum allowed";
                break;

            case ranged_value_error::above_max:
                return "value is above the maximum allowed";
                break;

            default:
                return "(unknown error)";
                break;
            }
        }
    };

    std::error_code make_error_code(ranged_value_error error)
    {
        static ranged_value_error_category const category_instance{};
        return std::error_code{static_cast<int>(error), category_instance};
    }

    template <typename T, T MIN, T MAX>
    struct ranged_value
    {
        static_assert(MIN <= MAX, "Out of range");
        static_assert(std::is_integral<T>::value, "T must be integral");

        using underlying_type = T;

        explicit constexpr ranged_value(T value)
          : _value{value}
        {
            validate_min(_value);
            validate_max(_value);
        }

        template <T OTHER_MIN, T OTHER_MAX>
        explicit constexpr ranged_value(
            ranged_value<T, OTHER_MIN, OTHER_MAX> const & other)
          : _value{static_cast<T>(other)}
        {
            static_assert(MIN < OTHER_MAX, "Out of range");
            static_assert(MAX > OTHER_MIN, "Out of range");

            validate_min(_value);
            validate_max(_value);
        }

        constexpr operator T() const noexcept
        {
            return _value;
        }

        friend constexpr bool operator==(ranged_value<T, MIN, MAX> const & lhs,
                                         ranged_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value == rhs._value;
        }

        friend constexpr bool operator!=(ranged_value<T, MIN, MAX> const & lhs,
                                         ranged_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value != rhs._value;
        }

        friend constexpr bool operator<(ranged_value<T, MIN, MAX> const & lhs,
                                        ranged_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value < rhs._value;
        }

        friend constexpr bool operator<=(ranged_value<T, MIN, MAX> const & lhs,
                                         ranged_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value <= rhs._value;
        }

        friend constexpr bool operator>(ranged_value<T, MIN, MAX> const & lhs,
                                        ranged_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value > rhs._value;
        }

        friend constexpr bool operator>=(ranged_value<T, MIN, MAX> const & lhs,
                                         ranged_value<T, MIN, MAX> const & rhs)
        {
            return lhs._value >= rhs._value;
        }

    private:
        T _value;

        void validate_min(T value)
        {
            if(value < MIN)
            {
                throw std::system_error{
                    make_error_code(ranged_value_error::below_min)};
            }
        }

        void validate_max(T value)
        {
            if(value > MAX)
            {
                throw std::system_error{
                    make_error_code(ranged_value_error::above_max)};
            }
        }
    };
} // namespace houseguest

namespace std
{
    template <>
    struct is_error_code_enum<houseguest::ranged_value_error> : true_type
    {
    };
} // namespace std

#endif
