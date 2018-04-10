#ifndef HOUSEGUEST_MUTEX_HPP
#define HOUSEGUEST_MUTEX_HPP 1

#include <mutex>
#include <shared_mutex>

/** \file
 *
 * \brief Provide a method for specialization of mutex-related types
 *
 * The types in this file follow the pattern set by type_traits.  Each struct
 * provides contains a type alias to the appropriate type.  Default
 * implementations will point to types provided by the standard library when
 * possible, but specializations can be provided as needed in client code.
 *
 * Convenience types (e.g., lock_guard_t) are provided as well.
 */

namespace houseguest
{
    /** \brief A type that mimics std::lock_guard
     *
     * lock_guard is expected to lock a mutex in the constructor and unlock in
     * its destructor.
     *
     * \tparam MUTEX The mutex to specialize for
     */
    template <typename MUTEX>
    struct lock_guard
    {
        using type = std::lock_guard<MUTEX>;
    };

    /// \brief A convenience type for working with lock_guard
    template <typename MUTEX>
    using lock_guard_t = typename lock_guard<MUTEX>::type;

    /** \brief a type that mimics std::unique_lock
     *
     * Unlike lock_guard, a unique_lock can be unlocked without being
     * destroyed.  This means htey can work with types similar to
     * std::condition_variable.
     *
     * \tparam MUTEX The mutex to spcialize for
     */
    template <typename MUTEX>
    struct unique_lock
    {
        using type = std::unique_lock<MUTEX>;
    };

    /// \brief A convenience type for working with unique_lock
    template <typename MUTEX>
    using unique_lock_t = typename unique_lock<MUTEX>::type;

    /** \brief A type that mimics std::shared_lock
     *
     * A shared_lock can be locked by multiple threads at the same time.  It's
     * primarily used for scenarios like readers/writers, where many threads
     * will be readers (shared_lock) but only one can be a writer
     * (unique_lock).
     *
     * \tparam MUTEX The mutex type to specialize for
     */
    template <typename MUTEX>
    struct shared_lock
    {
        using type = std::shared_lock<MUTEX>;
    }
    ;

    /// \brief A convenience type for working with shared_lock
    template <typename MUTEX>
    using shared_lock_t = typename shared_lock<MUTEX>::type;
} // namespace houseguest

#endif
