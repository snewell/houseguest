#ifndef HOUSEGUEST_CONSTRAINED_VALUE_HPP
#define HOUSEGUEST_CONSTRAINED_VALUE_HPP 1

#include <tuple>
#include <type_traits>

namespace houseguest
{
    namespace internal
    {
        /** \brief a type to manage storage for constrained_value
         *
         * \internal
         *
         * constrained_value_storage is an abstraction for storing a
         * constrained_value's data.  This makes it easier to specialize down
         * the road (if desired), and to easily guarantee no memory overhead for
         * empty validators (this is the common scenario).
         *
         * \tparam T         the integral type being managed
         * \tparam VALIDATOR the validator being used
         */
        template <typename T, typename VALIDATOR>
        struct constrained_value_storage
        {
            /** \brief construct constrained_value_storage
             *
             * \param value     the initial value to use
             * \param validator the validator to own
             */
            constrained_value_storage(T && value, VALIDATOR && validator)
              : _data{make_storage(std::forward<T>(value), std::forward<VALIDATOR>(validator))}
            {
            }

            /// \brief get the stored value
            constexpr T & value() noexcept
            {
                return std::get<0>(_data);
            }

            /// \brief get the stored value
            constexpr T value() const noexcept
            {
                return std::get<0>(_data);
            }

            /// \brief get the stored validator
            constexpr VALIDATOR & validator() noexcept
            {
                return std::get<1>(_data);
            }

        private:
            // Using a tuple means the compiler should optimize away any
            // storage for empty VALIDATOR types.  Compiler Explorer seems to
            // verify this.
            using storage = std::tuple<T, VALIDATOR>;

            static storage make_storage(T && value, VALIDATOR && validator)
            {
                auto temp_value = validator(std::forward<T>(value));
                return std::make_tuple(std::move(temp_value), std::forward<VALIDATOR>(validator));
            }

            // make sure this optimization is turned on
            static_assert(!std::is_empty<VALIDATOR>::value ||
                              (sizeof(storage) == sizeof(T)),
                          "Non-empty validator is impacting size");

            storage _data;
        };
    } // namespace internal

    /** \brief a trait to determine if two validators are compatible
     *
     * \tparam FROM the validator being converted from
     * \tparam TO   the validator being converted to
     *
     * \note By default, validators are considered compatible.  This lets even
     *       incompatible validators attempt to convert, although that may
     *       result in runtime errors.
     */
    template <typename FROM, typename TO>
    struct is_validator_convertible
    {
        /// \brief a variable to determine if validators are convertible
        constexpr static bool value = true;
    };

    template <typename FROM, typename TO>
#if __cplusplus >= 201703L
    inline
#endif
    constexpr auto is_validator_convertible_v =
        is_validator_convertible<TO, FROM>::value;

    /** \brief An integral type constrained in some way
     *
     * \tparam T         The integral type to use for storage.  T must an
     *                   integral type.
     * \tparam VALIDATOR A type to perform validation of any potential values.
     *                   VALIDATOR is free to manipulate possible values in any
     *                   way it wishes.
     */
    template <typename T, typename VALIDATOR>
    struct constrained_value
    {
        static_assert(std::is_integral<T>::value, "T must be integral");

        /// \brief the integral type being managed
        using underlying_type = T;

        /// \brief the validator type
        using validator = VALIDATOR;

        /** \brief Construct a constrained_value
         *
         * \param value     The initial value.  \a value will be passed through
         *                  \a validator prior to being stored.
         * \param validator a VALIDATOR to use
         */
        explicit constexpr constrained_value(T && value,
                                             VALIDATOR && validator = VALIDATOR{})
          : _data{std::forward<T>(value), std::forward<VALIDATOR>(validator)}
        {
        }

        /** \brief Construct a constrained_value from a different
         * constrained_value
         *
         * \tparam OTHER_VALIDATOR The VALIDATOR used by \a other.  This
         *                         parameter is unused beyond matching
         *                         arbitrary constrained_values.
         *
         * \param other A constrained_value to construct from.  \a other must
         *              be constrained in a way that's compatible with the
         *              constrained_value being constructed (i.e., there's at
         *              least some overlap between valid ranges). \a other's
         *              value will be passed through \a validator.
         *              \param validator a VALIDATOR to use for the constructed
         *              constrained_value
         */
        template <typename OTHER_VALIDATOR>
        explicit constexpr constrained_value(
            constrained_value<T, OTHER_VALIDATOR> const & other,
            VALIDATOR && validator =
                VALIDATOR{}) noexcept(noexcept(validator(static_cast<T>(other))))
          : _data{static_cast<T>(other), std::forward<VALIDATOR>(validator)}
        {
            static_assert(
                is_validator_convertible<OTHER_VALIDATOR, VALIDATOR>::value,
                "Validators are not convertible");
        }

        /// \brief access the raw value
        constexpr operator T() const noexcept
        {
            return _data.value();
        }

        /// \cond false
        friend constexpr bool
        operator==(constrained_value<T, VALIDATOR> const & lhs,
                   constrained_value<T, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() == rhs._data.value();
        }

        friend constexpr bool
        operator!=(constrained_value<T, VALIDATOR> const & lhs,
                   constrained_value<T, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() != rhs._data.value();
        }

        friend constexpr bool
        operator<(constrained_value<T, VALIDATOR> const & lhs,
                  constrained_value<T, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() < rhs._data.value();
        }

        friend constexpr bool
        operator<=(constrained_value<T, VALIDATOR> const & lhs,
                   constrained_value<T, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() <= rhs._data.value();
        }

        friend constexpr bool
        operator>(constrained_value<T, VALIDATOR> const & lhs,
                  constrained_value<T, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() > rhs._data.value();
        }

        friend constexpr bool
        operator>=(constrained_value<T, VALIDATOR> const & lhs,
                   constrained_value<T, VALIDATOR> const & rhs) noexcept
        {
            return lhs._data.value() >= rhs._data.value();
        }
        /// \endcond

    private:
        internal::constrained_value_storage<T, VALIDATOR> _data;
    };
} // namespace houseguest

#endif
