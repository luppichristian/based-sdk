// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(strings_cstrings_test, cstr8_len) {
  EXPECT_EQ(5U, cstr8_len("hello"));
  EXPECT_EQ(0U, cstr8_len(""));
}

TEST(strings_cstrings_test, cstr8_is_empty) {
  EXPECT_TRUE(cstr8_is_empty("") != 0);
  EXPECT_FALSE(cstr8_is_empty("a") != 0);
}

TEST(strings_cstrings_test, cstr8_clear) {
  c8 buf[16] = "hello";
  cstr8_clear(buf);
  EXPECT_STREQ("", buf);
}

TEST(strings_cstrings_test, cstr8_cmp) {
  EXPECT_TRUE(cstr8_cmp("hello", "hello"));
  EXPECT_FALSE(cstr8_cmp("abc", "abd"));
  EXPECT_FALSE(cstr8_cmp("abd", "abc"));
}

TEST(strings_cstrings_test, cstr8_cmp_n) {
  EXPECT_TRUE(cstr8_cmp_n("hello", "helium", 3));
  EXPECT_FALSE(cstr8_cmp_n("abc", "abd", 10));
}

TEST(strings_cstrings_test, cstr8_cmp_nocase) {
  EXPECT_TRUE(cstr8_cmp_nocase("Hello", "HELLO"));
  EXPECT_FALSE(cstr8_cmp_nocase("abc", "abd"));
  EXPECT_FALSE(cstr8_cmp_nocase("abd", "abc"));
}

TEST(strings_cstrings_test, cstr8_to_u64) {
  u64 value = 0;
  EXPECT_TRUE(cstr8_to_u64("42", 100, &value));
  EXPECT_EQ(42U, value);
  EXPECT_FALSE(cstr8_to_u64("101", 100, &value));
  EXPECT_FALSE(cstr8_to_u64("-1", U64_MAX, &value));
  EXPECT_FALSE(cstr8_to_u64("", U64_MAX, &value));
}

TEST(strings_cstrings_test, cstr8_copy) {
  c8 buf[16];
  sz len = cstr8_copy(buf, sizeof(buf), "hello");
  EXPECT_EQ(5U, len);
  EXPECT_STREQ("hello", buf);
}

TEST(strings_cstrings_test, cstr8_copy_n) {
  c8 buf[16];
  sz len = cstr8_copy_n(buf, sizeof(buf), "hello", 3);
  EXPECT_EQ(3U, len);
  EXPECT_STREQ("hel", buf);
}

TEST(strings_cstrings_test, cstr8_cat) {
  c8 buf[16] = "hello";
  sz len = cstr8_cat(buf, sizeof(buf), " world");
  EXPECT_EQ(11U, len);
  EXPECT_STREQ("hello world", buf);
}

TEST(strings_cstrings_test, cstr8_append_char) {
  c8 buf[16] = "hello";
  sz len = cstr8_append_char(buf, sizeof(buf), '!');
  EXPECT_EQ(6U, len);
  EXPECT_STREQ("hello!", buf);
}

TEST(strings_cstrings_test, cstr8_truncate) {
  c8 buf[16] = "hello";
  cstr8_truncate(buf, 3);
  EXPECT_STREQ("hel", buf);
}

TEST(strings_cstrings_test, cstr8_format) {
  c8 buf[32];
  b32 success = cstr8_format(buf, sizeof(buf), "hello %s %d", "world", 42);
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("hello world 42", buf);
}

TEST(strings_cstrings_test, cstr8_find) {
  const c8* result = cstr8_find("hello world", "world");
  EXPECT_NE(nullptr, result);
  EXPECT_STREQ("world", result);

  result = cstr8_find("hello world", "xyz");
  EXPECT_EQ(nullptr, result);
}

TEST(strings_cstrings_test, cstr8_find_last) {
  const c8* result = cstr8_find_last("hello world world", "world");
  EXPECT_NE(nullptr, result);
  EXPECT_STREQ("world", result);
}

TEST(strings_cstrings_test, cstr8_find_char) {
  const c8* result = cstr8_find_char("hello", 'l');
  EXPECT_NE(nullptr, result);
  EXPECT_EQ('l', *result);

  result = cstr8_find_char("hello", 'x');
  EXPECT_EQ(nullptr, result);
}

TEST(strings_cstrings_test, cstr8_find_last_char) {
  const c8* result = cstr8_find_last_char("hello", 'l');
  EXPECT_NE(nullptr, result);
  EXPECT_EQ('l', *result);
}

TEST(strings_cstrings_test, cstr8_count_char) {
  EXPECT_EQ(2U, cstr8_count_char("hello", 'l'));
  EXPECT_EQ(0U, cstr8_count_char("hello", 'x'));
}

TEST(strings_cstrings_test, cstr8_starts_with) {
  EXPECT_TRUE(cstr8_starts_with("hello", "hel") != 0);
  EXPECT_FALSE(cstr8_starts_with("hello", "xyz") != 0);
}

