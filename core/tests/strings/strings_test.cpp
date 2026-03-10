// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(strings_strings_test, str8_make) {
  c8 buf[32] = "hello";
  str8 str = str8_make(buf, sizeof(buf));
  EXPECT_EQ(5U, str.size);
  EXPECT_EQ(32U, str.cap);
  EXPECT_EQ(buf, str.ptr);
}

TEST(strings_strings_test, str8_empty) {
  c8 buf[32];
  str8 str = str8_empty(buf, sizeof(buf));
  EXPECT_EQ(0U, str.size);
  EXPECT_EQ(32U, str.cap);
}

TEST(strings_strings_test, str8_from_cstr) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");
  EXPECT_EQ(5U, str.size);
}

TEST(strings_strings_test, str8_is_empty) {
  c8 buf[32];
  str8 empty = str8_empty(buf, sizeof(buf));
  EXPECT_TRUE(str8_is_empty(empty) != 0);

  str8 non_empty = str8_from_cstr(buf, sizeof(buf), "hello");
  EXPECT_FALSE(str8_is_empty(non_empty) != 0);
}

TEST(strings_strings_test, str8_cmp) {
  c8 buf1[32];
  c8 buf2[32];
  c8 buf3[32];
  str8 str_a = str8_from_cstr(buf1, sizeof(buf1), "hello");
  str8 str_b = str8_from_cstr(buf2, sizeof(buf2), "hello");
  str8 str_c = str8_from_cstr(buf3, sizeof(buf3), "world");

  EXPECT_TRUE(str8_cmp(str_a, str_b));
  EXPECT_FALSE(str8_cmp(str_a, str_c));
  EXPECT_FALSE(str8_cmp(str_c, str_a));
}

TEST(strings_strings_test, str8_cmp_nocase) {
  c8 buf1[32];
  c8 buf2[32];
  str8 str_a = str8_from_cstr(buf1, sizeof(buf1), "Hello");
  str8 str_b = str8_from_cstr(buf2, sizeof(buf2), "HELLO");

  EXPECT_TRUE(str8_cmp_nocase(str_a, str_b));
}

TEST(strings_strings_test, str8_to_u64) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "42");
  u64 value = 0;

  EXPECT_TRUE(str8_to_u64(str, 100, &value));
  EXPECT_EQ(42U, value);

  str = str8_from_cstr(buf, sizeof(buf), "200");
  EXPECT_FALSE(str8_to_u64(str, 100, &value));
}

TEST(strings_strings_test, str8_find) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello world");

  const c8* result = str8_find(str, "world");
  EXPECT_NE(nullptr, result);

  result = str8_find(str, "xyz");
  EXPECT_EQ(nullptr, result);
}

TEST(strings_strings_test, str8_find_char) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");

  const c8* result = str8_find_char(str, 'l');
  EXPECT_NE(nullptr, result);

  result = str8_find_char(str, 'x');
  EXPECT_EQ(nullptr, result);
}

TEST(strings_strings_test, str8_find_last) {
  c8 buf[64];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello world world");

  const c8* result = str8_find_last(str, "world");
  EXPECT_NE(nullptr, result);
}

TEST(strings_strings_test, str8_find_last_char) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");

  const c8* result = str8_find_last_char(str, 'l');
  EXPECT_NE(nullptr, result);
}

TEST(strings_strings_test, str8_count_char) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");

  EXPECT_EQ(2U, str8_count_char(str, 'l'));
  EXPECT_EQ(0U, str8_count_char(str, 'x'));
}

TEST(strings_strings_test, str8_starts_with) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");

  EXPECT_TRUE(str8_starts_with(str, "hel") != 0);
  EXPECT_FALSE(str8_starts_with(str, "xyz") != 0);
}

TEST(strings_strings_test, str8_ends_with) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");

  EXPECT_TRUE(str8_ends_with(str, "llo") != 0);
  EXPECT_FALSE(str8_ends_with(str, "xyz") != 0);
}

TEST(strings_strings_test, str8_to_i64) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "12345");

  i64 val;
  b32 success = str8_to_i64(str, &val);
  EXPECT_TRUE(success != 0);
  EXPECT_EQ(12345, val);

  str8 bad = str8_from_cstr(buf, sizeof(buf), "hello");
  success = str8_to_i64(bad, &val);
  EXPECT_FALSE(success != 0);
}

