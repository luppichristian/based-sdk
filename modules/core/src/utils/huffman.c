// MIT License
// Copyright (c) 2026 Christian Luppi

#include "utils/huffman.h"

#include "basic/assert.h"
#include "basic/profiler.h"
#include "basic/safe.h"
#include "context/thread_ctx.h"
#include "memory/memops.h"

func void huffman_code_clear_internal(huffman_code* code) {
  if (code == NULL) {
    return;
  }

  mem_zero_value(code);
}

func void huffman_code_set_bit_internal(huffman_code* code, sz bit_idx, b32 bit_value) {
  if (code == NULL || bit_idx >= (sz)(HUFFMAN_CODE_WORD_CAP * 64)) {
    return;
  }

  sz word_idx = bit_idx / 64;
  sz word_bit = bit_idx % 64;
  u64 mask = 1ULL << word_bit;
  if (bit_value != 0) {
    code->bits[word_idx] |= mask;
  } else {
    code->bits[word_idx] &= ~mask;
  }
}

func huffman_code huffman_code_append_bit_internal(huffman_code code, b32 bit_value) {
  if (code.bit_length < (HUFFMAN_CODE_WORD_CAP * 64U) - 1U) {
    huffman_code_set_bit_internal(&code, (sz)code.bit_length, bit_value);
    code.bit_length++;
  }
  return code;
}

func b32 huffman_tree_is_leaf_internal(const huffman_tree* tree, u16 node_idx) {
  return tree != NULL && node_idx < tree->node_count && tree->node_is_leaf[node_idx] != 0;
}

func void huffman_tree_assign_codes_internal(
    huffman_tree* tree,
    u16 node_idx,
    huffman_code prefix) {
  if (tree == NULL || node_idx >= tree->node_count) {
    return;
  }

  if (tree->node_is_leaf[node_idx] != 0) {
    if (prefix.bit_length == 0) {
      prefix.bit_length = 1;
    }
    tree->codes[tree->node_symbol[node_idx]] = prefix;
    return;
  }

  huffman_tree_assign_codes_internal(
      tree,
      tree->node_left[node_idx],
      huffman_code_append_bit_internal(prefix, 0));
  huffman_tree_assign_codes_internal(
      tree,
      tree->node_right[node_idx],
      huffman_code_append_bit_internal(prefix, 1));
}

func void huffman_tree_select_smallest_internal(
    const huffman_tree* tree,
    const u16* active_nodes,
    sz active_count,
    sz* out_first_idx,
    sz* out_second_idx) {
  *out_first_idx = 0;
  *out_second_idx = 1;

  safe_for (sz idx = 0; idx < active_count; idx++) {
    sz current_best = *out_first_idx;
    u16 current_node = active_nodes[idx];
    u16 best_node = active_nodes[current_best];
    if (tree->node_frequencies[current_node] < tree->node_frequencies[best_node] ||
        (tree->node_frequencies[current_node] == tree->node_frequencies[best_node] &&
         current_node < best_node)) {
      *out_first_idx = idx;
    }
  }

  if (*out_second_idx == *out_first_idx) {
    *out_second_idx = (*out_first_idx == 0) ? 1 : 0;
  }

  safe_for (sz idx = 0; idx < active_count; idx++) {
    if (idx == *out_first_idx) {
      continue;
    }

    sz current_best = *out_second_idx;
    if (current_best == *out_first_idx) {
      *out_second_idx = idx;
      continue;
    }

    u16 current_node = active_nodes[idx];
    u16 best_node = active_nodes[current_best];
    if (tree->node_frequencies[current_node] < tree->node_frequencies[best_node] ||
        (tree->node_frequencies[current_node] == tree->node_frequencies[best_node] &&
         current_node < best_node)) {
      *out_second_idx = idx;
    }
  }

  if (*out_second_idx < *out_first_idx) {
    swap_ptr(out_first_idx, out_second_idx);
  }
}

func b32 huffman_tree_validate_symbol_count_internal(sz symbol_count, cstr8 func_name) {
  if (symbol_count > HUFFMAN_SYMBOL_CAP) {
    thread_log_error("%s: Symbol count %zu exceeds %u", func_name, symbol_count, HUFFMAN_SYMBOL_CAP);
    return false;
  }

  return true;
}

