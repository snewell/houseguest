#ifndef HOUSEGUEST_ODDEVEN_VALUE_HPP
#define HOUSEGUEST_ODDEVEN_VALUE_HPP 1

#include <type_traits>

namespace houseguest
{
    template <typename T, bool EVEN>
    struct oddeven_validator
    {
        static_assert(std::is_integral<T>::value, "T must be integral");

        T operator()(T val)
        {
            if((val & 1) != EVEN)
            {
            }
            return val;
        }
    };
} // namespace houseguest

#endif
