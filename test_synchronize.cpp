#include <houseguest/synchronize.hpp>

#include <gtest/gtest.h>

TEST(Synchronize, simple)
{
    std::mutex m;
    houseguest::synchronize(m, [&m]() {
        ASSERT_FALSE(m.try_lock());
    });
}

TEST(Synchronize, simple_forward)
{
    std::mutex m;
    houseguest::synchronize(m, [](std::mutex &inner) {
        ASSERT_FALSE(inner.try_lock());
    }, m);
}

TEST(Synchronize, unique)
{
    std::mutex m;
    houseguest::synchronize_unique(m, [&m](std::unique_lock<std::mutex> lock) {
        ASSERT_TRUE(lock.owns_lock());
        ASSERT_FALSE(m.try_lock());
    });
}

TEST(Synchronize, unique_forward)
{
    std::mutex m;
    houseguest::synchronize_unique(m, [](std::unique_lock<std::mutex> lock, std::mutex &inner) {
        ASSERT_TRUE(lock.owns_lock());
        ASSERT_FALSE(inner.try_lock());
    }, m);
}

TEST(Synchronize, make_synchronize)
{
    auto called = false;
    std::mutex m;
    auto sync_fn = houseguest::make_synchronize(m, [&m, &called]() {
        ASSERT_FALSE(m.try_lock());
        called = true;
    });
    ASSERT_TRUE(m.try_lock());
    m.unlock();
    sync_fn();
    ASSERT_TRUE(called);
}

TEST(Synchronize, make_synchronize_forward)
{
    std::mutex m;
    auto sync_fn = houseguest::make_synchronize(m, [](std::mutex &inner, bool & called) {
        ASSERT_FALSE(inner.try_lock());
        called = true;
    });
    ASSERT_TRUE(m.try_lock());
    m.unlock();
    auto called = false;
    sync_fn(m, called);
    ASSERT_TRUE(called);
}

TEST(Synchronize, make_synchronize_unique)
{
    auto called = false;
    std::mutex m;
    auto sync_fn = houseguest::make_synchronize_unique(m, [&m, &called](std::unique_lock<std::mutex> lock) {
        ASSERT_TRUE(lock.owns_lock());
        ASSERT_FALSE(m.try_lock());
        called = true;
    });
    ASSERT_TRUE(m.try_lock());
    m.unlock();
    sync_fn();
    ASSERT_TRUE(called);
}

TEST(Synchronize, make_synchronize_unique_forward)
{
    std::mutex m;
    auto sync_fn = houseguest::make_synchronize_unique(m, [](std::unique_lock<std::mutex> lock, std::mutex &inner, bool & called) {
        ASSERT_TRUE(lock.owns_lock());
        ASSERT_FALSE(inner.try_lock());
        called = true;
    });
    ASSERT_TRUE(m.try_lock());
    m.unlock();
    auto called = false;
    sync_fn(m, called);
    ASSERT_TRUE(called);
}
