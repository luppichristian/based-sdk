// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

struct tree_node {
  struct tree_node* parent;
  struct tree_node* first_child;
  struct tree_node* last_child;
  struct tree_node* next_sibling;
  struct tree_node* prev_sibling;
  i32 value;
};

TEST(containers_tree_test, next_preorder_ptr) {
  struct tree_node root = {0};
  struct tree_node child1 = {0};
  struct tree_node child2 = {0};

  root.first_child = &child1;
  root.last_child = &child2;
  child1.parent = &root;
  child2.parent = &root;
  child1.next_sibling = &child2;
  child2.prev_sibling = &child1;

  struct tree_node* next = (struct tree_node*)tree_next_preorder_ptr(&root, &root);
  EXPECT_EQ(&child1, next);

  next = (struct tree_node*)tree_next_preorder_ptr(&root, next);
  EXPECT_EQ(&child2, next);

  next = (struct tree_node*)tree_next_preorder_ptr(&root, next);
  EXPECT_EQ(nullptr, next);
}

TEST(containers_tree_test, is_root) {
  struct tree_node root = {0};
  struct tree_node child = {0};

  child.parent = &root;

  EXPECT_NE(0, TREE_IS_ROOT(&root));
  EXPECT_EQ(0, TREE_IS_ROOT(&child));
}

TEST(containers_tree_test, is_leaf) {
  struct tree_node root = {0};
  struct tree_node child = {0};

  root.first_child = &child;
  root.last_child = &child;

  EXPECT_EQ(0, TREE_IS_LEAF(&root));
  EXPECT_NE(0, TREE_IS_LEAF(&child));
}

TEST(containers_tree_test, insert_child_front) {
  struct tree_node parent_data = {0};
  struct tree_node child1 = {0};
  struct tree_node child2 = {0};
  struct tree_node* parent = &parent_data;
  struct tree_node* node = &child1;

  TREE_INSERT_CHILD_FRONT(parent, node);
  EXPECT_EQ(&child1, parent_data.first_child);
  EXPECT_EQ(&child1, parent_data.last_child);

  node = &child2;
  TREE_INSERT_CHILD_FRONT(parent, node);
  EXPECT_EQ(&child2, parent_data.first_child);
  EXPECT_EQ(&child1, parent_data.last_child);
  EXPECT_EQ(&child2, child1.prev_sibling);
}

TEST(containers_tree_test, insert_child_back) {
  struct tree_node parent_data = {0};
  struct tree_node child1 = {0};
  struct tree_node child2 = {0};
  struct tree_node* parent = &parent_data;
  struct tree_node* node = &child1;

  TREE_INSERT_CHILD_BACK(parent, node);
  EXPECT_EQ(&child1, parent_data.first_child);
  EXPECT_EQ(&child1, parent_data.last_child);

  node = &child2;
  TREE_INSERT_CHILD_BACK(parent, node);
  EXPECT_EQ(&child1, parent_data.first_child);
  EXPECT_EQ(&child2, parent_data.last_child);
  EXPECT_EQ(&child2, child1.next_sibling);
}

TEST(containers_tree_test, insert_before) {
  struct tree_node parent = {0};
  struct tree_node child1 = {0};
  struct tree_node child2 = {0};

  parent.first_child = &child1;
  parent.last_child = &child1;
  child1.parent = &parent;

  TREE_INSERT_BEFORE(&child1, &child2);
  EXPECT_EQ(&child2, parent.first_child);
  EXPECT_EQ(&child1, parent.last_child);
}

TEST(containers_tree_test, insert_after) {
  struct tree_node parent = {0};
  struct tree_node child1 = {0};
  struct tree_node child2 = {0};

  parent.first_child = &child1;
  parent.last_child = &child1;
  child1.parent = &parent;

  TREE_INSERT_AFTER(&child1, &child2);
  EXPECT_EQ(&child1, parent.first_child);
  EXPECT_EQ(&child2, parent.last_child);
  EXPECT_EQ(&child2, child1.next_sibling);
}

TEST(containers_tree_test, remove) {
  struct tree_node parent = {0};
  struct tree_node child = {0};

  parent.first_child = &child;
  parent.last_child = &child;
  child.parent = &parent;

  TREE_REMOVE(&child);
  EXPECT_EQ(nullptr, parent.first_child);
  EXPECT_EQ(nullptr, parent.last_child);
  EXPECT_EQ(nullptr, child.parent);
}

TEST(containers_tree_test, foreach_children) {
  struct tree_node parent_data = {0};
  struct tree_node child1 = {0};
  struct tree_node child2 = {0};
  struct tree_node child3 = {0};
  struct tree_node* parent = &parent_data;
  struct tree_node* node = &child1;

  child1.value = 1;
  child2.value = 2;
  child3.value = 3;

  TREE_INSERT_CHILD_BACK(parent, node);
  node = &child2;
  TREE_INSERT_CHILD_BACK(parent, node);
  node = &child3;
  TREE_INSERT_CHILD_BACK(parent, node);

  i32 sum = 0;
  TREE_FOREACH_CHILDREN(parent, it) {
    sum += it->value;
  }

  EXPECT_EQ(6, sum);
}

TEST(containers_tree_test, foreach_children_reverse) {
  struct tree_node parent_data = {0};
  struct tree_node child1 = {0};
  struct tree_node child2 = {0};
  struct tree_node child3 = {0};
  struct tree_node* parent = &parent_data;
  struct tree_node* node = &child1;

  child1.value = 1;
  child2.value = 2;
  child3.value = 3;

  TREE_INSERT_CHILD_BACK(parent, node);
  node = &child2;
  TREE_INSERT_CHILD_BACK(parent, node);
  node = &child3;
  TREE_INSERT_CHILD_BACK(parent, node);

  i32 sum = 0;
  TREE_FOREACH_CHILDREN_REVERSE(parent, it) {
    sum += it->value;
  }

  EXPECT_EQ(6, sum);
}

TEST(containers_tree_test, foreach_preorder) {
  struct tree_node root = {0};
  struct tree_node child1 = {0};
  struct tree_node child2 = {0};
  struct tree_node grandchild = {0};
  struct tree_node* parent = &root;
  struct tree_node* node = &child1;

  root.value = 1;
  child1.value = 2;
  child2.value = 3;
  grandchild.value = 4;

  TREE_INSERT_CHILD_BACK(parent, node);
  node = &child2;
  TREE_INSERT_CHILD_BACK(parent, node);
  parent = &child1;
  node = &grandchild;
  TREE_INSERT_CHILD_BACK(parent, node);

  i32 sum = 0;
  TREE_FOREACH_PREORDER(&root, it) {
    sum += it->value;
  }

  EXPECT_EQ(10, sum);
}
