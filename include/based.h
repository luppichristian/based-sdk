// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

// Include basic module.
#include "basic/assert.h"
#include "basic/codespace.h"
#include "basic/entry.h"
#include "basic/env_defines.h"
#include "basic/intrinsics.h"
#include "basic/keyword_defines.h"
#include "basic/primitive_types.h"
#include "basic/utility_defines.h"

// Include container modules.
#include "containers/binary_tree.h"
#include "containers/bitset.h"
#include "containers/doubly_list.h"
#include "containers/hash_map.h"
#include "containers/ring_list.h"
#include "containers/singly_list.h"
#include "containers/stack_list.h"
#include "containers/tree.h"

// Include memory modules.
#include "memory/allocator.h"
#include "memory/arena.h"
#include "memory/buffer.h"
#include "memory/heap.h"
#include "memory/pool.h"
#include "memory/ring.h"
#include "memory/scratch.h"
#include "memory/vmem.h"

// Include process modules.
#include "processes/pipe.h"
#include "processes/process.h"
#include "processes/process_current.h"

// Include string modules.
#include "strings/char.h"
#include "strings/cstrings.h"
#include "strings/strings.h"
#include "strings/unicode.h"

// Include utility modules.
#include "utils/cmdline.h"
#include "utils/crc.h"
#include "utils/endian.h"
#include "utils/id.h"
#include "utils/timer.h"
#include "utils/timestamp.h"
#include "utils/uuid.h"
#include "utils/version.h"

// Include threading modules.
#include "threads/atomics.h"
#include "threads/condvar.h"
#include "threads/mutex.h"
#include "threads/rwlock.h"
#include "threads/semaphore.h"
#include "threads/spinlock.h"
#include "threads/thread.h"
#include "threads/thread_current.h"
#include "threads/thread_group.h"

// Olib, my library for working with file formats...
#include <olib.h>

// libmath2, my library for math utilities and functions...
// Should be complete with everything needed.
#ifndef LM2_ENABLE_UNPREFIXED_NAMES
#  define LM2_ENABLE_UNPREFIXED_NAMES
#endif
#ifndef LM2_CUSTOM_ASSERT
#  define LM2_CUSTOM_ASSERT
#endif
#include <lm2.h>