TEST(strings_strings_test, str8_to_f64) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "3.14");

  f64 val;
  b32 success = str8_to_f64(str, &val);
  EXPECT_TRUE(success != 0);
  EXPECT_DOUBLE_EQ(3.14, val);

  str8 bad = str8_from_cstr(buf, sizeof(buf), "hello");
  success = str8_to_f64(bad, &val);
  EXPECT_FALSE(success != 0);
}

TEST(strings_strings_test, str8_clear) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");
  str8_clear(&str);
  EXPECT_EQ(0U, str.size);
}

TEST(strings_strings_test, str8_copy) {
  c8 buf[32];
  str8 str = str8_empty(buf, sizeof(buf));

  sz written = str8_copy(&str, "hello world");
  EXPECT_EQ(11U, written);
  EXPECT_EQ(11U, str.size);
}

TEST(strings_strings_test, str8_cat) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");

  sz written = str8_cat(&str, " world");
  EXPECT_EQ(11U, written);
  EXPECT_EQ(11U, str.size);
}

TEST(strings_strings_test, str8_append_char) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");

  sz written = str8_append_char(&str, '!');
  EXPECT_EQ(6U, written);
  EXPECT_EQ(6U, str.size);
}

TEST(strings_strings_test, str8_truncate) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");

  str8_truncate(&str, 3);
  EXPECT_EQ(3U, str.size);
}

TEST(strings_strings_test, str8_format) {
  c8 buf[32];
  str8 str = str8_empty(buf, sizeof(buf));

  b32 success = str8_format(&str, "hello %s %d", "world", 42);
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("hello world 42", str.ptr);
}

TEST(strings_strings_test, str8_to_upper) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");
  str8_to_upper(&str);
  EXPECT_STREQ("HELLO", str.ptr);
}

TEST(strings_strings_test, str8_to_lower) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "HELLO");
  str8_to_lower(&str);
  EXPECT_STREQ("hello", str.ptr);
}

TEST(strings_strings_test, str8_trim) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "  hello  ");
  str8_trim(&str);
  EXPECT_STREQ("hello", str.ptr);
}

TEST(strings_strings_test, str8_replace_char) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");
  str8_replace_char(&str, 'l', 'x');
  EXPECT_STREQ("hexxo", str.ptr);
}

TEST(strings_strings_test, str8_remove_char) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello");
  sz removed = str8_remove_char(&str, 'l');
  EXPECT_EQ(2U, removed);
  EXPECT_STREQ("heo", str.ptr);
}

TEST(strings_strings_test, str8_remove_whitespace) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "h e l l o");
  sz removed = str8_remove_whitespace(&str);
  EXPECT_EQ(4U, removed);
  EXPECT_STREQ("hello", str.ptr);
}

TEST(strings_strings_test, str8_remove_prefix) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "helloworld");
  b32 success = str8_remove_prefix(&str, "hello");
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("world", str.ptr);

  str8 bad = str8_from_cstr(buf, sizeof(buf), "helloworld");
  success = str8_remove_prefix(&bad, "xyz");
  EXPECT_FALSE(success != 0);
}

TEST(strings_strings_test, str8_remove_suffix) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "helloworld");
  b32 success = str8_remove_suffix(&str, "world");
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("hello", str.ptr);

  str8 bad = str8_from_cstr(buf, sizeof(buf), "helloworld");
  success = str8_remove_suffix(&bad, "xyz");
  EXPECT_FALSE(success != 0);
}

TEST(strings_strings_test, str8_replace) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello world");
  sz count = str8_replace(&str, "world", "there");
  EXPECT_EQ(1U, count);
  EXPECT_STREQ("hello there", str.ptr);
}

TEST(strings_strings_test, str8_beautify) {
  c8 buf[32];
  str8 str = str8_from_cstr(buf, sizeof(buf), "hello world");
  str8_beautify(&str);
  EXPECT_STREQ("Hello world", str.ptr);
}