TEST(strings_cstrings_test, cstr8_ends_with) {
  EXPECT_TRUE(cstr8_ends_with("hello", "llo") != 0);
  EXPECT_FALSE(cstr8_ends_with("hello", "xyz") != 0);
}

TEST(strings_cstrings_test, cstr8_to_upper) {
  c8 buf[] = "hello";
  cstr8_to_upper(buf);
  EXPECT_STREQ("HELLO", buf);
}

TEST(strings_cstrings_test, cstr8_to_lower) {
  c8 buf[] = "HELLO";
  cstr8_to_lower(buf);
  EXPECT_STREQ("hello", buf);
}

TEST(strings_cstrings_test, cstr8_trim) {
  c8 buf[] = "  hello  ";
  cstr8_trim(buf);
  EXPECT_STREQ("hello", buf);
}

TEST(strings_cstrings_test, cstr8_replace_char) {
  c8 buf[] = "hello";
  cstr8_replace_char(buf, 'l', 'x');
  EXPECT_STREQ("hexxo", buf);
}

TEST(strings_cstrings_test, cstr8_remove_char) {
  c8 buf[] = "hello";
  sz removed = cstr8_remove_char(buf, 'l');
  EXPECT_EQ(2U, removed);
  EXPECT_STREQ("heo", buf);
}

TEST(strings_cstrings_test, cstr8_remove_whitespace) {
  c8 buf[] = "h e l l o";
  sz removed = cstr8_remove_whitespace(buf);
  EXPECT_EQ(4U, removed);
  EXPECT_STREQ("hello", buf);
}

TEST(strings_cstrings_test, cstr8_remove_prefix) {
  c8 buf[] = "helloworld";
  b32 success = cstr8_remove_prefix(buf, "hello");
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("world", buf);

  c8 buf2[] = "helloworld";
  success = cstr8_remove_prefix(buf2, "xyz");
  EXPECT_FALSE(success != 0);
}

TEST(strings_cstrings_test, cstr8_remove_suffix) {
  c8 buf[] = "helloworld";
  b32 success = cstr8_remove_suffix(buf, "world");
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("hello", buf);

  c8 buf2[] = "helloworld";
  success = cstr8_remove_suffix(buf2, "xyz");
  EXPECT_FALSE(success != 0);
}

TEST(strings_cstrings_test, cstr8_replace) {
  c8 buf[32] = "hello world";
  sz count = cstr8_replace(buf, sizeof(buf), "world", "there");
  EXPECT_EQ(1U, count);
  EXPECT_STREQ("hello there", buf);
}

TEST(strings_cstrings_test, cstr8_common_prefix) {
  c8 buf[16];
  sz len = cstr8_common_prefix("hello", "helium", buf, sizeof(buf));
  EXPECT_EQ(3U, len);
  EXPECT_STREQ("hel", buf);

  len = cstr8_common_prefix("abc", "xyz", buf, sizeof(buf));
  EXPECT_EQ(0U, len);
  EXPECT_STREQ("", buf);
}

TEST(strings_cstrings_test, cstr8_beautify) {
  c8 buf[] = "hello world";
  cstr8_beautify(buf);
  EXPECT_STREQ("Hello world", buf);
}

TEST(strings_cstrings_test, cstr8_to_i64) {
  i64 val;
  b32 success = cstr8_to_i64("12345", &val);
  EXPECT_TRUE(success != 0);
  EXPECT_EQ(12345, val);

  success = cstr8_to_i64("hello", &val);
  EXPECT_FALSE(success != 0);
}

TEST(strings_cstrings_test, cstr8_to_f64) {
  f64 val;
  b32 success = cstr8_to_f64("3.14", &val);
  EXPECT_TRUE(success != 0);
  EXPECT_DOUBLE_EQ(3.14, val);

  success = cstr8_to_f64("hello", &val);
  EXPECT_FALSE(success != 0);
}

TEST(strings_cstrings_test, cstr8_tokenizer) {
  cstr8_tokenizer tok = cstr8_tokenizer_make("a,b,c", ",");
  c8 buf[16];

  EXPECT_TRUE(cstr8_tokenizer_next(&tok, buf, sizeof(buf)) != 0);
  EXPECT_STREQ("a", buf);

  EXPECT_TRUE(cstr8_tokenizer_next(&tok, buf, sizeof(buf)) != 0);
  EXPECT_STREQ("b", buf);

  EXPECT_TRUE(cstr8_tokenizer_next(&tok, buf, sizeof(buf)) != 0);
  EXPECT_STREQ("c", buf);

  EXPECT_TRUE(cstr8_tokenizer_next(&tok, buf, sizeof(buf)) != 0);
  EXPECT_STREQ("", buf);

  EXPECT_FALSE(cstr8_tokenizer_next(&tok, buf, sizeof(buf)) != 0);
}

TEST(strings_cstrings_test, cstr8_join) {
  c8 buf[64];
  c8 const* parts[] = {"a", "b", "c"};
  sz len = cstr8_join(buf, sizeof(buf), parts, 3, ",");
  EXPECT_STREQ("a,b,c", buf);
}

