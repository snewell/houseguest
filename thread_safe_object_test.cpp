#include <houseguest/thread_safe_object.hpp>

#include <future>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include <houseguest/synchronize.hpp>

TEST(ThreadSafeObject, default_ctor) // NOLINT
{
    houseguest::threadsafe_object<std::vector<int>> tsv;
    auto handle = tsv.read();
    ASSERT_TRUE(handle->empty());
    ASSERT_EQ(0, handle->size());
}

TEST(ThreadSafeObject, args_ctor) // NOLINT
{
    houseguest::threadsafe_object<std::vector<int>> tsv{10};
    auto handle = tsv.read();
    ASSERT_FALSE(handle->empty());
    ASSERT_EQ(1, handle->size());
}

TEST(ThreadSafeObject, write_handle) // NOLINT
{
    houseguest::threadsafe_object<std::vector<int>> tsv;
    auto handle = tsv.write();
    handle->push_back(10);
    ASSERT_FALSE(handle->empty());
    ASSERT_EQ(1, handle->size());
}

TEST(ThreadSafeObject, multiple_read_handles) // NOLINT
{
    houseguest::threadsafe_object<int> tsi;
    auto handle1 = tsi.read();
    auto handle2 = tsi.read();
}

TEST(ThreadSafeObject, copy_read_handles) // NOLINT
{
    houseguest::threadsafe_object<int> tsi;
    auto handle1 = tsi.read();
    auto handle2{handle1};
}

TEST(ThreadSafeObject, multi_thread_read) // NOLINT
{
    houseguest::threadsafe_object<int> tsi;

    std::mutex m;
    auto make_thread_fn = [&m, &tsi](auto &local_promise, auto &local_cv) {
        return houseguest::make_synchronize_unique(m, [&tsi, &local_promise, &local_cv](auto lock) {
            auto handle = tsi.read();
            local_promise.set_value();
            local_cv.wait(lock);
        });
    };

    houseguest::synchronize_unique(m, [&make_thread_fn](auto lock) {
        std::promise<void> p1;
        std::promise<void> p2;
        std::condition_variable c1;
        std::condition_variable c2;

        std::thread ts[] = { std::thread{make_thread_fn(p1, c1)},
                             std::thread{make_thread_fn(p2, c2)} };

        lock.unlock();
        p1.get_future().get();
        p2.get_future().get();
        lock.lock();
        c1.notify_one();
        c2.notify_one();
        lock.unlock();
        std::for_each(std::begin(ts), std::end(ts), [](auto &t) {
            t.join();
        });
    });
}

#if 0
// This test is excluded on purpose.  Because a read_handle is used to call a
// non-const member function, this code *should not* compile.  This test should
// be enabled only to verify the code will not compile.
TEST(ThreadSafeObject, read_handle_failure)
{
    houseguest::threadsafe_object<std::vector<int>> tsv;
    auto handle = tsv.read();
    handle->push_back(10);
}
#endif