TEST(strings_strings_test, str8_split_next) {
  c8 buf[64];
  str8 src = str8_from_cstr(buf, sizeof(buf), "a,b,c");
  cstr8_tokenizer tok = cstr8_tokenizer_make(src.ptr, ",");
  c8 token_buf[16] = {0};
  str8 token = str8_empty(token_buf, sizeof(token_buf));

  EXPECT_TRUE(str8_split_next(&tok, &token) != 0);
  EXPECT_STREQ("a", token.ptr);

  EXPECT_TRUE(str8_split_next(&tok, &token) != 0);
  EXPECT_STREQ("b", token.ptr);

  EXPECT_TRUE(str8_split_next(&tok, &token) != 0);
  EXPECT_STREQ("c", token.ptr);

  EXPECT_TRUE(str8_split_next(&tok, &token) != 0);
  EXPECT_STREQ("", token.ptr);

  EXPECT_FALSE(str8_split_next(&tok, &token) != 0);
}

TEST(strings_strings_test, str8_join) {
  c8 buf[64];
  str8 dst = str8_empty(buf, sizeof(buf));
  c8 const* parts[] = {"a", "b", "c"};

  b32 success = str8_join(&dst, parts, 3, ",");
  EXPECT_TRUE(success != 0);
  EXPECT_STREQ("a,b,c", dst.ptr);
}

TEST(strings_strings_test, str8_to_str16) {
  c8 src_buf[32];
  c16 dst_buf[32];
  str8 src = str8_from_cstr(src_buf, sizeof(src_buf), "hello");
  str16 dst = str16_empty(dst_buf, sizeof(dst_buf));

  sz written = str8_to_str16(src, &dst);
  EXPECT_EQ(5U, written);
}

TEST(strings_strings_test, str8_to_str32) {
  c8 src_buf[32];
  c32 dst_buf[32];
  str8 src = str8_from_cstr(src_buf, sizeof(src_buf), "hello");
  str32 dst = str32_empty(dst_buf, sizeof(dst_buf));

  sz written = str8_to_str32(src, &dst);
  EXPECT_EQ(5U, written);
}

TEST(strings_strings_test, str16_make) {
  c16 buf[32] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 str = str16_make(buf, sizeof(buf));
  EXPECT_EQ(5U, str.size);
}

TEST(strings_strings_test, str16_empty) {
  c16 buf[32];
  str16 str = str16_empty(buf, sizeof(buf));
  EXPECT_EQ(0U, str.size);
}

TEST(strings_strings_test, str16_from_cstr) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  EXPECT_EQ(5U, str.size);
}

TEST(strings_strings_test, str16_is_empty) {
  c16 buf[32];
  str16 empty = str16_empty(buf, sizeof(buf));
  EXPECT_TRUE(str16_is_empty(empty) != 0);

  c16 buf2[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 non_empty = str16_from_cstr(buf2, sizeof(buf2), src);
  EXPECT_FALSE(str16_is_empty(non_empty) != 0);
}

TEST(strings_strings_test, str16_cmp) {
  c16 buf1[32];
  c16 buf2[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 str_a = str16_from_cstr(buf1, sizeof(buf1), src);
  str16 str_b = str16_from_cstr(buf2, sizeof(buf2), src);

  EXPECT_TRUE(str16_cmp(str_a, str_b));
}

TEST(strings_strings_test, str16_find) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c16 sub[] = {'l', 'l', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);

  const c16* result = str16_find(str, sub);
  EXPECT_NE(nullptr, result);
}

TEST(strings_strings_test, str16_starts_with) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c16 prefix[] = {'h', 'e', 'l', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);

  EXPECT_TRUE(str16_starts_with(str, prefix) != 0);
}

TEST(strings_strings_test, str16_ends_with) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c16 suffix[] = {'l', 'o', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);

  EXPECT_TRUE(str16_ends_with(str, suffix) != 0);
}

TEST(strings_strings_test, str16_to_upper) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  str16_to_upper(&str);
  EXPECT_EQ('H', str.ptr[0]);
}

