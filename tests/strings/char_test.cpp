// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(strings_char_test, c8_is_alnum) {
  EXPECT_TRUE(c8_is_alnum('a') != 0);
  EXPECT_TRUE(c8_is_alnum('Z') != 0);
  EXPECT_TRUE(c8_is_alnum('0') != 0);
  EXPECT_TRUE(c8_is_alnum('9') != 0);
  EXPECT_FALSE(c8_is_alnum(' ') != 0);
  EXPECT_FALSE(c8_is_alnum('!') != 0);
}

TEST(strings_char_test, c8_is_alpha) {
  EXPECT_TRUE(c8_is_alpha('a') != 0);
  EXPECT_TRUE(c8_is_alpha('Z') != 0);
  EXPECT_FALSE(c8_is_alpha('0') != 0);
  EXPECT_FALSE(c8_is_alpha(' ') != 0);
}

TEST(strings_char_test, c8_is_blank) {
  EXPECT_TRUE(c8_is_blank(' ') != 0);
  EXPECT_TRUE(c8_is_blank('\t') != 0);
  EXPECT_FALSE(c8_is_blank('a') != 0);
  EXPECT_FALSE(c8_is_blank('\n') != 0);
}

TEST(strings_char_test, c8_is_cntrl) {
  EXPECT_TRUE(c8_is_cntrl('\0') != 0);
  EXPECT_TRUE(c8_is_cntrl('\n') != 0);
  EXPECT_TRUE(c8_is_cntrl('\t') != 0);
  EXPECT_FALSE(c8_is_cntrl('a') != 0);
  EXPECT_FALSE(c8_is_cntrl(' ') != 0);
}

TEST(strings_char_test, c8_is_digit) {
  EXPECT_TRUE(c8_is_digit('0') != 0);
  EXPECT_TRUE(c8_is_digit('9') != 0);
  EXPECT_FALSE(c8_is_digit('a') != 0);
  EXPECT_FALSE(c8_is_digit(' ') != 0);
}

TEST(strings_char_test, c8_is_graph) {
  EXPECT_TRUE(c8_is_graph('a') != 0);
  EXPECT_TRUE(c8_is_graph('!') != 0);
  EXPECT_FALSE(c8_is_graph(' ') != 0);
  EXPECT_FALSE(c8_is_graph('\n') != 0);
}

TEST(strings_char_test, c8_is_lower) {
  EXPECT_TRUE(c8_is_lower('a') != 0);
  EXPECT_TRUE(c8_is_lower('z') != 0);
  EXPECT_FALSE(c8_is_lower('A') != 0);
  EXPECT_FALSE(c8_is_lower('0') != 0);
}

TEST(strings_char_test, c8_is_print) {
  EXPECT_TRUE(c8_is_print('a') != 0);
  EXPECT_TRUE(c8_is_print(' ') != 0);
  EXPECT_FALSE(c8_is_print('\n') != 0);
  EXPECT_FALSE(c8_is_print('\0') != 0);
}

TEST(strings_char_test, c8_is_punct) {
  EXPECT_TRUE(c8_is_punct('!') != 0);
  EXPECT_TRUE(c8_is_punct('.') != 0);
  EXPECT_TRUE(c8_is_punct('@') != 0);
  EXPECT_FALSE(c8_is_punct('a') != 0);
  EXPECT_FALSE(c8_is_punct(' ') != 0);
}

TEST(strings_char_test, c8_is_space) {
  EXPECT_TRUE(c8_is_space(' ') != 0);
  EXPECT_TRUE(c8_is_space('\t') != 0);
  EXPECT_TRUE(c8_is_space('\n') != 0);
  EXPECT_TRUE(c8_is_space('\r') != 0);
  EXPECT_FALSE(c8_is_space('a') != 0);
}

TEST(strings_char_test, c8_is_upper) {
  EXPECT_TRUE(c8_is_upper('A') != 0);
  EXPECT_TRUE(c8_is_upper('Z') != 0);
  EXPECT_FALSE(c8_is_upper('a') != 0);
  EXPECT_FALSE(c8_is_upper('0') != 0);
}

