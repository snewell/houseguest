#include <houseguest/thread_safe_object.hpp>

#include <vector>

#include <gtest/gtest.h>

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
