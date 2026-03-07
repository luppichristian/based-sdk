// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// CRC-32 (IEEE 802.3 / PKZip) helpers.
// Seed value for incremental CRC-32 computations.
func u32 crc32_init(void);
// Updates an incremental CRC-32 with a byte range.
func u32 crc32_update(u32 crc, const void* data, sz size);
// Finalizes an incremental CRC-32 into the standard output value.
func u32 crc32_finalize(u32 crc);
// One-shot CRC-32 convenience helper.
func u32 crc32(const void* data, sz size);

// CRC-64 (ECMA-182) helpers.
// Seed value for incremental CRC-64 computations.
func u64 crc64_init(void);
// Updates an incremental CRC-64 with a byte range.
func u64 crc64_update(u64 crc, const void* data, sz size);
// Finalizes an incremental CRC-64 value.
func u64 crc64_finalize(u64 crc);
// One-shot CRC-64 convenience helper.
func u64 crc64(const void* data, sz size);
