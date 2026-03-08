// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

struct stack_node {
  struct stack_node* next;
  i32 value;
};

TEST(containers_stack_list_test, empty_macro) {
  struct stack_node* head = nullptr;

  EXPECT_NE(0, STACK_LIST_EMPTY(head));
}

TEST(containers_stack_list_test, push_pop) {
  struct stack_node* head = nullptr;

  struct stack_node node1 = {0};
  node1.value = 1;

  STACK_LIST_PUSH(head, &node1);
  EXPECT_EQ(&node1, head);

  struct stack_node node2 = {0};
  node2.value = 2;

  STACK_LIST_PUSH(head, &node2);
  EXPECT_EQ(&node2, head);
  EXPECT_EQ(&node1, node2.next);
}

TEST(containers_stack_list_test, pop) {
  struct stack_node* head = nullptr;

  struct stack_node node1 = {0};
  struct stack_node node2 = {0};

  STACK_LIST_PUSH(head, &node1);
  STACK_LIST_PUSH(head, &node2);

  struct stack_node* popped = nullptr;
  STACK_LIST_POP(head, popped);
  EXPECT_EQ(&node2, popped);
  EXPECT_EQ(&node1, head);

  STACK_LIST_POP(head, popped);
  EXPECT_EQ(&node1, popped);
  EXPECT_EQ(nullptr, head);
}

TEST(containers_stack_list_test, count) {
  struct stack_node* head = nullptr;
  sz count = 0;

  STACK_LIST_COUNT(head, count);
  EXPECT_EQ(0U, count);

  struct stack_node node1 = {0};
  struct stack_node node2 = {0};

  STACK_LIST_PUSH(head, &node1);
  STACK_LIST_PUSH(head, &node2);

  STACK_LIST_COUNT(head, count);
  EXPECT_EQ(2U, count);
}

TEST(containers_stack_list_test, head_macro) {
  struct stack_node* head = nullptr;

  struct stack_node node1 = {0};
  node1.value = 42;

  STACK_LIST_PUSH(head, &node1);

  EXPECT_EQ(&node1, STACK_LIST_HEAD(head));
}

TEST(containers_stack_list_test, next_macro) {
  struct stack_node* head = nullptr;

  struct stack_node node1 = {0};
  struct stack_node node2 = {0};

  STACK_LIST_PUSH(head, &node1);
  STACK_LIST_PUSH(head, &node2);

  EXPECT_EQ(&node1, STACK_LIST_NEXT(&node2));
}

TEST(containers_stack_list_test, foreach) {
  struct stack_node* head = nullptr;

  struct stack_node node1 = {0};
  struct stack_node node2 = {0};
  struct stack_node node3 = {0};

  node1.value = 1;
  node2.value = 2;
  node3.value = 3;

  STACK_LIST_PUSH(head, &node1);
  STACK_LIST_PUSH(head, &node2);
  STACK_LIST_PUSH(head, &node3);

  i32 sum = 0;
  STACK_LIST_FOREACH(head, it) {
    sum += it->value;
  }

  EXPECT_EQ(6, sum);
}
