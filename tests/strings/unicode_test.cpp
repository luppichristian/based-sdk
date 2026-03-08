// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(strings_unicode_test, unicode_is_valid) {
  EXPECT_TRUE(unicode_is_valid(0) != 0);
  EXPECT_TRUE(unicode_is_valid('A') != 0);
  EXPECT_TRUE(unicode_is_valid(0xD7FF) != 0);
  EXPECT_TRUE(unicode_is_valid(0xE000) != 0);
  EXPECT_TRUE(unicode_is_valid(0x10FFFF) != 0);
  EXPECT_FALSE(unicode_is_valid(0xD800) != 0);
  EXPECT_FALSE(unicode_is_valid(0xDFFF) != 0);
  EXPECT_FALSE(unicode_is_valid(0x110000) != 0);
}

TEST(strings_unicode_test, utf8_encode_size) {
  EXPECT_EQ(1U, utf8_encode_size('A'));
  EXPECT_EQ(2U, utf8_encode_size(0x80));
  EXPECT_EQ(3U, utf8_encode_size(0x800));
  EXPECT_EQ(4U, utf8_encode_size(0x10000));
  EXPECT_EQ(0U, utf8_encode_size(0xD800));
}

TEST(strings_unicode_test, utf8_byte_count) {
  EXPECT_EQ(1U, utf8_byte_count(0x00));
  EXPECT_EQ(1U, utf8_byte_count(0x7F));
  EXPECT_EQ(2U, utf8_byte_count(0xC0));
  EXPECT_EQ(2U, utf8_byte_count(0xDF));
  EXPECT_EQ(3U, utf8_byte_count(0xE0));
  EXPECT_EQ(3U, utf8_byte_count(0xEF));
  EXPECT_EQ(4U, utf8_byte_count(0xF0));
  EXPECT_EQ(4U, utf8_byte_count(0xF7));
  EXPECT_EQ(0U, utf8_byte_count(0x80));
}

TEST(strings_unicode_test, utf8_decode) {
  c8 src[] = "A";
  sz consumed = 0;
  c32 codepoint = utf8_decode(src, &consumed);
  EXPECT_EQ('A', codepoint);
  EXPECT_EQ(1U, consumed);
}

TEST(strings_unicode_test, utf8_decode_multibyte) {
  c8 src[] = {(c8)0xC3, (c8)0xA9, 0};
  sz consumed = 0;
  c32 codepoint = utf8_decode(src, &consumed);
  EXPECT_EQ(0x00E9, codepoint);
  EXPECT_EQ(2U, consumed);
}

TEST(strings_unicode_test, utf8_encode) {
  c8 buf[8];
  sz written = utf8_encode('A', buf);
  EXPECT_EQ(1U, written);
  EXPECT_EQ('A', buf[0]);

  written = utf8_encode(0x00E9, buf);
  EXPECT_EQ(2U, written);
  EXPECT_EQ(0xC3, (u8)buf[0]);
  EXPECT_EQ(0xA9, (u8)buf[1]);

  written = utf8_encode(0xD800, buf);
  EXPECT_EQ(0U, written);
}

TEST(strings_unicode_test, utf8_codepoint_count) {
  EXPECT_EQ(5U, utf8_codepoint_count("hello", 5));
  EXPECT_EQ(1U, utf8_codepoint_count((c8*)"\xC3\xA9", 2));
  EXPECT_EQ(1U, utf8_codepoint_count((c8*)"\xE2\x82\xAC", 3));
}

TEST(strings_unicode_test, utf16_encode_size) {
  EXPECT_EQ(1U, utf16_encode_size('A'));
  EXPECT_EQ(1U, utf16_encode_size(0xFFFF));
  EXPECT_EQ(2U, utf16_encode_size(0x10000));
  EXPECT_EQ(0U, utf16_encode_size(0xD800));
}

TEST(strings_unicode_test, utf16_decode) {
  c16 src[] = {'A', 0};
  sz consumed = 0;
  c32 codepoint = utf16_decode(src, &consumed);
  EXPECT_EQ('A', codepoint);
  EXPECT_EQ(1U, consumed);
}

TEST(strings_unicode_test, utf16_decode_surrogate) {
  c16 src[] = {0xD83D, 0xDE00, 0};
  sz consumed = 0;
  c32 codepoint = utf16_decode(src, &consumed);
  EXPECT_EQ(0x1F600, codepoint);
  EXPECT_EQ(2U, consumed);
}

TEST(strings_unicode_test, utf16_encode) {
  c16 buf[4];
  sz written = utf16_encode('A', buf);
  EXPECT_EQ(1U, written);
  EXPECT_EQ('A', buf[0]);

  written = utf16_encode(0x1F600, buf);
  EXPECT_EQ(2U, written);
  EXPECT_EQ(0xD83D, buf[0]);
  EXPECT_EQ(0xDE00, buf[1]);

  written = utf16_encode(0xD800, buf);
  EXPECT_EQ(0U, written);
}

TEST(strings_unicode_test, utf16_codepoint_count) {
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  EXPECT_EQ(5U, utf16_codepoint_count(src, 5));

  c16 src_surrogate[] = {0xD83D, 0xDE00, 0};
  EXPECT_EQ(1U, utf16_codepoint_count(src_surrogate, 2));
}

TEST(strings_unicode_test, utf8_to_utf16) {
  c8 src[] = "hello";
  c16 dst[16];
  sz written = utf8_to_utf16(src, 5, dst, sizeof(dst));
  EXPECT_EQ(5U, written);
  EXPECT_EQ('h', dst[0]);
  EXPECT_EQ('e', dst[1]);
  EXPECT_EQ('l', dst[2]);
}

