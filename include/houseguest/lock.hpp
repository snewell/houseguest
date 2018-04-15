#ifndef HOUSEGUEST_LOCK_HPP
#define HOUSEGUEST_LOCK_HPP 1

#include <mutex>
#include <shared_mutex>

/** \file
 *
 * \brief Provide lock-related types
 *
 * The types here are provided so support client code specializing them for
 * their own lock types.
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
        /// \brief the actual type to use
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
        /// \brief the actual type to use
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
        /// \brief the actual type to use
        using type = std::shared_lock<MUTEX>;
    };

    /// \brief A convenience type for working with shared_lock
    template <typename MUTEX>
    using shared_lock_t = typename shared_lock<MUTEX>::type;

    /** \brief A collection of functions supported locks must provide
     *
     * To enhance flexibility in code that uses locks, houseguest won't assume
     * specific interfaces or functionality on the lock types.  Instead, it
     * will exercise functionality using specializations of this type.
     *
     * Users of houseguest are expected to specialize this for their own lock
     * types.
     *
     * \tparam LOCK The lock being exercised
     */
    template <typename LOCK>
    struct lock
    {
        /** \brief Determine if a lock is currently holding a resource
         *
         * \param lock The lock to test
         *
         * \retval true  @a lock believes it owns its resource
         * \retval false @a lock believes it does not own its resource
         */
        static bool owns_lock(LOCK const & lock);
    };

    // Specalizations should be on the actual type, not an alias provided by
    // houseguest.  If houseguest::lock_guard ends up not being an
    // std::lock_guard after template specialization (e.g., a boost mutex)
    // anything here that specializes on houseguest::lock_guard may break.

    /** \brief A lock specialization for std::lock_guard
     *
     * \tparam MUTEX Any mutex supported by std::lock_guard
     */
    template <typename MUTEX>
    struct lock<std::lock_guard<MUTEX>>
    {
        /// \cond false
        static constexpr bool owns_lock(std::lock_guard<MUTEX> const & lock)
        {
            // lock_guards can't release their locks, so they always think they
            // own the resource
            (void)lock;
            return true;
        }
        /// \endcond
    };

    /** \brief A lock specialization for std::unique_lock
     *
     * \tparam MUTEX Any mutex supported by std::unique_lock
     */
    template <typename MUTEX>
    struct lock<std::unique_lock<MUTEX>>
    {
        /// \cond false
        static constexpr bool owns_lock(std::unique_lock<MUTEX> const & lock)
        {
            return lock.owns_lock();
        }
        /// \endcond
    };

    /** \brief A lock specialization for std::shared_lock
     *
     * \tparam MUTEX Any mutex supported by std::shared_lock
     */
    template <typename MUTEX>
    struct lock<std::shared_lock<MUTEX>>
    {
        /// \cond false
        static constexpr bool owns_lock(std::shared_lock<MUTEX> const & lock)
        {
            return lock.owns_lock();
        }
        /// \endcond
    };
} // namespace houseguest

#endif
