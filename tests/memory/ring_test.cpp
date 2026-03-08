// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(memory_ring_test, create_with_buffer) {
  u8 buf[256];
  ring rng = ring_create(buf, sizeof(buf), nullptr);
  EXPECT_EQ(0U, ring_size(&rng));
  EXPECT_EQ(sizeof(buf), ring_capacity(&rng));
  EXPECT_EQ(sizeof(buf), ring_space(&rng));
  ring_destroy(&rng);
}

TEST(memory_ring_test, create_alloc) {
  allocator zero_alloc = {0};
  ring rng = ring_create_alloc(zero_alloc, 256, nullptr);
  EXPECT_EQ(0U, ring_size(&rng));
  EXPECT_EQ(256U, ring_capacity(&rng));
  ring_destroy(&rng);
}

TEST(memory_ring_test, write_single) {
  u8 buf[256];
  ring rng = ring_create(buf, sizeof(buf), nullptr);
  u8 data[] = {1, 2, 3, 4, 5};
  sz written = ring_write(&rng, data, sizeof(data));
  EXPECT_EQ(5U, written);
  EXPECT_EQ(5U, ring_size(&rng));
  EXPECT_EQ(251U, ring_space(&rng));
  ring_destroy(&rng);
}

TEST(memory_ring_test, write_overflow) {
  u8 buf[16];
  ring rng = ring_create(buf, sizeof(buf), nullptr);
  u8 data[32];
  buffer_set8(buffer_from(data, sizeof(data)), 0xAB);
  sz written = ring_write(&rng, data, sizeof(data));
  EXPECT_EQ(16U, written);
  EXPECT_EQ(16U, ring_size(&rng));
  EXPECT_EQ(0U, ring_space(&rng));
  ring_destroy(&rng);
}

TEST(memory_ring_test, read_basic) {
  u8 buf[256];
  ring rng = ring_create(buf, sizeof(buf), nullptr);
  u8 write_data[] = {10, 20, 30, 40, 50};
  ring_write(&rng, write_data, sizeof(write_data));

  u8 read_data[5] = {0};
  sz read_count = ring_read(&rng, read_data, sizeof(read_data));
  EXPECT_EQ(5U, read_count);
  EXPECT_EQ(10, read_data[0]);
  EXPECT_EQ(20, read_data[1]);
  EXPECT_EQ(30, read_data[2]);
  EXPECT_EQ(40, read_data[3]);
  EXPECT_EQ(50, read_data[4]);
  EXPECT_EQ(0U, ring_size(&rng));
  ring_destroy(&rng);
}

TEST(memory_ring_test, read_partial) {
  u8 buf[256];
  ring rng = ring_create(buf, sizeof(buf), nullptr);
  u8 write_data[] = {1, 2, 3, 4, 5};
  ring_write(&rng, write_data, sizeof(write_data));

  u8 read_data[2] = {0};
  sz read_count = ring_read(&rng, read_data, 2);
  EXPECT_EQ(2U, read_count);
  EXPECT_EQ(1, read_data[0]);
  EXPECT_EQ(2, read_data[1]);
  EXPECT_EQ(3U, ring_size(&rng));
  ring_destroy(&rng);
}

TEST(memory_ring_test, peek) {
  u8 buf[256];
  ring rng = ring_create(buf, sizeof(buf), nullptr);
  u8 write_data[] = {100, 101, 102, 103, 104};
  ring_write(&rng, write_data, sizeof(write_data));

  u8 peek_data[5] = {0};
  sz peek_count = ring_peek(&rng, peek_data, sizeof(peek_data));
  EXPECT_EQ(5U, peek_count);
  EXPECT_EQ(100, peek_data[0]);
  EXPECT_EQ(5U, ring_size(&rng));

  u8 peek_data2[5] = {0};
  sz peek_count2 = ring_peek(&rng, peek_data2, sizeof(peek_data2));
  EXPECT_EQ(5U, peek_count2);
  EXPECT_EQ(100, peek_data2[0]);
  ring_destroy(&rng);
}

TEST(memory_ring_test, skip) {
  u8 buf[256];
  ring rng = ring_create(buf, sizeof(buf), nullptr);
  u8 write_data[] = {1, 2, 3, 4, 5};
  ring_write(&rng, write_data, sizeof(write_data));

  sz skipped = ring_skip(&rng, 2);
  EXPECT_EQ(2U, skipped);
  EXPECT_EQ(3U, ring_size(&rng));

  u8 read_data[3] = {0};
  ring_read(&rng, read_data, 3);
  EXPECT_EQ(3, read_data[0]);
  EXPECT_EQ(4, read_data[1]);
  EXPECT_EQ(5, read_data[2]);
  ring_destroy(&rng);
}

TEST(memory_ring_test, clear) {
  u8 buf[256];
  ring rng = ring_create(buf, sizeof(buf), nullptr);
  u8 data[] = {1, 2, 3};
  ring_write(&rng, data, sizeof(data));
  EXPECT_GT(ring_size(&rng), 0U);

  ring_clear(&rng);
  EXPECT_EQ(0U, ring_size(&rng));
  EXPECT_EQ(256U, ring_space(&rng));
  ring_destroy(&rng);
}

TEST(memory_ring_test, wrap_around_write_read) {
  u8 buf[16];
  ring rng = ring_create(buf, sizeof(buf), nullptr);

  u8 first[10];
  buffer_set8(buffer_from(first, 10), 0xAA);
  ring_write(&rng, first, 10);

  u8 temp[8];
  ring_read(&rng, temp, 8);

  u8 second[10];
  buffer_set8(buffer_from(second, 10), 0xBB);
  sz written = ring_write(&rng, second, 10);
  EXPECT_EQ(10U, written);

  EXPECT_EQ(12U, ring_size(&rng));

  u8 read_data[12] = {0};
  ring_read(&rng, read_data, 12);
  EXPECT_EQ(0xAA, read_data[0]);
  EXPECT_EQ(0xAA, read_data[1]);
  EXPECT_EQ(0xBB, read_data[2]);
  EXPECT_EQ(0xBB, read_data[11]);
  ring_destroy(&rng);
}

TEST(memory_ring_test, empty_read) {
  u8 buf[256];
  ring rng = ring_create(buf, sizeof(buf), nullptr);
  u8 read_data[10] = {0xFF};
  sz read_count = ring_read(&rng, read_data, 10);
  EXPECT_EQ(0U, read_count);
  ring_destroy(&rng);
}

TEST(memory_ring_test, empty_peek) {
  u8 buf[256];
  ring rng = ring_create(buf, sizeof(buf), nullptr);
  u8 peek_data[10] = {0xFF};
  sz peek_count = ring_peek(&rng, peek_data, 10);
  EXPECT_EQ(0U, peek_count);
  ring_destroy(&rng);
}