TEST(strings_unicode_test, utf8_to_utf16_multibyte) {
  c8 src[] = {(c8)0xC3, (c8)0xA9, 0};
  c16 dst[16];
  sz written = utf8_to_utf16(src, 2, dst, sizeof(dst));
  EXPECT_EQ(1U, written);
  EXPECT_EQ(0x00E9, dst[0]);
}

TEST(strings_unicode_test, utf8_to_utf16_count) {
  c8 src[] = "hello";
  sz count = utf8_to_utf16(src, 5, nullptr, 0);
  EXPECT_EQ(5U, count);
}

TEST(strings_unicode_test, utf8_to_utf32) {
  c8 src[] = "hello";
  c32 dst[16];
  sz written = utf8_to_utf32(src, 5, dst, sizeof(dst));
  EXPECT_EQ(5U, written);
  EXPECT_EQ('h', dst[0]);
  EXPECT_EQ('e', dst[1]);
}

TEST(strings_unicode_test, utf8_to_utf32_multibyte) {
  c8 src[] = {(c8)0xC3, (c8)0xA9, 0};
  c32 dst[16];
  sz written = utf8_to_utf32(src, 2, dst, sizeof(dst));
  EXPECT_EQ(1U, written);
  EXPECT_EQ(0x00E9, dst[0]);
}

TEST(strings_unicode_test, utf8_to_utf32_count) {
  c8 src[] = "hello";
  sz count = utf8_to_utf32(src, 5, nullptr, 0);
  EXPECT_EQ(5U, count);
}

TEST(strings_unicode_test, utf16_to_utf8) {
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c8 dst[16];
  sz written = utf16_to_utf8(src, 5, dst, sizeof(dst));
  EXPECT_EQ(5U, written);
  EXPECT_STREQ("hello", dst);
}

TEST(strings_unicode_test, utf16_to_utf8_multibyte) {
  c16 src[] = {0x00E9, 0};
  c8 dst[16];
  sz written = utf16_to_utf8(src, 1, dst, sizeof(dst));
  EXPECT_EQ(2U, written);
  EXPECT_EQ(0xC3, (u8)dst[0]);
  EXPECT_EQ(0xA9, (u8)dst[1]);
}

TEST(strings_unicode_test, utf16_to_utf8_surrogate) {
  c16 src[] = {0xD83D, 0xDE00, 0};
  c8 dst[16];
  sz written = utf16_to_utf8(src, 2, dst, sizeof(dst));
  EXPECT_EQ(4U, written);
}

TEST(strings_unicode_test, utf16_to_utf8_count) {
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  sz count = utf16_to_utf8(src, 5, nullptr, 0);
  EXPECT_EQ(5U, count);
}

TEST(strings_unicode_test, utf16_to_utf32) {
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c32 dst[16];
  sz written = utf16_to_utf32(src, 5, dst, sizeof(dst));
  EXPECT_EQ(5U, written);
  EXPECT_EQ('h', dst[0]);
}

TEST(strings_unicode_test, utf16_to_utf32_surrogate) {
  c16 src[] = {0xD83D, 0xDE00, 0};
  c32 dst[16];
  sz written = utf16_to_utf32(src, 2, dst, sizeof(dst));
  EXPECT_EQ(1U, written);
  EXPECT_EQ(0x1F600, dst[0]);
}

TEST(strings_unicode_test, utf16_to_utf32_count) {
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  sz count = utf16_to_utf32(src, 5, nullptr, 0);
  EXPECT_EQ(5U, count);
}

TEST(strings_unicode_test, utf32_to_utf8) {
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c8 dst[16];
  sz written = utf32_to_utf8(src, 5, dst, sizeof(dst));
  EXPECT_EQ(5U, written);
  EXPECT_STREQ("hello", dst);
}

TEST(strings_unicode_test, utf32_to_utf8_codepoint) {
  c32 src[] = {0x00E9, 0};
  c8 dst[16];
  sz written = utf32_to_utf8(src, 1, dst, sizeof(dst));
  EXPECT_EQ(2U, written);
  EXPECT_EQ(0xC3, (u8)dst[0]);
  EXPECT_EQ(0xA9, (u8)dst[1]);
}

TEST(strings_unicode_test, utf32_to_utf8_invalid) {
  c32 src[] = {0xD800, 0};
  c8 dst[16];
  sz written = utf32_to_utf8(src, 1, dst, sizeof(dst));
  EXPECT_EQ(3U, written);
}

TEST(strings_unicode_test, utf32_to_utf8_count) {
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  sz count = utf32_to_utf8(src, 5, nullptr, 0);
  EXPECT_EQ(5U, count);
}

TEST(strings_unicode_test, utf32_to_utf16) {
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c16 dst[16];
  sz written = utf32_to_utf16(src, 5, dst, sizeof(dst));
  EXPECT_EQ(5U, written);
  EXPECT_EQ('h', dst[0]);
}

TEST(strings_unicode_test, utf32_to_utf16_codepoint) {
  c32 src[] = {0x1F600, 0};
  c16 dst[16];
  sz written = utf32_to_utf16(src, 1, dst, sizeof(dst));
  EXPECT_EQ(2U, written);
  EXPECT_EQ(0xD83D, dst[0]);
  EXPECT_EQ(0xDE00, dst[1]);
}

TEST(strings_unicode_test, utf32_to_utf16_invalid) {
  c32 src[] = {0xD800, 0};
  c16 dst[16];
  sz written = utf32_to_utf16(src, 1, dst, sizeof(dst));
  EXPECT_EQ(1U, written);
  EXPECT_EQ(UNICODE_REPLACEMENT_CHAR, dst[0]);
}

TEST(strings_unicode_test, utf32_to_utf16_count) {
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  sz count = utf32_to_utf16(src, 5, nullptr, 0);
  EXPECT_EQ(5U, count);
}
