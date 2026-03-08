// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

struct ring_node {
  struct ring_node* next;
  struct ring_node* prev;
  i32 value;
};

TEST(containers_ring_list_test, empty_macro) {
  struct ring_node* head = nullptr;

  EXPECT_NE(0, RING_LIST_EMPTY(head));
}

TEST(containers_ring_list_test, push_front) {
  struct ring_node* head = nullptr;

  struct ring_node node1 = {0};
  node1.value = 1;

  RING_LIST_PUSH_FRONT(head, &node1);
  EXPECT_EQ(&node1, head);
  EXPECT_EQ(&node1, node1.prev);

  struct ring_node node2 = {0};
  node2.value = 2;

  RING_LIST_PUSH_FRONT(head, &node2);
  EXPECT_EQ(&node2, head);
}

TEST(containers_ring_list_test, push_back) {
  struct ring_node* head = nullptr;

  struct ring_node node1 = {0};
  node1.value = 1;

  RING_LIST_PUSH_BACK(head, &node1);
  EXPECT_EQ(&node1, head);
  EXPECT_EQ(&node1, node1.prev);

  struct ring_node node2 = {0};
  node2.value = 2;

  RING_LIST_PUSH_BACK(head, &node2);
  EXPECT_EQ(&node1, head);
}

TEST(containers_ring_list_test, pop_front) {
  struct ring_node* head = nullptr;

  struct ring_node node1 = {0};
  struct ring_node node2 = {0};

  RING_LIST_PUSH_BACK(head, &node1);
  RING_LIST_PUSH_BACK(head, &node2);

  struct ring_node* popped = nullptr;
  RING_LIST_POP_FRONT(head, popped);
  EXPECT_EQ(&node1, popped);
  EXPECT_EQ(&node2, head);

  RING_LIST_POP_FRONT(head, popped);
  EXPECT_EQ(&node2, popped);
  EXPECT_EQ(nullptr, head);
}

TEST(containers_ring_list_test, pop_back) {
  struct ring_node* head = nullptr;

  struct ring_node node1 = {0};
  struct ring_node node2 = {0};

  RING_LIST_PUSH_BACK(head, &node1);
  RING_LIST_PUSH_BACK(head, &node2);

  struct ring_node* popped = nullptr;
  RING_LIST_POP_BACK(head, popped);
  EXPECT_EQ(&node2, popped);
  EXPECT_EQ(&node1, head);

  RING_LIST_POP_BACK(head, popped);
  EXPECT_EQ(&node1, popped);
  EXPECT_EQ(nullptr, head);
}

TEST(containers_ring_list_test, remove) {
  struct ring_node* head = nullptr;

  struct ring_node node1 = {0};
  struct ring_node node2 = {0};
  struct ring_node node3 = {0};

  RING_LIST_PUSH_BACK(head, &node1);
  RING_LIST_PUSH_BACK(head, &node2);
  RING_LIST_PUSH_BACK(head, &node3);

  RING_LIST_REMOVE(head, &node2);
  EXPECT_EQ(&node1, head);
  EXPECT_EQ(&node3, node1.next);
  EXPECT_EQ(&node1, node3.prev);

  RING_LIST_REMOVE(head, &node1);
  EXPECT_EQ(&node3, head);
}

TEST(containers_ring_list_test, head_tail) {
  struct ring_node* head = nullptr;

  struct ring_node node1 = {0};
  struct ring_node node2 = {0};

  RING_LIST_PUSH_BACK(head, &node1);
  RING_LIST_PUSH_BACK(head, &node2);

  EXPECT_EQ(&node1, RING_LIST_HEAD(head));
  EXPECT_EQ(&node2, RING_LIST_TAIL(head));
}

TEST(containers_ring_list_test, count) {
  struct ring_node* head = nullptr;
  sz count = 0;

  RING_LIST_COUNT(head, count);
  EXPECT_EQ(0U, count);

  struct ring_node node1 = {0};
  struct ring_node node2 = {0};

  RING_LIST_PUSH_BACK(head, &node1);
  RING_LIST_PUSH_BACK(head, &node2);

  RING_LIST_COUNT(head, count);
  EXPECT_EQ(2U, count);
}

TEST(containers_ring_list_test, foreach) {
  struct ring_node* head = nullptr;

  struct ring_node node1 = {0};
  struct ring_node node2 = {0};
  struct ring_node node3 = {0};

  node1.value = 1;
  node2.value = 2;
  node3.value = 3;

  RING_LIST_PUSH_BACK(head, &node1);
  RING_LIST_PUSH_BACK(head, &node2);
  RING_LIST_PUSH_BACK(head, &node3);

  i32 sum = 0;
  RING_LIST_FOREACH(head, it) {
    sum += it->value;
  }

  EXPECT_EQ(6, sum);
}

TEST(containers_ring_list_test, foreach_reverse) {
  struct ring_node* head = nullptr;

  struct ring_node node1 = {0};
  struct ring_node node2 = {0};
  struct ring_node node3 = {0};

  node1.value = 1;
  node2.value = 2;
  node3.value = 3;

  RING_LIST_PUSH_BACK(head, &node1);
  RING_LIST_PUSH_BACK(head, &node2);
  RING_LIST_PUSH_BACK(head, &node3);

  i32 sum = 0;
  RING_LIST_FOREACH_REVERSE(head, it) {
    sum += it->value;
  }

  EXPECT_EQ(6, sum);
}