TEST(strings_strings_test, str16_to_lower) {
  c16 buf[32];
  c16 src[] = {'H', 'E', 'L', 'L', 'O', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  str16_to_lower(&str);
  EXPECT_EQ('h', str.ptr[0]);
}

TEST(strings_strings_test, str16_trim) {
  c16 buf[32];
  c16 src[] = {' ', 'h', 'e', 'l', 'l', 'o', ' ', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  str16_trim(&str);
  EXPECT_EQ('h', str.ptr[0]);
}

TEST(strings_strings_test, str16_to_i64) {
  c16 buf[32];
  c16 src[] = {'1', '2', '3', '4', '5', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);

  i64 val;
  b32 success = str16_to_i64(str, &val);
  EXPECT_TRUE(success != 0);
  EXPECT_EQ(12345, val);
}

TEST(strings_strings_test, str16_to_f64) {
  c16 buf[32];
  c16 src[] = {'3', '.', '1', '4', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);

  f64 val;
  b32 success = str16_to_f64(str, &val);
  EXPECT_TRUE(success != 0);
  EXPECT_DOUBLE_EQ(3.14, val);
}

TEST(strings_strings_test, str16_clear) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  str16_clear(&str);
  EXPECT_EQ(0U, str.size);
}

TEST(strings_strings_test, str16_copy) {
  c16 buf[32];
  str16 str = str16_empty(buf, sizeof(buf));
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};

  sz written = str16_copy(&str, src);
  EXPECT_EQ(5U, written);
}

TEST(strings_strings_test, str16_cat) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  c16 add[] = {' ', 'w', 'o', 'r', 'l', 'd', 0};

  sz written = str16_cat(&str, add);
  EXPECT_EQ(11U, written);
}

TEST(strings_strings_test, str16_replace_char) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  str16_replace_char(&str, 'l', 'x');
  EXPECT_EQ('x', str.ptr[2]);
}

TEST(strings_strings_test, str16_remove_char) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  sz removed = str16_remove_char(&str, 'l');
  EXPECT_EQ(2U, removed);
}

TEST(strings_strings_test, str16_remove_prefix) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', 0};
  c16 prefix[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  b32 success = str16_remove_prefix(&str, prefix);
  EXPECT_TRUE(success != 0);
}

TEST(strings_strings_test, str16_remove_suffix) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', 0};
  c16 suffix[] = {'w', 'o', 'r', 'l', 'd', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  b32 success = str16_remove_suffix(&str, suffix);
  EXPECT_TRUE(success != 0);
}

TEST(strings_strings_test, str16_replace) {
  c16 buf[64];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', 0};
  c16 from[] = {'w', 'o', 'r', 'l', 'd', 0};
  c16 rep[] = {'t', 'h', 'e', 'r', 'e', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  sz count = str16_replace(&str, from, rep);
  EXPECT_EQ(1U, count);
}

TEST(strings_strings_test, str16_beautify) {
  c16 buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', 0};
  str16 str = str16_from_cstr(buf, sizeof(buf), src);
  str16_beautify(&str);
  EXPECT_EQ('H', str.ptr[0]);
}

TEST(strings_strings_test, str16_to_str8) {
  c16 src_buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 src_str = str16_from_cstr(src_buf, sizeof(src_buf), src);

  c8 dst_buf[32];
  str8 dst = str8_empty(dst_buf, sizeof(dst_buf));

  sz written = str16_to_str8(src_str, &dst);
  EXPECT_EQ(5U, written);
}

TEST(strings_strings_test, str16_to_str32) {
  c16 src_buf[32];
  c16 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str16 src_str = str16_from_cstr(src_buf, sizeof(src_buf), src);

  c32 dst_buf[32];
  str32 dst = str32_empty(dst_buf, sizeof(dst_buf));

  sz written = str16_to_str32(src_str, &dst);
  EXPECT_EQ(5U, written);
}

TEST(strings_strings_test, str32_make) {
  c32 buf[32] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 str = str32_make(buf, sizeof(buf));
  EXPECT_EQ(5U, str.size);
}

TEST(strings_strings_test, str32_empty) {
  c32 buf[32];
  str32 str = str32_empty(buf, sizeof(buf));
  EXPECT_EQ(0U, str.size);
}

TEST(strings_strings_test, str32_from_cstr) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  EXPECT_EQ(5U, str.size);
}

