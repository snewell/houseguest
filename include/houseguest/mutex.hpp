#ifndef HOUSEGUEST_MUTEX_HPP
#define HOUSEGUEST_MUTEX_HPP 1

#include <mutex>

#if __cplusplus == 201703L
#include <shared_mutex>
#endif

namespace houseguest
{
    template <typename MUTEX>
    struct lock_guard
    {
        using type = std::lock_guard<MUTEX>;
    };

    template <typename MUTEX>
    using lock_guard_t = typename lock_guard<MUTEX>::type;

    template <typename MUTEX>
    struct unique_lock
    {
        using type = std::unique_lock<MUTEX>;
    };

    template <typename MUTEX>
    using unique_lock_t = typename unique_lock<MUTEX>::type;

    template <typename MUTEX>
    struct shared_lock
#if __cplusplus == 201703L
    {
        using type = std::shared_lock<MUTEX>;
    }
#endif
    ;

    template <typename MUTEX>
    using shared_lock_t = typename shared_lock<MUTEX>::type;
} // namespace houseguest

#endif
