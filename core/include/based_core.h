// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

// Include basic module.
#include "basic/assert.h"
#include "basic/codespace.h"
#include "basic/entry.h"
#include "basic/entry_impl.h"
#include "basic/env_defines.h"
#include "basic/intrinsics.h"
#include "basic/keyword_defines.h"
#include "basic/primitive_types.h"
#include "basic/profiler.h"
#include "basic/utility_defines.h"

// Include container modules.
#include "containers/binary_tree.h"
#include "containers/bitset.h"
#include "containers/doubly_list.h"
#include "containers/hash_map.h"
#include "containers/ring_list.h"
#include "containers/singly_list.h"
#include "containers/sort.h"
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

// Include filesystem modules.
#include "filesystem/archive.h"
#include "filesystem/directory.h"
#include "filesystem/file.h"
#include "filesystem/filemap.h"
#include "filesystem/filestream.h"
#include "filesystem/module.h"
#include "filesystem/path.h"
#include "filesystem/pathinfo.h"
#include "filesystem/pathwatch.h"

// Include input modules.
#include "input/bindings.h"
#include "input/camera.h"
#include "input/capture.h"
#include "input/clipboard.h"
#include "input/devices.h"
#include "input/gamepads.h"
#include "input/input_state.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "input/sensor.h"
#include "input/tablet.h"
#include "input/touch.h"

// Include windowing modules.
#include "windowing/dialog.h"
#include "windowing/monitor.h"
#include "windowing/window.h"

// Include process modules.
#include "processes/common_processes.h"
#include "processes/pipe.h"
#include "processes/process.h"
#include "processes/process_current.h"

// Include system modules.
#include "system/cpu_info.h"
#include "system/system_info.h"
#include "system/system_runtime.h"

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
#include "utils/log_state.h"
#include "utils/stacktrace.h"
#include "utils/timer.h"
#include "utils/timestamp.h"
#include "utils/uuid.h"
#include "utils/version.h"

// Include context modules.
#include "context/ctx.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"

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

// Include olib, my own library for dealing with file formats.
#ifndef BASED_NO_OLIB
#  include <olib.h>
#endif

// Include libmath2, my own math library that integrates well with the codebase.
#ifndef BASED_NO_LIBMATH2
#  ifndef LM2_ENABLE_UNPREFIXED_NAMES
#    define LM2_ENABLE_UNPREFIXED_NAMES
#  endif
#  ifndef LM2_CUSTOM_ASSERT
#    define LM2_CUSTOM_ASSERT
#  endif
#  ifndef LM2_NO_GENERICS
#    define LM2_NO_GENERICS
#  endif
#  include <lm2.h>
#endif