func void huffman_tree_clear(huffman_tree* tree) {
  profile_func_begin;
  if (tree == NULL) {
    thread_log_warn("huffman_tree_clear: Tree is null");
    profile_func_end;
    return;
  }

  mem_zero_value(tree);
  tree->root_idx = HUFFMAN_INVALID_NODE;
  tree->root_symbol = HUFFMAN_INVALID_NODE;
  safe_for (sz idx = 0; idx < HUFFMAN_NODE_CAP; idx++) {
    tree->node_left[idx] = HUFFMAN_INVALID_NODE;
    tree->node_right[idx] = HUFFMAN_INVALID_NODE;
    tree->node_symbol[idx] = HUFFMAN_INVALID_NODE;
  }
  profile_func_end;
}

func b32 huffman_tree_build(huffman_tree* tree, const u64* frequencies, sz symbol_count) {
  profile_func_begin;
  if (tree == NULL || frequencies == NULL) {
    thread_log_error("huffman_tree_build: Tree or frequencies are null");
    profile_func_end;
    return false;
  }

  if (!huffman_tree_validate_symbol_count_internal(symbol_count, "huffman_tree_build")) {
    profile_func_end;
    return false;
  }

  huffman_tree_clear(tree);
  safe_for (sz sym_idx = 0; sym_idx < symbol_count; sym_idx++) {
    tree->symbol_frequencies[sym_idx] = frequencies[sym_idx];
  }

  u16 active_nodes[HUFFMAN_SYMBOL_CAP] = {0};
  sz active_count = 0;

  safe_for (sz sym_idx = 0; sym_idx < symbol_count; sym_idx++) {
    u64 frequency = frequencies[sym_idx];
    if (frequency == 0) {
      continue;
    }

    u16 node_idx = (u16)tree->node_count;
    tree->node_count++;
    tree->symbol_count++;
    tree->node_frequencies[node_idx] = frequency;
    tree->node_left[node_idx] = HUFFMAN_INVALID_NODE;
    tree->node_right[node_idx] = HUFFMAN_INVALID_NODE;
    tree->node_symbol[node_idx] = (u16)sym_idx;
    tree->node_is_leaf[node_idx] = 1;
    active_nodes[active_count++] = node_idx;
    tree->root_symbol = (u16)sym_idx;
  }

  if (tree->node_count == 0) {
    thread_log_debug("huffman_tree_build: No active symbols");
    profile_func_end;
    return true;
  }

  while (active_count > 1) {
    sz first_idx = 0;
    sz second_idx = 1;
    huffman_tree_select_smallest_internal(tree, active_nodes, active_count, &first_idx, &second_idx);

    u16 left_idx = active_nodes[first_idx];
    u16 right_idx = active_nodes[second_idx];
    u16 parent_idx = (u16)tree->node_count;
    tree->node_count++;
    tree->node_frequencies[parent_idx] = tree->node_frequencies[left_idx] + tree->node_frequencies[right_idx];
    tree->node_left[parent_idx] = left_idx;
    tree->node_right[parent_idx] = right_idx;
    tree->node_symbol[parent_idx] = tree->node_symbol[left_idx] < tree->node_symbol[right_idx]
                                        ? tree->node_symbol[left_idx]
                                        : tree->node_symbol[right_idx];
    tree->node_is_leaf[parent_idx] = 0;

    active_nodes[first_idx] = parent_idx;
    safe_for (sz move_idx = second_idx; move_idx + 1 < active_count; move_idx++) {
      active_nodes[move_idx] = active_nodes[move_idx + 1];
    }
    active_count--;
  }

  tree->root_idx = active_nodes[0];
  tree->root_symbol = tree->node_symbol[tree->root_idx];
  huffman_tree_assign_codes_internal(tree, tree->root_idx, (huffman_code) {0});
  thread_log_debug(
      "huffman_tree_build: Built tree with %zu symbols and %zu nodes",
      tree->symbol_count,
      tree->node_count);
  profile_func_end;
  return true;
}

func b32 huffman_tree_build_for_bytes(huffman_tree* tree, buffer src) {
  profile_func_begin;
  if (tree == NULL) {
    thread_log_error("huffman_tree_build_for_bytes: Tree is null");
    profile_func_end;
    return false;
  }

  if (src.size > 0 && src.ptr == NULL) {
    thread_log_error("huffman_tree_build_for_bytes: Source pointer is null for non-empty buffer");
    profile_func_end;
    return false;
  }

  u64 frequencies[HUFFMAN_SYMBOL_CAP] = {0};
  const u8* src_bytes = (const u8*)src.ptr;
  safe_for (sz idx = 0; idx < src.size; idx++) {
    frequencies[src_bytes[idx]]++;
  }

  b32 result = huffman_tree_build(tree, frequencies, HUFFMAN_SYMBOL_CAP);
  profile_func_end;
  return result;
}

