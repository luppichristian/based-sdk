// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(utils_uuid_test, zero) {
  uuid value = uuid_zero();
  EXPECT_TRUE(uuid_is_zero(value));
  EXPECT_EQ(0, uuid_cmp(value, uuid_zero()));
}

TEST(utils_uuid_test, from_bytes) {
  u8 bytes[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  uuid value = uuid_from_bytes(bytes);
  EXPECT_FALSE(uuid_is_zero(value));
  u8 out[16];
  uuid_get_bytes(value, out);
  EXPECT_TRUE(buffer_cmp(buffer_from(bytes, 16), buffer_from(out, 16)) != 0);
}

TEST(utils_uuid_test, from_u64) {
  uuid value = uuid_from_u64(0x0102030405060708ULL, 0x090A0B0C0D0E0F10ULL);
  EXPECT_EQ(0x0102030405060708ULL, uuid_get_upper(value));
  EXPECT_EQ(0x090A0B0C0D0E0F10ULL, uuid_get_lower(value));
}

TEST(utils_uuid_test, equal) {
  uuid lhs = uuid_from_u64(1, 2);
  uuid rhs = uuid_from_u64(1, 2);
  uuid val = uuid_from_u64(1, 3);
  EXPECT_TRUE(uuid_equal(lhs, rhs));
  EXPECT_FALSE(uuid_equal(lhs, val));
}

TEST(utils_uuid_test, cmp) {
  uuid first = uuid_from_u64(1, 2);
  uuid second = uuid_from_u64(1, 2);
  uuid greater = uuid_from_u64(1, 3);
  EXPECT_EQ(0, uuid_cmp(first, second));
  EXPECT_LT(uuid_cmp(first, greater), 0);
  EXPECT_GT(uuid_cmp(greater, first), 0);
}

TEST(utils_uuid_test, parse_cstr8) {
  uuid value;
  b32 success = uuid_parse_cstr8("550e8400-e29b-41d4-a716-446655440000", &value);
  EXPECT_TRUE(success != 0);
  u8 expected[16] = {0x55, 0x0e, 0x84, 0x00, 0xe2, 0x9b, 0x41, 0xd4, 0xa7, 0x16, 0x44, 0x66, 0x55, 0x44, 0x00, 0x00};
  u8 out[16];
  uuid_get_bytes(value, out);
  EXPECT_TRUE(buffer_cmp(buffer_from(expected, 16), buffer_from(out, 16)) != 0);
}

TEST(utils_uuid_test, to_cstr8) {
  u8 bytes[16] = {0x55, 0x0e, 0x84, 0x00, 0xe2, 0x9b, 0x41, 0xd4, 0xa7, 0x16, 0x44, 0x66, 0x55, 0x44, 0x00, 0x00};
  uuid value = uuid_from_bytes(bytes);
  c8 buf[37];
  b32 success = uuid_to_cstr8(value, buf, sizeof(buf));
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("550e8400-e29b-41d4-a716-446655440000", buf);
}

TEST(utils_uuid_test, string_length) {
  EXPECT_EQ(36U, uuid_string_length());
}

TEST(utils_uuid_test, generate_v4) {
  uuid value = uuid_generate_v4();
  EXPECT_FALSE(uuid_is_zero(value));
  u8 ver = uuid_get_version(value);
  EXPECT_EQ(4U, ver);
  u8 variant = uuid_get_variant(value);
  EXPECT_EQ(2U, variant);
}

TEST(utils_uuid_test, version_nibble) {
  uuid value = uuid_generate_v4();
  u8 ver = uuid_get_version(value);
  EXPECT_GE(ver, 1U);
  EXPECT_LE(ver, 8U);
}
