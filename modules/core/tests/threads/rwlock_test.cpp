// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

namespace {

  typedef struct rwlock_writer_ctx {
    rwlock lock;
    atomic_u32* writer_active;
    atomic_u32* reader_can_proceed;
  } rwlock_writer_ctx;

  typedef struct rwlock_reader_ctx {
    rwlock lock;
    atomic_u32* reader_active;
    atomic_u32* writer_can_proceed;
  } rwlock_reader_ctx;

  func i32 rwlock_writer_entry(void* arg) {
    rwlock_writer_ctx* ctx = (rwlock_writer_ctx*)arg;
    rwlock_write_lock(ctx->lock);
    atomic_u32_set(ctx->writer_active, 1);
    safe_while (atomic_u32_get(ctx->reader_can_proceed) == 0) {
      thread_sleep(1);
    }
    rwlock_write_unlock(ctx->lock);
    return 0;
  }

  func i32 rwlock_reader_entry(void* arg) {
    rwlock_reader_ctx* ctx = (rwlock_reader_ctx*)arg;
    rwlock_read_lock(ctx->lock);
    atomic_u32_set(ctx->reader_active, 1);
    safe_while (atomic_u32_get(ctx->writer_can_proceed) == 0) {
      thread_sleep(1);
    }
    rwlock_read_unlock(ctx->lock);
    return 0;
  }

}  // namespace

TEST(threads_rwlock_test, create_destroy) {
  rwlock lock = rwlock_create();
  EXPECT_NE(0, rwlock_is_valid(lock));
  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, read_lock_unlock) {
  rwlock lock = rwlock_create();

  rwlock_read_lock(lock);
  rwlock_read_unlock(lock);

  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, write_lock_unlock) {
  rwlock lock = rwlock_create();

  rwlock_write_lock(lock);
  rwlock_write_unlock(lock);

  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, try_read_lock_success) {
  rwlock lock = rwlock_create();

  b32 result = rwlock_try_read_lock(lock);
  EXPECT_NE(0, result);

  rwlock_read_unlock(lock);
  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, try_read_lock_fails_with_writer) {
  rwlock lock = rwlock_create();
  atomic_u32 writer_active = {0};
  atomic_u32 reader_can_proceed = {0};
  rwlock_writer_ctx ctx = {lock, &writer_active, &reader_can_proceed};

  thread writer = thread_create(rwlock_writer_entry, &ctx, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(writer));

  safe_while (atomic_u32_get(&writer_active) == 0) {
    thread_sleep(1);
  }

  b32 result = rwlock_try_read_lock(lock);
  EXPECT_EQ(0, result);

  atomic_u32_set(&reader_can_proceed, 1);
  thread_join(writer, NULL);
  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, try_write_lock_success) {
  rwlock lock = rwlock_create();

  b32 result = rwlock_try_write_lock(lock);
  EXPECT_NE(0, result);

  rwlock_write_unlock(lock);
  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, try_write_lock_fails_with_reader) {
  rwlock lock = rwlock_create();
  atomic_u32 reader_active = {0};
  atomic_u32 writer_can_proceed = {0};
  rwlock_reader_ctx ctx = {lock, &reader_active, &writer_can_proceed};

  thread reader = thread_create(rwlock_reader_entry, &ctx, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(reader));

  safe_while (atomic_u32_get(&reader_active) == 0) {
    thread_sleep(1);
  }

  b32 result = rwlock_try_write_lock(lock);
  EXPECT_EQ(0, result);

  atomic_u32_set(&writer_can_proceed, 1);
  thread_join(reader, NULL);
  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, try_write_lock_fails_with_writer) {
  rwlock lock = rwlock_create();
  atomic_u32 writer_active = {0};
  atomic_u32 reader_can_proceed = {0};
  rwlock_writer_ctx ctx = {lock, &writer_active, &reader_can_proceed};

  thread writer = thread_create(rwlock_writer_entry, &ctx, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(writer));

  safe_while (atomic_u32_get(&writer_active) == 0) {
    thread_sleep(1);
  }

  b32 result = rwlock_try_write_lock(lock);
  EXPECT_EQ(0, result);

  atomic_u32_set(&reader_can_proceed, 1);
  thread_join(writer, NULL);
  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, timed_read_lock) {
  rwlock lock = rwlock_create();

  b32 result = rwlock_timed_read_lock(lock, 1000);
  EXPECT_NE(0, result);

  rwlock_read_unlock(lock);
  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, timed_write_lock) {
  rwlock lock = rwlock_create();

  b32 result = rwlock_timed_write_lock(lock, 1000);
  EXPECT_NE(0, result);

  rwlock_write_unlock(lock);
  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, multiple_readers) {
  rwlock lock = rwlock_create();

  rwlock_read_lock(lock);
  rwlock_read_lock(lock);
  rwlock_read_lock(lock);

  rwlock_read_unlock(lock);
  rwlock_read_unlock(lock);
  rwlock_read_unlock(lock);

  EXPECT_NE(0, rwlock_destroy(lock));
}

TEST(threads_rwlock_test, writer_excludes_reader) {
  rwlock lock = rwlock_create();
  atomic_u32 writer_active = {0};
  atomic_u32 reader_can_proceed = {0};
  rwlock_writer_ctx ctx = {lock, &writer_active, &reader_can_proceed};

  thread writer = thread_create(rwlock_writer_entry, &ctx, (ctx_setup) {0});
  EXPECT_NE(0, thread_is_valid(writer));

  safe_while (atomic_u32_get(&writer_active) == 0) {
    thread_sleep(1);
  }

  b32 result = rwlock_try_read_lock(lock);
  EXPECT_EQ(0, result);

  atomic_u32_set(&reader_can_proceed, 1);
  thread_join(writer, NULL);

  result = rwlock_try_read_lock(lock);
  EXPECT_NE(0, result);
  rwlock_read_unlock(lock);

  EXPECT_NE(0, rwlock_destroy(lock));
}
