#ifndef HOUSEGUEST_THREAD_SAFE_OBJECT_HPP
#define HOUSEGUEST_THREAD_SAFE_OBJECT_HPP 1

#include <houseguest/mutex.hpp>

namespace houseguest
{
    template <typename T, typename MUTEX>
    class write_handle
    {
    public:
        using lock_type = houseguest::unique_lock_t<MUTEX>;

        write_handle(T & t, lock_type lock)
          : _t{t}
          , _lock{std::move(lock)}
        {
        }

        T & operator*() noexcept
        {
            return _t;
        }

        T * operator->() noexcept
        {
            return &_t;
        }

    private:
        T & _t;
        lock_type _lock;
    };

    template <typename T, typename MUTEX>
    class read_handle
    {
    public:
        using lock_type = houseguest::shared_lock_t<MUTEX>;

        read_handle(T const & t, lock_type lock)
          : _t{t}
          , _lock{std::move(lock)}
        {
        }

        T const & operator*() const noexcept
        {
            return _t;
        }

        T const * operator->() const noexcept
        {
            return &_t;
        }

    private:
        T const & _t;
        lock_type _lock;
    };

    template <typename T, typename MUTEX = std::shared_mutex>
    class threadsafe_object
    {
    public:
        using write_handle_type = write_handle<T, MUTEX>;

        using read_handle_type = read_handle<T, MUTEX>;

        template <typename... Ts>
        threadsafe_object(Ts &&... ts)
          : _t{std::forward<Ts>(ts)...}
        {
        }

        auto write()
        {
            typename write_handle_type::lock_type lock{_m};
            return write_handle_type{_t, std::move(lock)};
        }

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
