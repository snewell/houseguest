#ifndef HOUSEGUEST_THREAD_SAFE_OBJECT_HPP
#define HOUSEGUEST_THREAD_SAFE_OBJECT_HPP 1

#include <cassert>

#include <houseguest/mutex.hpp>

namespace houseguest
{
    /** \brief A class to provide access to a mutable object
     *
     * A write_handle provides mutable access to some object while holding a
     * lock.
     *
     * \tparam T     The type being managed
     * \tparam MUTEX The mutex being used to control access
     *
     * \note Because write_handles hold a lock during their entire lifetime,
     *       their scope should be as limited as possible.  Avoid using them in
     *       any fashion that conflicts with this, including as member
     *       variables in classes.
     */
    template <typename T, typename MUTEX>
    class
#if __cplusplus >= 201703L
    [[nodiscard]]
#endif
    write_handle
    {
    public:
        /// \brief The lock required by write_handle
        using lock_type = houseguest::unique_lock_t<MUTEX>;

        /** \brief Construct a write handle
         *
         * \param t    The object to manage
         * \param lock A lock that provides exclusive access to \a t
         */
        write_handle(T & t, lock_type lock)
          : _t{t}
          , _lock{std::move(lock)}
        {
            assert(_lock.owns_lock());
        }

        /** \brief Retreive a reference to the managed object
         *
         * \return A reference to the managed object
         */
        T & operator*() noexcept
        {
            assert(_lock.owns_lock());
            return _t;
        }

        /** \brief Retrieve a pointer to the managed object
         *
         * \return A pointer to the managed object
         */
        T * operator->() noexcept
        {
            assert(_lock.owns_lock());
            return &_t;
        }

    private:
        T & _t;
        lock_type _lock;
    };

    /** \brief A class to provide access to an immutable object
     *
     * A read_handle provides immutable access to some object while holding a
     * lock.  Multiple read_handles to the same object can exist, but the
     * existance of a single read_handle excludes the ability to create a
     * write_handle to the same object.
     *
     * \tparam T     The type being managed
     * \tparam MUTEX The mutex being used to control access
     *
     * \note Because read_handles hold a lock during their entire lifetime,
     *       their scope should be as limited as possible.  Avoid using them in
     *       any fashion that conflicts with this, including as member
     *       variables in classes.
     */
    template <typename T, typename MUTEX>
    class
#if __cplusplus >= 201703L
    [[nodiscard]]
#endif
    read_handle
    {
    public:
        /// \brief The lock required by read_handle
        using lock_type = houseguest::shared_lock_t<MUTEX>;

        /** \brief Construct a read_handle
         *
         * \param t    The object to manage
         * \param lock The lock to manage access to \a t
         */
        read_handle(T const & t, lock_type lock)
          : _t{t}
          , _lock{std::move(lock)}
        {
            assert(_lock.owns_lock());
        }

        /** \brief Retreive a reference to the managed object
         *
         * \return A reference to the managed object
         */
        T const & operator*() const noexcept
        {
            assert(_lock.owns_lock());
            return _t;
        }

        /** \brief Retreive a pointer to the managed object
         *
         * \return A pointer to the managed object
         */
        T const * operator->() const noexcept
        {
            assert(_lock.owns_lock());
            return &_t;
        }

    private:
        T const & _t;
        lock_type _lock;
    };

    /** \brief Provide a thread-safe wrapper around types
     *
     * threadsafe_object provides a method of ensuring an object has the
     * minimum required lock.
     *
     * \tparam T     The type to manage.
     * \tparam MUTEX The mutex to handle locking.  This type must support both
     *               unique and shared locks.
     */
    template <typename T, typename MUTEX = std::shared_mutex>
    class threadsafe_object
    {
    public:
        /// \brief The type that provides write access to a \a T
        using write_handle_type = write_handle<T, MUTEX>;

        /// \brief The type that provides read access to a \a T
        using read_handle_type = read_handle<T, MUTEX>;

        /** \brief Construct a threadsafe_object
         *
         * \tparam Ts Any extra types passed to the constructor
         *
         * \param ts Extra arguments passed to the constructor.  Arguments
         *           aren't required by threadsafe_object, but may be required
         *           by T.  If provided, they will be passed to T's constructor
         *           via std::forward.
         */
        template <typename... Ts>
        threadsafe_object(Ts &&... ts)
          : _t{std::forward<Ts>(ts)...}
        {
        }

        /** \brief Construct a write_handle for the underlying data
         *
         * An object can have at most one write_handle at any given time.  If
         * anybody calls this function while a write_handle to the managed T
         * already exists, the call will block until the original write_handle
         * is destroyed.
         *
         * This call may also block if any read_handles exist.
         *
         * \return A write_handle to modify the managed T
         */
        auto write()
        {
            typename write_handle_type::lock_type lock{_m};
            return write_handle_type{_t, std::move(lock)};
        }

        /** \brief Construct a read_handle for the underlying data
         *
         * An object can have as many read_handles as it wishes at one time,
         * provided there are no existing write_handles.
         *
         * \return A read_handle to modify the managed T
         */
        auto read() const
        {
            typename read_handle_type::lock_type lock{_m};
            return read_handle_type{_t, std::move(lock)};
        }

    private:
        T _t;
        mutable MUTEX _m;
    };
} // namespace houseguest

#endif
