// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

struct singly_node {
  struct singly_node* next;
  i32 value;
};

TEST(containers_singly_list_test, empty_macro) {
  struct singly_node* head = nullptr;
  struct singly_node* tail = nullptr;

  EXPECT_NE(0, SINGLY_LIST_EMPTY(head, tail));
}

TEST(containers_singly_list_test, push_front) {
  struct singly_node* head = nullptr;
  struct singly_node* tail = nullptr;

  struct singly_node node1 = {0};
  node1.value = 1;

  SINGLY_LIST_PUSH_FRONT(head, tail, &node1);
  EXPECT_EQ(&node1, head);
  EXPECT_EQ(&node1, tail);

  struct singly_node node2 = {0};
  node2.value = 2;

  SINGLY_LIST_PUSH_FRONT(head, tail, &node2);
  EXPECT_EQ(&node2, head);
  EXPECT_EQ(&node1, tail);
}

TEST(containers_singly_list_test, push_back) {
  struct singly_node* head = nullptr;
  struct singly_node* tail = nullptr;

  struct singly_node node1 = {0};
  node1.value = 1;

  SINGLY_LIST_PUSH_BACK(head, tail, &node1);
  EXPECT_EQ(&node1, head);
  EXPECT_EQ(&node1, tail);

  struct singly_node node2 = {0};
  node2.value = 2;

  SINGLY_LIST_PUSH_BACK(head, tail, &node2);
  EXPECT_EQ(&node1, head);
  EXPECT_EQ(&node2, tail);
}

TEST(containers_singly_list_test, pop_front) {
  struct singly_node* head = nullptr;
  struct singly_node* tail = nullptr;

  struct singly_node node1 = {0};
  struct singly_node node2 = {0};

  SINGLY_LIST_PUSH_BACK(head, tail, &node1);
  SINGLY_LIST_PUSH_BACK(head, tail, &node2);

  struct singly_node* popped = nullptr;
  SINGLY_LIST_POP_FRONT(head, tail, popped);
  EXPECT_EQ(&node1, popped);
  EXPECT_EQ(&node2, head);
  EXPECT_EQ(&node2, tail);

  SINGLY_LIST_POP_FRONT(head, tail, popped);
  EXPECT_EQ(&node2, popped);
  EXPECT_EQ(nullptr, head);
  EXPECT_EQ(nullptr, tail);
}

TEST(containers_singly_list_test, count) {
  struct singly_node* head = nullptr;
  struct singly_node* tail = nullptr;
  sz count = 0;

  SINGLY_LIST_COUNT(head, tail, count);
  EXPECT_EQ(0U, count);

  struct singly_node node1 = {0};
  struct singly_node node2 = {0};

  SINGLY_LIST_PUSH_BACK(head, tail, &node1);
  SINGLY_LIST_PUSH_BACK(head, tail, &node2);

  SINGLY_LIST_COUNT(head, tail, count);
  EXPECT_EQ(2U, count);
}

TEST(containers_singly_list_test, head_tail) {
  struct singly_node* head = nullptr;
  struct singly_node* tail = nullptr;

  struct singly_node node1 = {0};
  struct singly_node node2 = {0};

  SINGLY_LIST_PUSH_BACK(head, tail, &node1);
  SINGLY_LIST_PUSH_BACK(head, tail, &node2);

  EXPECT_EQ(&node1, SINGLY_LIST_HEAD(head, tail));
  EXPECT_EQ(&node2, SINGLY_LIST_TAIL(head, tail));
}

TEST(containers_singly_list_test, foreach) {
  struct singly_node* head = nullptr;
  struct singly_node* tail = nullptr;

  struct singly_node node1 = {0};
  struct singly_node node2 = {0};
  struct singly_node node3 = {0};

  node1.value = 1;
  node2.value = 2;
  node3.value = 3;

  SINGLY_LIST_PUSH_BACK(head, tail, &node1);
  SINGLY_LIST_PUSH_BACK(head, tail, &node2);
  SINGLY_LIST_PUSH_BACK(head, tail, &node3);

  i32 sum = 0;
  SINGLY_LIST_FOREACH(head, tail, it) {
    sum += it->value;
  }

  EXPECT_EQ(6, sum);
}
