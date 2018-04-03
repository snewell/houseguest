#ifndef HOUSEGUEST_SYNCHRONIZE_HPP
#define HOUSEGUEST_SYNCHRONIZE_HPP 1

#include <mutex>

namespace houseguest
{
    template <typename MUTEX, typename FN, typename... Ts>
    auto synchronize(MUTEX & m, FN && fn, Ts &&... ts)
    {
        std::lock_guard<MUTEX> lock{m};
        return fn(std::forward<Ts>(ts)...);
    }

    template <typename MUTEX, typename FN, typename... Ts>
    auto synchronize_unique(MUTEX & m, FN && fn, Ts &&... ts)
    {
        std::unique_lock<MUTEX> lock{m};
        return fn(std::move(lock), std::forward<Ts>(ts)...);
    }

    template <typename MUTEX, typename FN>
    auto make_synchronize(MUTEX & m, FN && fn)
    {
        return [&m, fn = std::forward<FN>(fn)](auto &&... args) {
            return synchronize(m, fn, std::forward<decltype(args)>(args)...);
        };
    }

    template <typename MUTEX, typename FN>
    auto make_synchronize_unique(MUTEX & m, FN && fn)
    {
        return [&m, fn = std::forward<FN>(fn)](auto &&... args) {
            return synchronize_unique(m, fn,
                                      std::forward<decltype(args)>(args)...);
        };
    }
} // namespace houseguest

#endif
