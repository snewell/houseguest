#ifndef HOUSEGUEST_BOUNDED_VALUE_HPP
#define HOUSEGUEST_BOUNDED_VALUE_HPP 1

#if __cplusplus >= 201703L
#include <algorithm>
#endif

#include <system_error>

#include <houseguest/constrained_value.hpp>

namespace houseguest
{
    enum class bounded_value_error
    {
        success = 0,
        below_min,
        above_max
    };

    /// \brief an error category_instance for bounded_value
    struct bounded_value_error_category : std::error_category
    {
        /// \brief get name of error category
        const char * name() const noexcept override
        {
            return "houseguest::bounded_value";
        }

        /// \brief get message from error value
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

    namespace internal
    {
        inline std::error_code
        make_bounded_value_error_code(bounded_value_error error)
        {
            static bounded_value_error_category const category_instance{};
            return std::error_code{static_cast<int>(error), category_instance};
        }
    } // namespace internal

    template <typename T, T MIN, T MAX>
    struct bounded_validator
    {
        static_assert(MIN <= MAX, "Out of range");
        static_assert(std::is_integral<T>::value, "T must be integral");

        /// \brief the integral type being validated
        using type = T;

        /// \brief the manimum value that can be accepted
        static constexpr T min = MIN;

        /// \brief the maximum value that can be accepted
        static constexpr T max = MAX;
    };

    /** \brief a validator for bounded_value that emits exceptions for invalid
     *         values
     *
     * \tparam T   the integral type to operate on
     * \tparam MIN the minimum legal value
     * \tparam MAX the maximum legal value
     */
    template <typename T, T MIN, T MAX>
    struct exception_validator : bounded_validator<T, MIN, MAX>
    {
        /** \brief validate a value is between MIN and MAX
         *
         * \param value the value to validate
         *
         * \retval value The only thing this function will return.
         *
         * \throw std::error_code thrown if \a value doesn't fall between MIN
         *                        and MAX
         */
        constexpr T operator()(T value) const
        {
            validate_min(value);
            validate_max(value);
            return value;
        }

    private:
        /** \brief verify \a value is greater-than or equal to MIN
         *
         * \param value the value to validate
         *
         * \throw std::error_code thrown if \a value is less-than MIN
         */
        constexpr void validate_min(T value) const
        {
            if(value < MIN)
            {
                throw std::system_error{internal::make_bounded_value_error_code(
                    bounded_value_error::below_min)};
            }
        }

        /** \brief verify \a value is less-than or equal to MAX
         *
         * \param value the value to validate
         *
         * \throw std::error_code thrown if \a value is greater-than MAX
         */
        constexpr void validate_max(T value) const
        {
            if(value > MAX)
            {
                throw std::system_error{internal::make_bounded_value_error_code(
                    bounded_value_error::above_max)};
            }
        }
    };

    /** \brief a constrained_value that requires values fall between MIN and
     *         MAX
     *
     * \tparam T   the integral type to operate on
     * \tparam MIN the minimum legal value
     * \tparam MAX the maximum legal value
     */
    template <typename T, T MIN, T MAX>
    using bounded_value =
        constrained_value<T, exception_validator<T, MIN, MAX>>;

    /** \brief a validator that clamps values between MIN and MAX
     *
     * \tparam T   the integral type to operate on
     * \tparam MIN the minimum legal value
     * \tparam MAX the maximum legal value
     */
    template <typename T, T MIN, T MAX>
    struct clamping_validator : bounded_validator<T, MIN, MAX>
    {

        /** \brief clamp \a value to the specified range
         *
         * \param value the value to clamp
         *
         * \retval MIN   if \a value is less-than MIN
         * \retval MAX   if \a value is greater-than MAX
         * \retval value if \a value is between MIN and MAX
         */
        constexpr T operator()(T value) const noexcept
        {
#if __cplusplus >= 201703L
            return std::clamp(value, MIN, MAX);
#else
            return (value < MIN) ? MIN : (value > MAX) ? MAX : value;
#endif
        }
    };

    /** \brief a constrained_value that clamps all values values between MIN and
     *         MAX
     *
     * \tparam T   the integral type to operate on
     * \tparam MIN the minimum legal value
     * \tparam MAX the maximum legal value
     */
    template <typename T, T MIN, T MAX>
    using clamped_value = constrained_value<T, clamping_validator<T, MIN, MAX>>;

    // TODO: The is_validator_convertible specializations are *very* incomplete.
    //       These need to be replaced with something that just operates on
    //       bounded_validator to avoid exponential specializations.

    /// \brief specialization for exception_validator
    template <typename T, T MIN, T MAX, T OTHER_MIN, T OTHER_MAX>
    struct is_validator_convertible<
        exception_validator<T, MIN, MAX>,
        exception_validator<T, OTHER_MIN, OTHER_MAX>>
    {
        /// \brief validators are convertible if their ranges overlap
        constexpr static bool value = (MIN <= OTHER_MAX) && (MAX >= OTHER_MIN);
    };

    /// \brief specialization for clamping_validator
    template <typename T, T MIN, T MAX, T OTHER_MIN, T OTHER_MAX>
    struct is_validator_convertible<clamping_validator<T, MIN, MAX>,
                                    clamping_validator<T, OTHER_MIN, OTHER_MAX>>
    {
        /// \brief validators are convertible if their ranges overlap
        constexpr static bool value = (MIN <= OTHER_MAX) && (MAX >= OTHER_MIN);
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
