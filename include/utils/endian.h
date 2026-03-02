// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// Returns 1 if the host is little-endian, 0 otherwise.
func b32 endian_is_little(void);

// Returns 1 if the host is big-endian, 0 otherwise.
func b32 endian_is_big(void);

// Little-endian to native-endian conversion.
func u16 endian_le16_to_native(u16 value);
func u32 endian_le32_to_native(u32 value);
func u64 endian_le64_to_native(u64 value);

// Big-endian to native-endian conversion.
func u16 endian_be16_to_native(u16 value);
func u32 endian_be32_to_native(u32 value);
func u64 endian_be64_to_native(u64 value);

// Native-endian to little-endian conversion.
func u16 endian_native_to_le16(u16 value);
func u32 endian_native_to_le32(u32 value);
func u64 endian_native_to_le64(u64 value);

// Native-endian to big-endian conversion.
func u16 endian_native_to_be16(u16 value);
func u32 endian_native_to_be32(u32 value);
func u64 endian_native_to_be64(u64 value);