TEST(strings_cstrings_test, cstr8_to_cstr16) {
  c8 src[] = "hello";
  c16 dst[16];
  sz len = cstr8_to_cstr16(src, dst, sizeof(dst));
  EXPECT_EQ(5U, len);
}

TEST(strings_cstrings_test, cstr8_to_cstr32) {
  c8 src[] = "hello";
  c32 dst[16];
  sz len = cstr8_to_cstr32(src, dst, sizeof(dst));
  EXPECT_EQ(5U, len);
}

TEST(strings_cstrings_test, cstr16_to_cstr8) {
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c8 dst[16];
  sz len = cstr16_to_cstr8(src, dst, sizeof(dst));
  EXPECT_EQ(5U, len);
  EXPECT_STREQ("hello", dst);
}

TEST(strings_cstrings_test, cstr16_len) {
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  EXPECT_EQ(5U, cstr16_len(src));
  EXPECT_EQ(0U, cstr16_len((c16*)L""));
}

TEST(strings_cstrings_test, cstr16_is_empty) {
  c16 empty[] = {0};
  c16 non_empty[] = {'a', 0};
  EXPECT_TRUE(cstr16_is_empty(empty) != 0);
  EXPECT_FALSE(cstr16_is_empty(non_empty) != 0);
}

TEST(strings_cstrings_test, cstr16_cmp) {
  c16 lhs[] = {'h', 'e', 'l', 'l', 'o', 0};
  c16 rhs[] = {'h', 'e', 'l', 'l', 'o', 0};
  EXPECT_TRUE(cstr16_cmp(lhs, rhs));
}

TEST(strings_cstrings_test, cstr16_find) {
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c16 sub[] = {'l', 'l', 0};
  const c16* result = cstr16_find(src, sub);
  EXPECT_NE(nullptr, result);
}

TEST(strings_cstrings_test, cstr16_starts_with) {
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c16 prefix[] = {'h', 'e', 'l', 0};
  EXPECT_TRUE(cstr16_starts_with(src, prefix) != 0);
}

TEST(strings_cstrings_test, cstr16_ends_with) {
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c16 suffix[] = {'l', 'o', 0};
  EXPECT_TRUE(cstr16_ends_with(src, suffix) != 0);
}

TEST(strings_cstrings_test, cstr16_to_upper) {
  c16 buf[] = {'h', 'e', 'l', 'l', 'o', 0};
  cstr16_to_upper(buf);
  EXPECT_EQ('H', buf[0]);
  EXPECT_EQ('E', buf[1]);
}

TEST(strings_cstrings_test, cstr16_to_lower) {
  c16 buf[] = {'H', 'E', 'L', 'L', 'O', 0};
  cstr16_to_lower(buf);
  EXPECT_EQ('h', buf[0]);
  EXPECT_EQ('e', buf[1]);
}

TEST(strings_cstrings_test, cstr16_trim) {
  c16 buf[] = {' ', 'h', 'e', 'l', 'l', 'o', ' ', 0};
  cstr16_trim(buf);
  EXPECT_EQ('h', buf[0]);
}

TEST(strings_cstrings_test, cstr32_len) {
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  EXPECT_EQ(5U, cstr32_len(src));
  EXPECT_EQ(0U, cstr32_len((c32*)U""));
}

TEST(strings_cstrings_test, cstr32_is_empty) {
  c32 empty[] = {0};
  c32 non_empty[] = {'a', 0};
  EXPECT_TRUE(cstr32_is_empty(empty) != 0);
  EXPECT_FALSE(cstr32_is_empty(non_empty) != 0);
}

TEST(strings_cstrings_test, cstr32_cmp) {
  c32 lhs[] = {'h', 'e', 'l', 'l', 'o', 0};
  c32 rhs[] = {'h', 'e', 'l', 'l', 'o', 0};
  EXPECT_TRUE(cstr32_cmp(lhs, rhs));
}

TEST(strings_cstrings_test, cstr32_find) {
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c32 sub[] = {'l', 'l', 0};
  const c32* result = cstr32_find(src, sub);
  EXPECT_NE(nullptr, result);
}

TEST(strings_cstrings_test, cstr32_starts_with) {
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c32 prefix[] = {'h', 'e', 'l', 0};
  EXPECT_TRUE(cstr32_starts_with(src, prefix) != 0);
}

TEST(strings_cstrings_test, cstr32_ends_with) {
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c32 suffix[] = {'l', 'o', 0};
  EXPECT_TRUE(cstr32_ends_with(src, suffix) != 0);
}

TEST(strings_cstrings_test, cstr32_to_upper) {
  c32 buf[] = {'h', 'e', 'l', 'l', 'o', 0};
  cstr32_to_upper(buf);
  EXPECT_EQ('H', buf[0]);
  EXPECT_EQ('E', buf[1]);
}

TEST(strings_cstrings_test, cstr32_to_lower) {
  c32 buf[] = {'H', 'E', 'L', 'L', 'O', 0};
  cstr32_to_lower(buf);
  EXPECT_EQ('h', buf[0]);
  EXPECT_EQ('e', buf[1]);
}
