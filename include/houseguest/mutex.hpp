#ifndef HOUSEGUEST_MUTEX_HPP
#define HOUSEGUEST_MUTEX_HPP 1

#include <mutex>
#include <shared_mutex>

/** \file
 *
 * \brief Provide a method for specialization of mutex-related types
 *
 * The primary purpose of this file is limiting assumptions on the standard
 * library.
 */

namespace houseguest
{
    /// \brief A generic mutex
    using mutex = std::mutex;

    /// \brief A mutex that supports recursive locking and unlocking
    using recursive_mutex = std::recursive_mutex;

    /// \brief A mutex that supports both shared and exclusive locks
    using shared_mutex =
#if __cplusplus >= 201703L
        std::shared_mutex;
#else
        std::shared_timed_mutex;
#endif
} // namespace houseguest

#endif
