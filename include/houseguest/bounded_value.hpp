#ifndef HOUSEGUEST_BOUNDED_VALUE_HPP
#define HOUSEGUEST_BOUNDED_VALUE_HPP 1

#if __cplusplus >= 201703L
#include <algorithm>
#endif

#include <system_error>
#include <tuple>
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

    inline std::error_code make_error_code(bounded_value_error error)
    {
        static bounded_value_error_category const category_instance{};
        return std::error_code{static_cast<int>(error), category_instance};
    }

    template <typename T, T MIN, T MAX>
    struct exception_validator
    {
        static_assert(MIN <= MAX, "Out of range");
        static_assert(std::is_integral<T>::value, "T must be integral");

        constexpr T operator()(T value) const
        {
            validate_min(value);
            validate_max(value);
            return value;
        }

    private:
        constexpr void validate_min(T value) const
        {
            if(value < MIN)
            {
                throw std::system_error{
                    make_error_code(bounded_value_error::below_min)};
            }
        }

        constexpr void validate_max(T value) const
        {
            if(value > MAX)
            {
                throw std::system_error{
                    make_error_code(bounded_value_error::above_max)};
            }
        }
    };

    namespace internal
    {
        template <typename T, typename VALIDATOR>
        struct bounded_value_storage
        {
            bounded_value_storage(T value, VALIDATOR validator)
              : _data{std::make_tuple(std::move(value), std::move(validator))}
            {
            }

            constexpr T & value() noexcept
            {
                return std::get<0>(_data);
            }

            constexpr T value() const noexcept
            {
                return std::get<0>(_data);
            }

            constexpr VALIDATOR & validator() noexcept
            {
                return std::get<1>(_data);
            }

        private:
            // Using a tuple means the compiler should optimize away any
            // storage for empty VALIDATOR types.  Compiler Explorer seems to
            // verify this.
            using storage = std::tuple<T, VALIDATOR>;

            // make sure this optimization is turned on
            static_assert(!std::is_empty<VALIDATOR>::value ||
                              (sizeof(storage) == sizeof(T)),
                          "Non-empty validator is impacting size");

            storage _data;
        };
    } // namespace internal

    template <typename T, T MIN, T MAX,
              typename VALIDATOR = exception_validator<T, MIN, MAX>>
    struct bounded_value
    {
        static_assert(MIN <= MAX, "Out of range");
        static_assert(std::is_integral<T>::value, "T must be integral");

        using underlying_type = T;

        explicit constexpr bounded_value(T value,
                                         VALIDATOR validator = VALIDATOR{})
          : _data{value, std::move(validator)}
        {
            _data.value() = _data.validator()(_data.value());
        }

        template <T OTHER_MIN, T OTHER_MAX, typename OTHER_VALIDATOR>
        explicit constexpr bounded_value(
            bounded_value<T, OTHER_MIN, OTHER_MAX, OTHER_VALIDATOR> const &
                other,
            VALIDATOR validator =
                VALIDATOR{}) noexcept(noexcept(validator(static_cast<T>(other))))
          : _data{static_cast<T>(other), std::move(validator)}
        {
            static_assert(MIN <= OTHER_MAX, "Out of range");
            static_assert(MAX >= OTHER_MIN, "Out of range");
            _data.value() = _data.validator()(_data.value());
        }

        constexpr operator T() const noexcept
        {
            return _data.value();
        }

        friend constexpr bool
        operator==(bounded_value<T, MIN, MAX, VALIDATOR> const & lhs,
                   bounded_value<T, MIN, MAX, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() == rhs._data.value();
        }

        friend constexpr bool
        operator!=(bounded_value<T, MIN, MAX, VALIDATOR> const & lhs,
                   bounded_value<T, MIN, MAX, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() != rhs._data.value();
        }

        friend constexpr bool
        operator<(bounded_value<T, MIN, MAX, VALIDATOR> const & lhs,
                  bounded_value<T, MIN, MAX, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() < rhs._data.value();
        }

        friend constexpr bool
        operator<=(bounded_value<T, MIN, MAX, VALIDATOR> const & lhs,
                   bounded_value<T, MIN, MAX, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() <= rhs._data.value();
        }

        friend constexpr bool
        operator>(bounded_value<T, MIN, MAX, VALIDATOR> const & lhs,
                  bounded_value<T, MIN, MAX, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() > rhs._data.value();
        }

        friend constexpr bool
        operator>=(bounded_value<T, MIN, MAX, VALIDATOR> const & lhs,
                   bounded_value<T, MIN, MAX, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() >= rhs._data.value();
        }

    private:
        internal::bounded_value_storage<T, VALIDATOR> _data;
    };

    template <typename T, T MIN, T MAX>
    struct clamping_validator
    {
        static_assert(MIN <= MAX, "Out of range");
        static_assert(std::is_integral<T>::value, "T must be integral");

        constexpr T operator()(T value) const noexcept
        {
#if __cplusplus >= 201703L
            return std::clamp(value, MIN, MAX);
#else
            return (value < MIN) ? MIN : (value > MAX) ? MAX : value;
#endif
        }
    };

    template <typename T, T MIN, T MAX>
    using clamped_value =
        bounded_value<T, MIN, MAX, clamping_validator<T, MIN, MAX>>;
} // namespace houseguest

namespace std
{
    template <>
    struct is_error_code_enum<houseguest::bounded_value_error> : true_type
    {
    };
} // namespace std

#endif
