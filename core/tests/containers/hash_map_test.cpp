// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(containers_hash_map_test, create_destroy) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(16, zero_alloc);

  EXPECT_EQ(0U, hash_map_count(&map));
  EXPECT_EQ(16U, hash_map_capacity(&map));

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, set_get) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(16, zero_alloc);

  b32 result = hash_map_set(&map, 1, (void*)100);
  EXPECT_NE(0, result);

  EXPECT_EQ(1U, hash_map_count(&map));

  void* val = hash_map_get(&map, 1);
  EXPECT_EQ((void*)100, val);

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, has) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(16, zero_alloc);

  EXPECT_EQ(0, hash_map_has(&map, 1));

  hash_map_set(&map, 1, (void*)100);
  EXPECT_NE(0, hash_map_has(&map, 1));

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, remove) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(16, zero_alloc);

  hash_map_set(&map, 1, (void*)100);
  EXPECT_EQ(1U, hash_map_count(&map));

  b32 result = hash_map_remove(&map, 1);
  EXPECT_NE(0, result);
  EXPECT_EQ(0U, hash_map_count(&map));
  EXPECT_EQ(0, hash_map_has(&map, 1));

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, multiple_keys) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(16, zero_alloc);

  hash_map_set(&map, 10, (void*)100);
  hash_map_set(&map, 20, (void*)200);
  hash_map_set(&map, 30, (void*)300);

  EXPECT_EQ(3U, hash_map_count(&map));
  EXPECT_EQ((void*)100, hash_map_get(&map, 10));
  EXPECT_EQ((void*)200, hash_map_get(&map, 20));
  EXPECT_EQ((void*)300, hash_map_get(&map, 30));

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, update_existing_key) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(16, zero_alloc);

  hash_map_set(&map, 1, (void*)100);
  hash_map_set(&map, 1, (void*)200);

  EXPECT_EQ(1U, hash_map_count(&map));
  EXPECT_EQ((void*)200, hash_map_get(&map, 1));

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, clear) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(16, zero_alloc);

  hash_map_set(&map, 1, (void*)100);
  hash_map_set(&map, 2, (void*)200);

  hash_map_clear(&map);
  EXPECT_EQ(0U, hash_map_count(&map));
  EXPECT_EQ(0, hash_map_has(&map, 1));

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, load_factor) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(16, zero_alloc);

  EXPECT_EQ(0.0F, hash_map_load_factor(&map));

  for (i32 i = 0; i < 8; i++) {
    hash_map_set(&map, (u64)i, &map);
  }

  f32 load_factor_val = hash_map_load_factor(&map);
  EXPECT_GT(load_factor_val, 0.0F);
  EXPECT_LE(load_factor_val, 1.0F);

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, reserve) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(4, zero_alloc);

  b32 result = hash_map_reserve(&map, 100);
  EXPECT_NE(0, result);
  EXPECT_GE(hash_map_capacity(&map), 100U);

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, rehash) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(4, zero_alloc);

  hash_map_set(&map, 1, (void*)100);
  hash_map_set(&map, 2, (void*)200);

  b32 result = hash_map_rehash(&map, 32);
  EXPECT_NE(0, result);
  EXPECT_EQ(32U, hash_map_capacity(&map));

  EXPECT_EQ((void*)100, hash_map_get(&map, 1));
  EXPECT_EQ((void*)200, hash_map_get(&map, 2));

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, iteration) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(16, zero_alloc);

  hash_map_set(&map, 1, (void*)100);
  hash_map_set(&map, 2, (void*)200);
  hash_map_set(&map, 3, (void*)300);

  sz count = 0;
  for (hash_map_iter it = hash_map_iter_begin();;) {
    hash_map_slot* slot = hash_map_next(&map, &it);
    if (slot == nullptr) {
      break;
    }
    count++;
    EXPECT_NE(0, slot->occupied);
    EXPECT_NE(nullptr, slot->value);
  }

  EXPECT_EQ(3U, count);

  hash_map_destroy(&map);
}

TEST(containers_hash_map_test, not_found) {
  allocator zero_alloc = {0};
  hash_map map = hash_map_create(16, zero_alloc);

  void* val = hash_map_get(&map, 999);
  EXPECT_EQ(nullptr, val);

  hash_map_destroy(&map);
}
