// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/primitive_types.h"
#include "memory/allocator.h"

// =========================================================================
c_begin;
// =========================================================================

// Callback used by the generic array sort helpers.
// Returns <0 when lhs should sort before rhs, >0 when lhs should sort after rhs,
// and 0 when both elements compare equal.
typedef i32 sort_compare_fn(const void* lhs_ptr, const void* rhs_ptr, void* user_data);

// Checks if an array is sorted according to the provided comparison function.
// Returns true if the array is sorted, false otherwise.
func b32 sort_check(
    const void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data);

// Sorts an array in-place using the bubble sort algorithm.
// Returns the number of sorted elements.
// Best - worst - average: O(n) - O(n^2) - O(n^2)
// Faster for: small arrays, already sorted arrays, and nearly sorted arrays.
func sz sort_bubble(
    void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data);

// Sorts an array in-place using the quicksort algorithm.
// Returns the number of sorted elements.
// Best - worst - average: O(n log n) - O(n^2) - O(n log n)
// Faster for: large arrays and random data.
func sz sort_quick(
    void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data,
    allocator allocator);

// Sorts an array in-place using the merge sort algorithm.
// Returns the number of sorted elements.
// Best - worst - average: O(n log n) - O(n log n) - O(n log n)
// Faster for: large arrays and random data.
func sz sort_merge(
    void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data,
    allocator allocator);

// Sorts an array in-place using the selection sort algorithm.
// Returns the number of sorted elements.
// Best - worst - average: O(n^2) - O(n^2) - O(n^2)
// Faster for: small arrays and already sorted arrays.
func sz sort_selection(
    void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data);

// Sorts an array in-place using the insertion sort algorithm.
// Returns the number of sorted elements.
// Best - worst - average: O(n) - O(n^2) - O(n^2)
// Faster for: small arrays, already sorted arrays, and nearly sorted arrays.
func sz sort_insertion(
    void* ptr,
    sz elem_count,
    sz elem_size,
    sort_compare_fn* compare,
    void* user_data);

// Sorts an array of 32-bit unsigned integers in-place using the radix sort algorithm.
// Returns the number of sorted elements.
// Best - worst - average: O(n * k) - O(n * k) - O(n * k), where k is the number of digits in the largest number.
// Faster for: large arrays of integers with a small range of values.
func sz sort_radix32(u32* ptr, sz elem_count);

// Sorts an array of 64-bit unsigned integers in-place using the radix sort algorithm.
// Returns the number of sorted elements.
// Best - worst - average: O(n * k) - O(n * k) - O(n * k), where k is the number of digits in the largest number.
// Faster for: large arrays of integers with a small range of values.
func sz sort_radix64(u64* ptr, sz elem_count);

// =========================================================================
c_end;
// =========================================================================