TEST(strings_char_test, c8_is_xdigit) {
  EXPECT_TRUE(c8_is_xdigit('0') != 0);
  EXPECT_TRUE(c8_is_xdigit('9') != 0);
  EXPECT_TRUE(c8_is_xdigit('a') != 0);
  EXPECT_TRUE(c8_is_xdigit('f') != 0);
  EXPECT_TRUE(c8_is_xdigit('A') != 0);
  EXPECT_TRUE(c8_is_xdigit('F') != 0);
  EXPECT_FALSE(c8_is_xdigit('g') != 0);
  EXPECT_FALSE(c8_is_xdigit(' ') != 0);
}

TEST(strings_char_test, c8_to_lower) {
  EXPECT_EQ('a', c8_to_lower('A'));
  EXPECT_EQ('z', c8_to_lower('Z'));
  EXPECT_EQ('0', c8_to_lower('0'));
  EXPECT_EQ('a', c8_to_lower('a'));
}

TEST(strings_char_test, c8_to_upper) {
  EXPECT_EQ('A', c8_to_upper('a'));
  EXPECT_EQ('Z', c8_to_upper('z'));
  EXPECT_EQ('0', c8_to_upper('0'));
  EXPECT_EQ('A', c8_to_upper('A'));
}

TEST(strings_char_test, c8_hex_to_nibble) {
  EXPECT_EQ(0, c8_hex_to_nibble('0'));
  EXPECT_EQ(9, c8_hex_to_nibble('9'));
  EXPECT_EQ(10, c8_hex_to_nibble('a'));
  EXPECT_EQ(15, c8_hex_to_nibble('f'));
  EXPECT_EQ(10, c8_hex_to_nibble('A'));
  EXPECT_EQ(15, c8_hex_to_nibble('F'));
  EXPECT_EQ(-1, c8_hex_to_nibble('g'));
  EXPECT_EQ(-1, c8_hex_to_nibble(' '));
}

TEST(strings_char_test, c8_nibble_to_hex) {
  EXPECT_EQ('0', c8_nibble_to_hex(0));
  EXPECT_EQ('9', c8_nibble_to_hex(9));
  EXPECT_EQ('a', c8_nibble_to_hex(10));
  EXPECT_EQ('f', c8_nibble_to_hex(15));
}

TEST(strings_char_test, c16_classification) {
  EXPECT_TRUE(c16_is_alpha('a'));
  EXPECT_TRUE(c16_is_digit('5'));
  EXPECT_TRUE(c16_is_upper('Z'));
  EXPECT_TRUE(c16_is_lower('m'));
  EXPECT_FALSE(c16_is_alpha('5'));
}

TEST(strings_char_test, c16_case_conversion) {
  EXPECT_EQ('A', c16_to_upper('a'));
  EXPECT_EQ('Z', c16_to_upper('z'));
  EXPECT_EQ('a', c16_to_lower('A'));
  EXPECT_EQ('z', c16_to_lower('Z'));
}

TEST(strings_char_test, c16_hex) {
  EXPECT_EQ(0, c16_hex_to_nibble('0'));
  EXPECT_EQ(15, c16_hex_to_nibble('f'));
  EXPECT_EQ('f', c16_nibble_to_hex(15));
}

TEST(strings_char_test, c32_classification) {
  EXPECT_TRUE(c32_is_alpha('a'));
  EXPECT_TRUE(c32_is_digit('5'));
  EXPECT_TRUE(c32_is_upper('Z'));
  EXPECT_TRUE(c32_is_lower('m'));
  EXPECT_FALSE(c32_is_alpha('5'));
}

TEST(strings_char_test, c32_case_conversion) {
  EXPECT_EQ('A', c32_to_upper('a'));
  EXPECT_EQ('Z', c32_to_upper('z'));
  EXPECT_EQ('a', c32_to_lower('A'));
  EXPECT_EQ('z', c32_to_lower('Z'));
}

TEST(strings_char_test, c32_hex) {
  EXPECT_EQ(0, c32_hex_to_nibble('0'));
  EXPECT_EQ(15, c32_hex_to_nibble('f'));
  EXPECT_EQ('f', c32_nibble_to_hex(15));
}
