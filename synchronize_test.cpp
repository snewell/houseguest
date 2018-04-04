#include <houseguest/synchronize.hpp>

#include <gtest/gtest.h>

TEST(Synchronize, simple) // NOLINT
{
    std::mutex m;
    houseguest::synchronize(m, [&m]() { ASSERT_FALSE(m.try_lock()); });
}

TEST(Synchronize, simple_forward) // NOLINT
{
    std::mutex m;
    houseguest::synchronize(
        m, [](std::mutex & inner) { ASSERT_FALSE(inner.try_lock()); }, m);
}

TEST(Synchronize, simple_return) // NOLINT
{
    std::mutex m;
    auto ret = houseguest::synchronize(m, []() { return 12; });
    ASSERT_EQ(12, ret);
}

TEST(Synchronize, unique) // NOLINT
{
    std::mutex m;
    houseguest::synchronize_unique(m, [&m](std::unique_lock<std::mutex> lock) {
        ASSERT_TRUE(lock.owns_lock());
        ASSERT_FALSE(m.try_lock());
    });
}

TEST(Synchronize, unique_forward) // NOLINT
{
    std::mutex m;
    houseguest::synchronize_unique(
        m,
        [](std::unique_lock<std::mutex> lock, std::mutex & inner) {
            ASSERT_TRUE(lock.owns_lock());
            ASSERT_FALSE(inner.try_lock());
        },
        m);
}

TEST(Synchronize, unique_return) // NOLINT
{
    std::mutex m;
    auto ret = houseguest::synchronize_unique(
        m, [](std::unique_lock<std::mutex> lock) {
            (void)lock;
            return 12;
        });
    ASSERT_EQ(12, ret);
}

TEST(Synchronize, make_synchronize) // NOLINT
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

TEST(Synchronize, make_synchronize_forward) // NOLINT
{
    std::mutex m;
    auto sync_fn =
        houseguest::make_synchronize(m, [](std::mutex & inner, bool & called) {
            ASSERT_FALSE(inner.try_lock());
            called = true;
        });
    ASSERT_TRUE(m.try_lock());
    m.unlock();
    auto called = false;
    sync_fn(m, called);
    ASSERT_TRUE(called);
}

TEST(Synchronize, make_synchronize_return) // NOLINT
{
    std::mutex m;
    auto sync_fn = houseguest::make_synchronize(m, []() { return 12; });
    auto ret = sync_fn();
    ASSERT_EQ(12, ret);
}

TEST(Synchronize, make_synchronize_unique) // NOLINT
{
    auto called = false;
    std::mutex m;
    auto sync_fn = houseguest::make_synchronize_unique(
        m, [&m, &called](std::unique_lock<std::mutex> lock) {
            ASSERT_TRUE(lock.owns_lock());
            ASSERT_FALSE(m.try_lock());
            called = true;
        });
    ASSERT_TRUE(m.try_lock());
    m.unlock();
    sync_fn();
    ASSERT_TRUE(called);
}

TEST(Synchronize, make_synchronize_unique_forward) // NOLINT
{
    std::mutex m;
    auto sync_fn = houseguest::make_synchronize_unique(
        m, [](std::unique_lock<std::mutex> lock, std::mutex & inner,
              bool & called) {
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

TEST(Synchronize, make_synchronize_unique_return) // NOLINT
{
    std::mutex m;
    auto sync_fn = houseguest::make_synchronize_unique(
        m, [](std::unique_lock<std::mutex> lock) {
            (void)lock;
            return 12;
        });
    auto ret = sync_fn();
    ASSERT_EQ(12, ret);
}