TEST(strings_strings_test, str32_is_empty) {
  c32 buf[32];
  str32 empty = str32_empty(buf, sizeof(buf));
  EXPECT_TRUE(str32_is_empty(empty) != 0);

  c32 buf2[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 non_empty = str32_from_cstr(buf2, sizeof(buf2), src);
  EXPECT_FALSE(str32_is_empty(non_empty) != 0);
}

TEST(strings_strings_test, str32_cmp) {
  c32 buf1[32];
  c32 buf2[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 str_a = str32_from_cstr(buf1, sizeof(buf1), src);
  str32 str_b = str32_from_cstr(buf2, sizeof(buf2), src);

  EXPECT_TRUE(str32_cmp(str_a, str_b));
}

TEST(strings_strings_test, str32_find) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c32 sub[] = {'l', 'l', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);

  const c32* result = str32_find(str, sub);
  EXPECT_NE(nullptr, result);
}

TEST(strings_strings_test, str32_starts_with) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c32 prefix[] = {'h', 'e', 'l', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);

  EXPECT_TRUE(str32_starts_with(str, prefix) != 0);
}

TEST(strings_strings_test, str32_ends_with) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  c32 suffix[] = {'l', 'o', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);

  EXPECT_TRUE(str32_ends_with(str, suffix) != 0);
}

TEST(strings_strings_test, str32_to_upper) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  str32_to_upper(&str);
  EXPECT_EQ('H', str.ptr[0]);
}

TEST(strings_strings_test, str32_to_lower) {
  c32 buf[32];
  c32 src[] = {'H', 'E', 'L', 'L', 'O', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  str32_to_lower(&str);
  EXPECT_EQ('h', str.ptr[0]);
}

TEST(strings_strings_test, str32_trim) {
  c32 buf[32];
  c32 src[] = {' ', 'h', 'e', 'l', 'l', 'o', ' ', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  str32_trim(&str);
  EXPECT_EQ('h', str.ptr[0]);
}

TEST(strings_strings_test, str32_clear) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  str32_clear(&str);
  EXPECT_EQ(0U, str.size);
}

TEST(strings_strings_test, str32_copy) {
  c32 buf[32];
  str32 str = str32_empty(buf, sizeof(buf));
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};

  sz written = str32_copy(&str, src);
  EXPECT_EQ(5U, written);
}

TEST(strings_strings_test, str32_cat) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  c32 add[] = {' ', 'w', 'o', 'r', 'l', 'd', 0};

  sz written = str32_cat(&str, add);
  EXPECT_EQ(11U, written);
}

TEST(strings_strings_test, str32_replace_char) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  str32_replace_char(&str, 'l', 'x');
  EXPECT_EQ('x', str.ptr[2]);
}

TEST(strings_strings_test, str32_remove_char) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  sz removed = str32_remove_char(&str, 'l');
  EXPECT_EQ(2U, removed);
}

TEST(strings_strings_test, str32_remove_prefix) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', 0};
  c32 prefix[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  b32 success = str32_remove_prefix(&str, prefix);
  EXPECT_TRUE(success != 0);
}

TEST(strings_strings_test, str32_remove_suffix) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', 0};
  c32 suffix[] = {'w', 'o', 'r', 'l', 'd', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  b32 success = str32_remove_suffix(&str, suffix);
  EXPECT_TRUE(success != 0);
}

TEST(strings_strings_test, str32_replace) {
  c32 buf[64];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', 0};
  c32 from[] = {'w', 'o', 'r', 'l', 'd', 0};
  c32 rep[] = {'t', 'h', 'e', 'r', 'e', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  sz count = str32_replace(&str, from, rep);
  EXPECT_EQ(1U, count);
}

TEST(strings_strings_test, str32_beautify) {
  c32 buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', 0};
  str32 str = str32_from_cstr(buf, sizeof(buf), src);
  str32_beautify(&str);
  EXPECT_EQ('H', str.ptr[0]);
}

TEST(strings_strings_test, str32_to_str8) {
  c32 src_buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 src_str = str32_from_cstr(src_buf, sizeof(src_buf), src);

  c8 dst_buf[32];
  str8 dst = str8_empty(dst_buf, sizeof(dst_buf));

  sz written = str32_to_str8(src_str, &dst);
  EXPECT_EQ(5U, written);
}

TEST(strings_strings_test, str32_to_str16) {
  c32 src_buf[32];
  c32 src[] = {'h', 'e', 'l', 'l', 'o', 0};
  str32 src_str = str32_from_cstr(src_buf, sizeof(src_buf), src);

  c16 dst_buf[32];
  str16 dst = str16_empty(dst_buf, sizeof(dst_buf));

  sz written = str32_to_str16(src_str, &dst);
  EXPECT_EQ(5U, written);
}
