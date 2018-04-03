#ifndef HOUSEGUEST_THREAD_SAFE_OBJECT_HPP
#define HOUSEGUEST_THREAD_SAFE_OBJECT_HPP 1

#include <houseguest/mutex.hpp>

namespace houseguest
{
    template <typename T, typename MUTEX = std::shared_mutex>
    class threadsafe_object
    {
    public:
        class write_handle
        {
        public:
            write_handle(T & t, houseguest::unique_lock_t<MUTEX> lock)
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
            houseguest::unique_lock_t<MUTEX> _lock;
        };

        class read_handle
        {
        public:
            read_handle(T const & t, houseguest::shared_lock_t<MUTEX> lock)
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
            houseguest::shared_lock_t<MUTEX> _lock;
        };

        template <typename... Ts>
        threadsafe_object(Ts &&... ts)
          : _t{std::forward<Ts>(ts)...}
        {
        }

        auto write()
        {
            houseguest::unique_lock_t<MUTEX> lock{_m};
            return write_handle{_t, std::move(lock)};
        }

        auto read() const
        {
            houseguest::shared_lock_t<MUTEX> lock{_m};
            return read_handle{_t, std::move(lock)};
        }

    private:
        T _t;
        mutable MUTEX _m;
    };
} // namespace houseguest

#endif