func b32 huffman_tree_has_symbol(const huffman_tree* tree, u8 symbol) {
  profile_func_begin;
  b32 has_symbol = tree != NULL && tree->symbol_frequencies[symbol] != 0;
  profile_func_end;
  return has_symbol;
}

func huffman_code huffman_tree_get_code(const huffman_tree* tree, u8 symbol) {
  profile_func_begin;
  huffman_code code = {0};
  if (tree == NULL || tree->symbol_frequencies[symbol] == 0) {
    if (tree == NULL) {
      thread_log_warn("huffman_tree_get_code: Tree is null");
    }
    profile_func_end;
    return code;
  }

  code = tree->codes[symbol];
  profile_func_end;
  return code;
}

func b32 huffman_code_get_bit(const huffman_code* code, sz bit_idx) {
  profile_func_begin;
  if (code == NULL || bit_idx >= code->bit_length || bit_idx >= (sz)(HUFFMAN_CODE_WORD_CAP * 64)) {
    profile_func_end;
    return false;
  }

  sz word_idx = bit_idx / 64;
  sz word_bit = bit_idx % 64;
  b32 bit_value = (code->bits[word_idx] & (1ULL << word_bit)) != 0;
  profile_func_end;
  return bit_value;
}

func u64 huffman_tree_bit_length_for_bytes(const huffman_tree* tree, buffer src) {
  profile_func_begin;
  if (tree == NULL) {
    thread_log_error("huffman_tree_bit_length_for_bytes: Tree is null");
    profile_func_end;
    return 0;
  }

  if (src.size > 0 && src.ptr == NULL) {
    thread_log_error("huffman_tree_bit_length_for_bytes: Source pointer is null for non-empty buffer");
    profile_func_end;
    return 0;
  }

  const u8* src_bytes = (const u8*)src.ptr;
  u64 total_bits = 0;
  safe_for (sz idx = 0; idx < src.size; idx++) {
    huffman_code code = tree->codes[src_bytes[idx]];
    if (tree->symbol_frequencies[src_bytes[idx]] == 0 || code.bit_length == 0) {
      thread_log_error("huffman_tree_bit_length_for_bytes: Missing code for symbol %u", src_bytes[idx]);
      profile_func_end;
      return 0;
    }

    total_bits += code.bit_length;
  }

  profile_func_end;
  return total_bits;
}

func b32 huffman_tree_decode_symbol(
    const huffman_tree* tree,
    buffer bitstream,
    sz bit_offset,
    u8* out_symbol,
    sz* out_next_bit_offset) {
  profile_func_begin;
  if (tree == NULL || out_symbol == NULL || out_next_bit_offset == NULL) {
    thread_log_error("huffman_tree_decode_symbol: Invalid arguments");
    profile_func_end;
    return false;
  }

  if (bitstream.size > 0 && bitstream.ptr == NULL) {
    thread_log_error("huffman_tree_decode_symbol: Bitstream pointer is null for non-empty buffer");
    profile_func_end;
    return false;
  }

  if (tree->node_count == 0 || tree->root_idx == HUFFMAN_INVALID_NODE) {
    thread_log_warn("huffman_tree_decode_symbol: Tree is empty");
    profile_func_end;
    return false;
  }

  if (huffman_tree_is_leaf_internal(tree, tree->root_idx)) {
    *out_symbol = (u8)tree->node_symbol[tree->root_idx];
    *out_next_bit_offset = bit_offset;
    profile_func_end;
    return true;
  }

  const u8* bit_bytes = (const u8*)bitstream.ptr;
  u16 node_idx = tree->root_idx;
  sz current_offset = bit_offset;
  sz total_bits = bitstream.size * 8;

  while (node_idx != HUFFMAN_INVALID_NODE && tree->node_is_leaf[node_idx] == 0) {
    if (current_offset >= total_bits) {
      thread_log_error("huffman_tree_decode_symbol: Bitstream ended before reaching a leaf");
      profile_func_end;
      return false;
    }

    sz byte_idx = current_offset / 8;
    sz bit_idx = current_offset % 8;
    b32 bit_value = (bit_bytes[byte_idx] & (u8)(1U << bit_idx)) != 0;
    node_idx = bit_value != 0 ? tree->node_right[node_idx] : tree->node_left[node_idx];
    current_offset++;
  }

  if (node_idx == HUFFMAN_INVALID_NODE || tree->node_is_leaf[node_idx] == 0) {
    thread_log_error("huffman_tree_decode_symbol: Encountered an invalid node");
    profile_func_end;
    return false;
  }

  *out_symbol = (u8)tree->node_symbol[node_idx];
  *out_next_bit_offset = current_offset;
  profile_func_end;
  return true;
}
