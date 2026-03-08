// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

struct list_node {
  struct list_node* next;
  struct list_node* prev;
  i32 value;
};

TEST(containers_doubly_list_test, empty_macro) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  EXPECT_NE(0, DOUBLY_LIST_EMPTY(head, tail));
}

TEST(containers_doubly_list_test, push_front) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  struct list_node node1 = {0};
  node1.value = 1;

  DOUBLY_LIST_PUSH_FRONT(head, tail, &node1);
  EXPECT_EQ(&node1, head);
  EXPECT_EQ(&node1, tail);
  EXPECT_EQ(0, node1.prev);

  struct list_node node2 = {0};
  node2.value = 2;

  DOUBLY_LIST_PUSH_FRONT(head, tail, &node2);
  EXPECT_EQ(&node2, head);
  EXPECT_EQ(&node1, tail);
  EXPECT_EQ(&node2, node1.prev);
}

TEST(containers_doubly_list_test, push_back) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  struct list_node node1 = {0};
  node1.value = 1;

  DOUBLY_LIST_PUSH_BACK(head, tail, &node1);
  EXPECT_EQ(&node1, head);
  EXPECT_EQ(&node1, tail);

  struct list_node node2 = {0};
  node2.value = 2;

  DOUBLY_LIST_PUSH_BACK(head, tail, &node2);
  EXPECT_EQ(&node1, head);
  EXPECT_EQ(&node2, tail);
}

TEST(containers_doubly_list_test, pop_front) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  struct list_node node1 = {0};
  struct list_node node2 = {0};

  DOUBLY_LIST_PUSH_BACK(head, tail, &node1);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node2);

  struct list_node* popped = nullptr;
  DOUBLY_LIST_POP_FRONT(head, tail, popped);
  EXPECT_EQ(&node1, popped);
  EXPECT_EQ(&node2, head);
  EXPECT_EQ(0, node2.prev);

  DOUBLY_LIST_POP_FRONT(head, tail, popped);
  EXPECT_EQ(&node2, popped);
  EXPECT_EQ(nullptr, head);
  EXPECT_EQ(nullptr, tail);
}

TEST(containers_doubly_list_test, pop_back) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  struct list_node node1 = {0};
  struct list_node node2 = {0};

  DOUBLY_LIST_PUSH_BACK(head, tail, &node1);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node2);

  struct list_node* popped = nullptr;
  DOUBLY_LIST_POP_BACK(head, tail, popped);
  EXPECT_EQ(&node2, popped);
  EXPECT_EQ(&node1, tail);

  DOUBLY_LIST_POP_BACK(head, tail, popped);
  EXPECT_EQ(&node1, popped);
  EXPECT_EQ(nullptr, head);
  EXPECT_EQ(nullptr, tail);
}

TEST(containers_doubly_list_test, remove) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  struct list_node node1 = {0};
  struct list_node node2 = {0};
  struct list_node node3 = {0};

  DOUBLY_LIST_PUSH_BACK(head, tail, &node1);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node2);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node3);

  DOUBLY_LIST_REMOVE(head, tail, &node2);
  EXPECT_EQ(&node1, head);
  EXPECT_EQ(&node3, tail);
  EXPECT_EQ(&node3, node1.next);
  EXPECT_EQ(&node1, node3.prev);

  DOUBLY_LIST_REMOVE(head, tail, &node1);
  EXPECT_EQ(&node3, head);
  EXPECT_EQ(&node3, tail);
}

TEST(containers_doubly_list_test, insert_after) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  struct list_node node1 = {0};
  struct list_node node2 = {0};
  struct list_node node3 = {0};

  DOUBLY_LIST_PUSH_BACK(head, tail, &node1);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node3);

  DOUBLY_LIST_INSERT_AFTER(head, tail, &node1, &node2);
  EXPECT_EQ(&node2, node1.next);
  EXPECT_EQ(&node3, node2.next);
}

TEST(containers_doubly_list_test, insert_before) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  struct list_node node1 = {0};
  struct list_node node2 = {0};
  struct list_node node3 = {0};

  DOUBLY_LIST_PUSH_BACK(head, tail, &node1);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node3);

  DOUBLY_LIST_INSERT_BEFORE(head, tail, &node3, &node2);
  EXPECT_EQ(&node2, node1.next);
  EXPECT_EQ(&node3, node2.next);
}

TEST(containers_doubly_list_test, count) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;
  sz count = 0;

  DOUBLY_LIST_COUNT(head, tail, count);
  EXPECT_EQ(0U, count);

  struct list_node node1 = {0};
  struct list_node node2 = {0};

  DOUBLY_LIST_PUSH_BACK(head, tail, &node1);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node2);

  DOUBLY_LIST_COUNT(head, tail, count);
  EXPECT_EQ(2U, count);
}

TEST(containers_doubly_list_test, head_tail) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  struct list_node node1 = {0};
  struct list_node node2 = {0};

  DOUBLY_LIST_PUSH_BACK(head, tail, &node1);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node2);

  EXPECT_EQ(&node1, DOUBLY_LIST_HEAD(head, tail));
  EXPECT_EQ(&node2, DOUBLY_LIST_TAIL(head, tail));
}

TEST(containers_doubly_list_test, foreach) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  struct list_node node1 = {0};
  struct list_node node2 = {0};
  struct list_node node3 = {0};

  node1.value = 1;
  node2.value = 2;
  node3.value = 3;

  DOUBLY_LIST_PUSH_BACK(head, tail, &node1);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node2);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node3);

  i32 sum = 0;
  DOUBLY_LIST_FOREACH(head, tail, it) {
    sum += it->value;
  }

  EXPECT_EQ(6, sum);
}

TEST(containers_doubly_list_test, foreach_reverse) {
  struct list_node* head = nullptr;
  struct list_node* tail = nullptr;

  struct list_node node1 = {0};
  struct list_node node2 = {0};
  struct list_node node3 = {0};

  node1.value = 1;
  node2.value = 2;
  node3.value = 3;

  DOUBLY_LIST_PUSH_BACK(head, tail, &node1);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node2);
  DOUBLY_LIST_PUSH_BACK(head, tail, &node3);

  i32 sum = 0;
  DOUBLY_LIST_FOREACH_REVERSE(head, tail, it) {
    sum += it->value;
  }

  EXPECT_EQ(6, sum);
}
