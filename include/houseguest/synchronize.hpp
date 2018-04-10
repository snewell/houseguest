#ifndef HOUSEGUEST_SYNCHRONIZE_HPP
#define HOUSEGUEST_SYNCHRONIZE_HPP 1

#include <type_traits>

#include <houseguest/mutex.hpp>

/** \file
 *
 *  \brief Synchronization functionality provided by houseguest.
 */

namespace houseguest
{
    /** \brief Invoke a callable while locking a resource
     *
     * This is the most simple synchronization helper available.  It will lock
     * some resource (\a m) and invoke a callable object (\a fn).  Extra
     * arguments to \a fn can be passed as normal (they'll be picked up as
     * \a ts).
     *
     * If \a fn returns a value, it also be returned from this function.
     *
     * \tparam MUTEX Some lockable type.  This is used to create an
     *               std::lock_guard to handle the actual locking.
     * \tparam FN    A type that can be called like a function.
     * \tparam Ts    Any template arguments. These will only be used if extra
     *               arguments are passed to synchronize.
     *
     * \param m  Something that can be locked.  The lock will be held until
     *           \a fn completes.
     * \param fn A callable to invoke.
     * \param ts Extra arguments to pass to \a fn.  If \a ts is provided, it
     *        will be passed to \a fn using std::forward.
     *
     * \return The result of \a fn.
     */
    template <typename MUTEX, typename FN, typename... Ts>
    auto synchronize(MUTEX & m, FN && fn, Ts &&... ts)
    {
#if __cplusplus >= 201703L
        static_assert(std::is_invocable_v<FN, Ts...>,
                      "Incorrect function signature");
#endif
        houseguest::lock_guard_t<MUTEX> lock{m};
        return fn(std::forward<Ts>(ts)...);
    }

    /** \brief Synchronize using a unique_lock.
     *
     * This function is similar to synchronize, but uses an std::unique_lock
     * instead of an std::lock_guard.  This permits scenarios where \a fn
     * requires releasing a lock (e.g., waiting for a condition variable), but
     * is otherwise identical to synchronize.
     *
     * If \a fn returns a value, it will be returned from this function.
     *
     * \tparam MUTEX A type capable of being locked using std::unique_lock.
     * \tparam FN    A callable.  It must take an std::unique_lock as its first
     *               argument, but additional arguments can be passed.
     * \tparam Ts    Any extra template arguments.  These will only be used if
     *               additional arguments are passed.
     *
     * \param m  Something that can be locked using std::unique_lock.  After
     *           being aquired, the lock will be passed to \a fn as the first
     *           argument.
     * \param fn A callable to invoke.
     * \param ts Extra arguments to pass to \a fn.  If provided, they will be
     *           passed to \a fn via std::forward.
     *
     * \return The result of \a fn.
     */
    template <typename MUTEX, typename FN, typename... Ts>
    auto synchronize_unique(MUTEX & m, FN && fn, Ts &&... ts)
    {
        using lock_t = houseguest::unique_lock_t<MUTEX>;
        static_assert(std::is_move_constructible<lock_t>::value,
                      "unique_lock must be move constructable");
#if __cplusplus >= 201703L
        static_assert(std::is_invocable_v<FN, lock_t, Ts...>,
                      "Incorrect function signature");
#endif
        lock_t lock{m};
        return fn(std::move(lock), std::forward<Ts>(ts)...);
    }

    /** \brief Create a callable object that works like synchronize
     *
     * Instead of performing synchronization immediately, this will return a
     * callable object that performs synchronization when invoked.  The
     * returned callable be invoked with any arguments (including none),
     * provided \a fn can accept those arguments.
     *
     * Any extra arguments passed to the callable will be forwarded to \a fn
     * using std::forward.
     *
     * \tparam MUTEX A type that can be locked.
     * \tparam FN    A callable.
     *
     * \param m  A resource to lock when the returned callable is invoked.
     * \param fn The callable to invoke once \a m is aquired.
     *
     * \return A type that can be invoked as a function.
     *
     * \warning This function *will not* preserve \a m; it's the responsibility
     *          of whoever calls this funciton to ensure \a m remains valid as
     *          long as necessary.
     */
    template <typename MUTEX, typename FN>
#if __cplusplus >= 201703L
    [[nodiscard]] auto make_synchronize(MUTEX & m, FN && fn)
#else
    auto make_synchronize(MUTEX & m, FN && fn)
#endif
    {
        return [&m, fn = std::forward<FN>(fn)](auto &&... args) {
            return synchronize(m, fn, std::forward<decltype(args)>(args)...);
        };
    }

    /** \brief Create a callable object that works like synchronize_unique
     *
     * This is the synchronize_unique version of make_synchronize.  Instead of
     * immediately synchronizing on a resource, this will return a callable
     * object that operates like synchronize_unique when invoked.  Any number
     * of arguments can be passed to the callable this function returns
     * (they'll be passed along using std::forward).
     *
     * \tparam MUTEX A resource that can be locked.
     * \tparam FN    A callable type.
     *
     * \param m  The resource to lock.
     * \param fn The callable to invoke once \a m is aquired.  It must take an
     *           std::unique_lock<MUTEX> as its first argument.
     *
     * \return A callable that can be invoked as a function.
     *
     * \warning This funciton *will not* ensure the lifetype of \a m; it's the
     *          responsibility of whomever calls this function to ensure \a m
     *          remains a valid object so long as anybody can call the result
     *          of this function.
     */
    template <typename MUTEX, typename FN>
#if __cplusplus >= 201703L
    [[nodiscard]] auto make_synchronize_unique(MUTEX & m, FN && fn)
#else
    auto make_synchronize_unique(MUTEX & m, FN && fn)
#endif
    {
        return [&m, fn = std::forward<FN>(fn)](auto &&... args) {
            return synchronize_unique(m, fn,
                                      std::forward<decltype(args)>(args)...);
        };
    }
} // namespace houseguest

#endif
