// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// CRC-32 (IEEE 802.3 / PKZip) helpers.
func u32 crc32_init(void);
func u32 crc32_update(u32 crc, const void* data, sz size);
func u32 crc32_finalize(u32 crc);
func u32 crc32(const void* data, sz size);

// CRC-64 (ECMA-182) helpers.
func u64 crc64_init(void);
func u64 crc64_update(u64 crc, const void* data, sz size);
func u64 crc64_finalize(u64 crc);
func u64 crc64(const void* data, sz size);
