# Core Module Quick Guide

`core` is the foundational module of based. Use it for platform abstractions, memory/context systems, containers, I/O and filesystem utilities, input/event flow, threading, and shared diagnostics/logging primitives.

This guide is organized by API area and then by header so you can quickly jump to relevant types and functions.

## API Areas

- `basic`
- `containers`
- `context`
- `filesystem`
- `input`
- `interface`
- `memory`
- `processes`
- `strings`
- `system`
- `threads`
- `utils`

## basic

Compiler/platform layer, primitive types, assertions, crash routing, intrinsics and foundational macros.

### `basic/assert.h`

- Types: **1**
- Functions: **3**

#### Types

- `assert_mode` (enum)
  - What it represents: Represents a set of named constants for `assert mode`
  - Members:
    - `ASSERT_MODE_DEBUG`: `enum value`
    - `ASSERT_MODE_QUIT`: `enum value`
    - `ASSERT_MODE_ABORT`: `enum value`
    - `ASSERT_MODE_LOG`: `enum value`
    - `ASSERT_MODE_IGNORE`: `enum value`

#### Functions

- `_assert(b32 condition, const char* msg, callsite site) -> void`
  - What it does: Performs  assert
  - Parameters:
    - `condition` (`b32`): Input parameter
    - `msg` (`const char*`): Input parameter
    - `site` (`callsite`): Input parameter
- `assert_set_mode(assert_mode mode) -> void`
  - What it does: Sets assert set mode
  - Parameters:
    - `mode` (`assert_mode`): Configuration value controlling behavior
- `assert_get_mode() -> assert_mode`
  - What it does: Gets assert get mode
  - Parameters: none

### `basic/crash.h`

- Types: **0**
- Functions: **3**

#### Types

- None

#### Functions

- `crash_install() -> b32`
  - What it does: Installs platform crash handlers that route unhandled faults through the assert system
  - Parameters: none
- `crash_uninstall() -> void`
  - What it does: Restores the previous platform crash handlers
  - Parameters: none
- `crash_is_installed() -> b32`
  - What it does: Reports whether the crash handler is currently installed
  - Parameters: none

### `basic/codespace.h`

- Types: **2**
- Functions: **0**

#### Types

- `callsite` (struct)
  - What it represents: Represents `callsite` data grouped in a struct
  - Members:
    - `filename`: `cstr8`
    - `function`: `cstr8`
    - `line`: `u32`
- `src_loc` (struct)
  - What it represents: Represents `src loc` data grouped in a struct
  - Members:
    - `filename`: `cstr8`
    - `line`: `u32`

#### Functions

- None

### `basic/entry.h`

- Types: **7**
- Functions: **11**

#### Types

- `app_result` (enum)
  - What it represents: Represents a set of named constants for `app result`
  - Members:
    - `APP_RESULT_CONTINUE`: `enum value`
    - `APP_RESULT_SUCCESS`: `enum value`
    - `APP_RESULT_FAIL`: `enum value`
    - `APP_RESULT_MAX`: `enum value`
- `entry_app_init_fn` (alias)
  - What it represents: Type alias used for `entry app init fn`
  - Members: none (alias/function type)
- `entry_app_update_fn` (alias)
  - What it represents: Type alias used for `entry app update fn`
  - Members: none (alias/function type)
- `entry_app_quit_fn` (alias)
  - What it represents: Type alias used for `entry app quit fn`
  - Members: none (alias/function type)
- `entry_app_callbacks` (struct)
  - What it represents: Represents `entry app callbacks` data grouped in a struct
  - Members:
    - `init_fn`: `entry_app_init_fn*`
    - `update_fn`: `entry_app_update_fn*`
    - `quit_fn`: `entry_app_quit_fn*`
- `entry_run_fn` (alias)
  - What it represents: Type alias used for `entry run fn`
  - Members: none (alias/function type)
- `entry_run_callbacks` (struct)
  - What it represents: Represents `entry run callbacks` data grouped in a struct
  - Members:
    - `run_fn`: `entry_run_fn*`

#### Functions

- `entry_init(cmdline cmdline) -> b32`
  - What it does: Initializes entry init
  - Parameters:
    - `cmdline` (`cmdline`): Input parameter
- `entry_get_cmdline() -> cmdline`
  - What it does: Gets entry get cmdline
  - Parameters: none
- `entry_get_allocator() -> allocator`
  - What it does: Gets entry get allocator
  - Parameters: none
- `entry_get_ctx_setup() -> ctx_setup`
  - What it does: Gets entry get ctx setup
  - Parameters: none
- `entry_quit() -> void`
  - What it does: Shuts down entry quit
  - Parameters: none
- `app_init(cmdline cmdl, void** state) -> app_result`
  - What it does: Initializes app init
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
    - `state` (`void**`): Configuration value controlling behavior
- `app_update(void* state) -> app_result`
  - What it does: Performs app update
  - Parameters:
    - `state` (`void*`): Configuration value controlling behavior
- `app_quit(void* state, app_result result) -> void`
  - What it does: Shuts down app quit
  - Parameters:
    - `state` (`void*`): Configuration value controlling behavior
    - `result` (`app_result`): Input parameter
- `run(cmdline cmdl) -> b32`
  - What it does: Performs run
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
- `mod_init() -> dll_export b32`
  - What it does: Initializes mod init
  - Parameters: none
- `mod_quit() -> dll_export void`
  - What it does: Shuts down mod quit
  - Parameters: none

### `basic/entry_impl.h`

- Types: **3**
- Functions: **0**

#### Types

- `HINSTANCE` (alias)
  - What it represents: Type alias used for `HINSTANCE`
  - Members: none (alias/function type)
- `LPSTR` (alias)
  - What it represents: Type alias used for `LPSTR`
  - Members: none (alias/function type)
- `PWSTR` (alias)
  - What it represents: Type alias used for `PWSTR`
  - Members: none (alias/function type)

#### Functions

- None

### `basic/env_defines.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `basic/intrinsics.h`

- Types: **0**
- Functions: **15**

#### Types

- None

#### Functions

- `popcount_u32(u32 val) -> i32`
  - What it does: Performs popcount u32
  - Parameters:
    - `val` (`u32`): Input parameter
- `popcount_u64(u64 val) -> i32`
  - What it does: Performs popcount u64
  - Parameters:
    - `val` (`u64`): Input parameter
- `ctz_u32(u32 val) -> i32`
  - What it does: Performs ctz u32
  - Parameters:
    - `val` (`u32`): Input parameter
- `ctz_u64(u64 val) -> i32`
  - What it does: Performs ctz u64
  - Parameters:
    - `val` (`u64`): Input parameter
- `clz_u32(u32 val) -> i32`
  - What it does: Performs clz u32
  - Parameters:
    - `val` (`u32`): Input parameter
- `clz_u64(u64 val) -> i32`
  - What it does: Performs clz u64
  - Parameters:
    - `val` (`u64`): Input parameter
- `bsr_u32(u32 val) -> i32`
  - What it does: Performs bsr u32
  - Parameters:
    - `val` (`u32`): Input parameter
- `bsr_u64(u64 val) -> i32`
  - What it does: Performs bsr u64
  - Parameters:
    - `val` (`u64`): Input parameter
- `bswap_u16(u16 val) -> u16`
  - What it does: Performs bswap u16
  - Parameters:
    - `val` (`u16`): Input parameter
- `bswap_u32(u32 val) -> u32`
  - What it does: Performs bswap u32
  - Parameters:
    - `val` (`u32`): Input parameter
- `bswap_u64(u64 val) -> u64`
  - What it does: Performs bswap u64
  - Parameters:
    - `val` (`u64`): Input parameter
- `rotl_u32(u32 val, i32 cnt) -> u32`
  - What it does: Performs rotl u32
  - Parameters:
    - `val` (`u32`): Input parameter
    - `cnt` (`i32`): Input parameter
- `rotl_u64(u64 val, i32 cnt) -> u64`
  - What it does: Performs rotl u64
  - Parameters:
    - `val` (`u64`): Input parameter
    - `cnt` (`i32`): Input parameter
- `rotr_u32(u32 val, i32 cnt) -> u32`
  - What it does: Performs rotr u32
  - Parameters:
    - `val` (`u32`): Input parameter
    - `cnt` (`i32`): Input parameter
- `rotr_u64(u64 val, i32 cnt) -> u64`
  - What it does: Performs rotr u64
  - Parameters:
    - `val` (`u64`): Input parameter
    - `cnt` (`i32`): Input parameter

### `basic/keyword_defines.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `basic/primitive_types.h`

- Types: **36**
- Functions: **0**

#### Types

- `i8` (alias)
  - What it represents: Type alias used for `i8`
  - Members: none (alias/function type)
- `u8` (alias)
  - What it represents: Type alias used for `u8`
  - Members: none (alias/function type)
- `i16` (alias)
  - What it represents: Type alias used for `i16`
  - Members: none (alias/function type)
- `u16` (alias)
  - What it represents: Type alias used for `u16`
  - Members: none (alias/function type)
- `i32` (alias)
  - What it represents: Type alias used for `i32`
  - Members: none (alias/function type)
- `u32` (alias)
  - What it represents: Type alias used for `u32`
  - Members: none (alias/function type)
- `i64` (alias)
  - What it represents: Type alias used for `i64`
  - Members: none (alias/function type)
- `u64` (alias)
  - What it represents: Type alias used for `u64`
  - Members: none (alias/function type)
- `f32` (alias)
  - What it represents: Type alias used for `f32`
  - Members: none (alias/function type)
- `f64` (alias)
  - What it represents: Type alias used for `f64`
  - Members: none (alias/function type)
- `b8` (alias)
  - What it represents: Type alias used for `b8`
  - Members: none (alias/function type)
- `b16` (alias)
  - What it represents: Type alias used for `b16`
  - Members: none (alias/function type)
- `b32` (alias)
  - What it represents: Type alias used for `b32`
  - Members: none (alias/function type)
- `b64` (alias)
  - What it represents: Type alias used for `b64`
  - Members: none (alias/function type)
- `c8` (alias)
  - What it represents: Type alias used for `c8`
  - Members: none (alias/function type)
- `c16` (alias)
  - What it represents: Type alias used for `c16`
  - Members: none (alias/function type)
- `c32` (alias)
  - What it represents: Type alias used for `c32`
  - Members: none (alias/function type)
- `cstr8` (alias)
  - What it represents: Type alias used for `cstr8`
  - Members: none (alias/function type)
- `cstr16` (alias)
  - What it represents: Type alias used for `cstr16`
  - Members: none (alias/function type)
- `cstr32` (alias)
  - What it represents: Type alias used for `cstr32`
  - Members: none (alias/function type)
- `i8x` (alias)
  - What it represents: Type alias used for `i8x`
  - Members: none (alias/function type)
- `u8x` (alias)
  - What it represents: Type alias used for `u8x`
  - Members: none (alias/function type)
- `i16x` (alias)
  - What it represents: Type alias used for `i16x`
  - Members: none (alias/function type)
- `u16x` (alias)
  - What it represents: Type alias used for `u16x`
  - Members: none (alias/function type)
- `i32x` (alias)
  - What it represents: Type alias used for `i32x`
  - Members: none (alias/function type)
- `u32x` (alias)
  - What it represents: Type alias used for `u32x`
  - Members: none (alias/function type)
- `i64x` (alias)
  - What it represents: Type alias used for `i64x`
  - Members: none (alias/function type)
- `u64x` (alias)
  - What it represents: Type alias used for `u64x`
  - Members: none (alias/function type)
- `b8x` (alias)
  - What it represents: Type alias used for `b8x`
  - Members: none (alias/function type)
- `b16x` (alias)
  - What it represents: Type alias used for `b16x`
  - Members: none (alias/function type)
- `b32x` (alias)
  - What it represents: Type alias used for `b32x`
  - Members: none (alias/function type)
- `b64x` (alias)
  - What it represents: Type alias used for `b64x`
  - Members: none (alias/function type)
- `sz` (alias)
  - What it represents: Type alias used for `sz`
  - Members: none (alias/function type)
- `up` (alias)
  - What it represents: Type alias used for `up`
  - Members: none (alias/function type)
- `sp` (alias)
  - What it represents: Type alias used for `sp`
  - Members: none (alias/function type)
- `dp` (alias)
  - What it represents: Type alias used for `dp`
  - Members: none (alias/function type)

#### Functions

- None

### `basic/profiler.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `basic/safe.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `basic/third_party.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `basic/utility_defines.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

## containers

Intrusive containers and sorting helpers for lightweight data structures.

### `containers/binary_tree.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `containers/bitset.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `containers/doubly_list.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `containers/hash_map.h`

- Types: **3**
- Functions: **13**

#### Types

- `hash_map_slot` (struct)
  - What it represents: Represents `hash map slot` data grouped in a struct
  - Members:
    - `key`: `u64`
    - `value`: `void*`
    - `probe_dist`: `u32`
    - `occupied`: `b32`
- `hash_map` (struct)
  - What it represents: Represents `hash map` data grouped in a struct
  - Members:
    - `slots`: `hash_map_slot*`
    - `count`: `sz`
    - `cap`: `sz`
    - `alloc`: `allocator`
- `hash_map_iter` (alias)
  - What it represents: Type alias used for `hash map iter`
  - Members: none (alias/function type)

#### Functions

- `hash_map_create(sz cap, allocator alloc) -> hash_map`
  - What it does: Creates hash map create resources or state
  - Parameters:
    - `cap` (`sz`): Input parameter
    - `alloc` (`allocator`): Allocator/context used for memory management
- `hash_map_destroy(hash_map* map) -> void`
  - What it does: Destroys hash map destroy resources or state
  - Parameters:
    - `map` (`hash_map*`): Input parameter
- `hash_map_clear(hash_map* map) -> void`
  - What it does: Performs hash map clear
  - Parameters:
    - `map` (`hash_map*`): Input parameter
- `hash_map_count(hash_map const* map) -> sz`
  - What it does: Performs hash map count
  - Parameters:
    - `map` (`hash_map const*`): Input parameter
- `hash_map_capacity(hash_map const* map) -> sz`
  - What it does: Performs hash map capacity
  - Parameters:
    - `map` (`hash_map const*`): Input parameter
- `hash_map_load_factor(hash_map const* map) -> f32`
  - What it does: Performs hash map load factor
  - Parameters:
    - `map` (`hash_map const*`): Input parameter
- `hash_map_rehash(hash_map* map, sz new_cap) -> b32`
  - What it does: Performs hash map rehash
  - Parameters:
    - `map` (`hash_map*`): Input parameter
    - `new_cap` (`sz`): Input parameter
- `hash_map_reserve(hash_map* map, sz min_cap) -> b32`
  - What it does: Performs hash map reserve
  - Parameters:
    - `map` (`hash_map*`): Input parameter
    - `min_cap` (`sz`): Input parameter
- `hash_map_set(hash_map* map, u64 key, void* value) -> b32`
  - What it does: Performs hash map set
  - Parameters:
    - `map` (`hash_map*`): Input parameter
    - `key` (`u64`): Input parameter
    - `value` (`void*`): Input parameter
- `hash_map_get(hash_map* map, u64 key) -> void*`
  - What it does: Performs hash map get
  - Parameters:
    - `map` (`hash_map*`): Input parameter
    - `key` (`u64`): Input parameter
- `hash_map_has(hash_map* map, u64 key) -> b32`
  - What it does: Performs hash map has
  - Parameters:
    - `map` (`hash_map*`): Input parameter
    - `key` (`u64`): Input parameter
- `hash_map_remove(hash_map* map, u64 key) -> b32`
  - What it does: Removes hash map remove
  - Parameters:
    - `map` (`hash_map*`): Input parameter
    - `key` (`u64`): Input parameter
- `hash_map_next(hash_map* map, hash_map_iter* iter) -> hash_map_slot*`
  - What it does: Performs hash map next
  - Parameters:
    - `map` (`hash_map*`): Input parameter
    - `iter` (`hash_map_iter*`): Input parameter

### `containers/ring_list.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `containers/singly_list.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `containers/sort.h`

- Types: **1**
- Functions: **8**

#### Types

- `sort_compare_fn` (alias)
  - What it represents: Type alias used for `sort compare fn`
  - Members: none (alias/function type)

#### Functions

- `sort_check(const void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data) -> b32`
  - What it does: Performs sort check
  - Parameters:
    - `ptr` (`const void*`): Input data used by the operation
    - `elem_count` (`sz`): Number of elements/items involved
    - `elem_size` (`sz`): Size or capacity value used by the operation
    - `compare` (`sort_compare_fn*`): Input parameter
    - `user_data` (`void*`): Input parameter
- `sort_bubble(void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data) -> sz`
  - What it does: Performs sort bubble
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `elem_count` (`sz`): Number of elements/items involved
    - `elem_size` (`sz`): Size or capacity value used by the operation
    - `compare` (`sort_compare_fn*`): Input parameter
    - `user_data` (`void*`): Input parameter
- `sort_quick(void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data, allocator allocator) -> sz`
  - What it does: Performs sort quick
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `elem_count` (`sz`): Number of elements/items involved
    - `elem_size` (`sz`): Size or capacity value used by the operation
    - `compare` (`sort_compare_fn*`): Input parameter
    - `user_data` (`void*`): Input parameter
    - `allocator` (`allocator`): Allocator/context used for memory management
- `sort_merge(void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data, allocator allocator) -> sz`
  - What it does: Performs sort merge
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `elem_count` (`sz`): Number of elements/items involved
    - `elem_size` (`sz`): Size or capacity value used by the operation
    - `compare` (`sort_compare_fn*`): Input parameter
    - `user_data` (`void*`): Input parameter
    - `allocator` (`allocator`): Allocator/context used for memory management
- `sort_selection(void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data) -> sz`
  - What it does: Performs sort selection
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `elem_count` (`sz`): Number of elements/items involved
    - `elem_size` (`sz`): Size or capacity value used by the operation
    - `compare` (`sort_compare_fn*`): Input parameter
    - `user_data` (`void*`): Input parameter
- `sort_insertion(void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data) -> sz`
  - What it does: Performs sort insertion
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `elem_count` (`sz`): Number of elements/items involved
    - `elem_size` (`sz`): Size or capacity value used by the operation
    - `compare` (`sort_compare_fn*`): Input parameter
    - `user_data` (`void*`): Input parameter
- `sort_radix32(u32* ptr, sz elem_count) -> sz`
  - What it does: Performs sort radix32
  - Parameters:
    - `ptr` (`u32*`): Input data used by the operation
    - `elem_count` (`sz`): Number of elements/items involved
- `sort_radix64(u64* ptr, sz elem_count) -> sz`
  - What it does: Performs sort radix64
  - Parameters:
    - `ptr` (`u64*`): Input data used by the operation
    - `elem_count` (`sz`): Number of elements/items involved

### `containers/stack_list.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

### `containers/tree.h`

- Types: **0**
- Functions: **0**

#### Types

- None

#### Functions

- None

## context

Global/thread context setup, allocators and log state access.

### `context/ctx.h`

- Types: **3**
- Functions: **15**

#### Types

- `ctx_user_data_idx` (alias)
  - What it represents: Type alias used for `ctx user data idx`
  - Members: none (alias/function type)
- `ctx_setup` (struct)
  - What it represents: Represents `ctx setup` data grouped in a struct
  - Members:
    - `main_allocator`: `allocator`
    - `mutex_handle`: `mutex`
    - `use_arena_allocs`: `b32`
    - `use_heap_allocs`: `b32`
    - `use_temp_allocs`: `b32`
    - `perm_arena_block_size`: `sz`
    - `temp_arena_block_size`: `sz`
    - `perm_heap_block_size`: `sz`
    - `temp_heap_block_size`: `sz`
- `ctx` (struct)
  - What it represents: Represents `ctx` data grouped in a struct
  - Members:
    - `is_init`: `b32`
    - `setup`: `ctx_setup`
    - `perm_arena`: `arena`
    - `temp_arena`: `arena`
    - `perm_heap`: `heap`
    - `temp_heap`: `heap`
    - `log`: `log_state`
    - `user_data`: `void* [CTX_USER_DATA_COUNT]`

#### Functions

- `ctx_setup_is_valid(ctx_setup* setup) -> b32`
  - What it does: Performs ctx setup is valid
  - Parameters:
    - `setup` (`ctx_setup*`): Input parameter
- `ctx_setup_fill_defaults(ctx_setup* setup) -> void`
  - What it does: Performs ctx setup fill defaults
  - Parameters:
    - `setup` (`ctx_setup*`): Input parameter
- `ctx_init(ctx* context, ctx_setup setup) -> b32`
  - What it does: Initializes ctx init
  - Parameters:
    - `context` (`ctx*`): Input parameter
    - `setup` (`ctx_setup`): Input parameter
- `ctx_quit(ctx* context) -> b32`
  - What it does: Shuts down ctx quit
  - Parameters:
    - `context` (`ctx*`): Input parameter
- `ctx_is_init(ctx* context) -> b32`
  - What it does: Initializes ctx is init
  - Parameters:
    - `context` (`ctx*`): Input parameter
- `ctx_get_allocator(ctx* context) -> allocator`
  - What it does: Gets ctx get allocator
  - Parameters:
    - `context` (`ctx*`): Input parameter
- `ctx_get_setup(ctx* context) -> ctx_setup`
  - What it does: Gets ctx get setup
  - Parameters:
    - `context` (`ctx*`): Input parameter
- `ctx_get_log_state(ctx* context) -> log_state*`
  - What it does: Gets ctx get log state
  - Parameters:
    - `context` (`ctx*`): Input parameter
- `ctx_get_perm_arena(ctx* context) -> arena*`
  - What it does: Gets ctx get perm arena
  - Parameters:
    - `context` (`ctx*`): Input parameter
- `ctx_get_temp_arena(ctx* context) -> arena*`
  - What it does: Gets ctx get temp arena
  - Parameters:
    - `context` (`ctx*`): Input parameter
- `ctx_get_perm_heap(ctx* context) -> heap*`
  - What it does: Gets ctx get perm heap
  - Parameters:
    - `context` (`ctx*`): Input parameter
- `ctx_get_temp_heap(ctx* context) -> heap*`
  - What it does: Gets ctx get temp heap
  - Parameters:
    - `context` (`ctx*`): Input parameter
- `ctx_get_user_data(ctx* context, ctx_user_data_idx idx) -> void*`
  - What it does: Gets ctx get user data
  - Parameters:
    - `context` (`ctx*`): Input parameter
    - `idx` (`ctx_user_data_idx`): Zero-based index used to select an item
- `ctx_set_user_data(ctx* context, ctx_user_data_idx idx, void* user_data) -> b32`
  - What it does: Sets ctx set user data
  - Parameters:
    - `context` (`ctx*`): Input parameter
    - `idx` (`ctx_user_data_idx`): Zero-based index used to select an item
    - `user_data` (`void*`): Input parameter
- `ctx_clear_temp(ctx* context) -> void`
  - What it does: Performs ctx clear temp
  - Parameters:
    - `context` (`ctx*`): Input parameter

### `context/global_ctx.h`

- Types: **0**
- Functions: **23**

#### Types

- None

#### Functions

- `global_ctx_init(ctx_setup setup) -> b32`
  - What it does: Initializes global ctx init
  - Parameters:
    - `setup` (`ctx_setup`): Input parameter
- `global_ctx_quit() -> b32`
  - What it does: Shuts down global ctx quit
  - Parameters: none
- `global_ctx_is_init() -> b32`
  - What it does: Initializes global ctx is init
  - Parameters: none
- `global_ctx_get() -> ctx*`
  - What it does: Performs global ctx get
  - Parameters: none
- `global_ctx_lock() -> void`
  - What it does: Performs global ctx lock
  - Parameters: none
- `global_ctx_unlock() -> void`
  - What it does: Performs global ctx unlock
  - Parameters: none
- `global_get_allocator() -> allocator`
  - What it does: Gets global get allocator
  - Parameters: none
- `global_get_setup() -> ctx_setup`
  - What it does: Gets global get setup
  - Parameters: none
- `global_get_main_allocator() -> allocator`
  - What it does: Gets global get main allocator
  - Parameters: none
- `global_get_log_state() -> log_state*`
  - What it does: Gets global get log state
  - Parameters: none
- `global_get_perm_arena() -> arena*`
  - What it does: Gets global get perm arena
  - Parameters: none
- `global_get_temp_arena() -> arena*`
  - What it does: Gets global get temp arena
  - Parameters: none
- `global_get_perm_heap() -> heap*`
  - What it does: Gets global get perm heap
  - Parameters: none
- `global_get_temp_heap() -> heap*`
  - What it does: Gets global get temp heap
  - Parameters: none
- `global_has_user_data_access(ctx_user_data_idx idx) -> b32`
  - What it does: Performs global has user data access
  - Parameters:
    - `idx` (`ctx_user_data_idx`): Zero-based index used to select an item
- `global_get_user_data(ctx_user_data_idx idx) -> void*`
  - What it does: Gets global get user data
  - Parameters:
    - `idx` (`ctx_user_data_idx`): Zero-based index used to select an item
- `global_set_user_data(ctx_user_data_idx idx, void* user_data) -> b32`
  - What it does: Sets global set user data
  - Parameters:
    - `idx` (`ctx_user_data_idx`): Zero-based index used to select an item
    - `user_data` (`void*`): Input parameter
- `global_set_user_data_access(ctx_user_data_idx idx, b8 has_access) -> b32`
  - What it does: Sets global set user data access
  - Parameters:
    - `idx` (`ctx_user_data_idx`): Zero-based index used to select an item
    - `has_access` (`b8`): Input parameter
- `global_set_user_data_access_all(b8 has_access) -> void`
  - What it does: Sets global set user data access all
  - Parameters:
    - `has_access` (`b8`): Input parameter
- `global_clear_temp() -> void`
  - What it does: Performs global clear temp
  - Parameters: none
- `global_log_set_level(log_level level) -> void`
  - What it does: Sets global log set level
  - Parameters:
    - `level` (`log_level`): Configuration value controlling behavior
- `global_log_begin_frame() -> void`
  - What it does: Begins global log begin frame
  - Parameters: none
- `global_log_end_frame(u32 severity_mask) -> log_frame*`
  - What it does: Ends global log end frame
  - Parameters:
    - `severity_mask` (`u32`): Input parameter

### `context/thread_ctx.h`

- Types: **0**
- Functions: **18**

#### Types

- None

#### Functions

- `thread_ctx_get() -> ctx*`
  - What it does: Performs thread ctx get
  - Parameters: none
- `thread_ctx_is_init() -> b32`
  - What it does: Initializes thread ctx is init
  - Parameters: none
- `thread_ctx_init(ctx_setup setup) -> b32`
  - What it does: Initializes thread ctx init
  - Parameters:
    - `setup` (`ctx_setup`): Input parameter
- `thread_ctx_quit() -> b32`
  - What it does: Shuts down thread ctx quit
  - Parameters: none
- `thread_get_allocator() -> allocator`
  - What it does: Gets thread get allocator
  - Parameters: none
- `thread_get_setup() -> ctx_setup`
  - What it does: Gets thread get setup
  - Parameters: none
- `thread_get_log_state() -> log_state*`
  - What it does: Gets thread get log state
  - Parameters: none
- `thread_get_perm_arena() -> arena*`
  - What it does: Gets thread get perm arena
  - Parameters: none
- `thread_get_temp_arena() -> arena*`
  - What it does: Gets thread get temp arena
  - Parameters: none
- `thread_get_perm_heap() -> heap*`
  - What it does: Gets thread get perm heap
  - Parameters: none
- `thread_get_temp_heap() -> heap*`
  - What it does: Gets thread get temp heap
  - Parameters: none
- `thread_get_user_data(ctx_user_data_idx idx) -> void*`
  - What it does: Gets thread get user data
  - Parameters:
    - `idx` (`ctx_user_data_idx`): Zero-based index used to select an item
- `thread_set_user_data(ctx_user_data_idx idx, void* user_data) -> b32`
  - What it does: Sets thread set user data
  - Parameters:
    - `idx` (`ctx_user_data_idx`): Zero-based index used to select an item
    - `user_data` (`void*`): Input parameter
- `thread_clear_temp() -> void`
  - What it does: Performs thread clear temp
  - Parameters: none
- `thread_log_set_level(log_level level) -> void`
  - What it does: Sets thread log set level
  - Parameters:
    - `level` (`log_level`): Configuration value controlling behavior
- `thread_log_sync() -> b32`
  - What it does: Performs thread log sync
  - Parameters: none
- `thread_log_begin_frame() -> void`
  - What it does: Begins thread log begin frame
  - Parameters: none
- `thread_log_end_frame(u32 severity_mask) -> log_frame*`
  - What it does: Ends thread log end frame
  - Parameters:
    - `severity_mask` (`u32`): Input parameter

## filesystem

Paths, files, streams, archives, modules and filesystem monitoring.

### `filesystem/archive.h`

- Types: **4**
- Functions: **14**

#### Types

- `archive_entry` (struct)
  - What it represents: Represents `archive entry` data grouped in a struct
  - Members:
    - `item_path`: `path`
    - `data_ptr`: `u8*`
    - `data_size`: `sz`
    - `data_capacity`: `sz`
    - `is_directory`: `b32`
- `archive` (struct)
  - What it represents: Represents `archive` data grouped in a struct
  - Members:
    - `entries`: `archive_entry*`
    - `entry_count`: `sz`
    - `entry_capacity`: `sz`
- `archive_entry_info` (struct)
  - What it represents: Represents `archive entry info` data grouped in a struct
  - Members:
    - `item_path`: `path`
    - `data_size`: `sz`
    - `is_directory`: `b32`
- `archive_iterate_callback` (alias)
  - What it represents: Type alias used for `archive iterate callback`
  - Members: none (alias/function type)

#### Functions

- `_archive_create(callsite site) -> archive`
  - What it does: Creates  archive create resources or state
  - Parameters:
    - `site` (`callsite`): Input parameter
- `_archive_destroy(archive* arc, callsite site) -> void`
  - What it does: Destroys  archive destroy resources or state
  - Parameters:
    - `arc` (`archive*`): Input parameter
    - `site` (`callsite`): Input parameter
- `archive_clear(archive* arc) -> void`
  - What it does: Performs archive clear
  - Parameters:
    - `arc` (`archive*`): Input parameter
- `archive_count(const archive* arc) -> sz`
  - What it does: Performs archive count
  - Parameters:
    - `arc` (`const archive*`): Input parameter
- `archive_exists(const archive* arc, const path* src) -> b32`
  - What it does: Performs archive exists
  - Parameters:
    - `arc` (`const archive*`): Input parameter
    - `src` (`const path*`): Input data used by the operation
- `archive_load_file(archive* arc, const path* src) -> b32`
  - What it does: Performs archive load file
  - Parameters:
    - `arc` (`archive*`): Input parameter
    - `src` (`const path*`): Input data used by the operation
- `archive_save_file(const archive* arc, const path* dst) -> b32`
  - What it does: Performs archive save file
  - Parameters:
    - `arc` (`const archive*`): Input parameter
    - `dst` (`const path*`): Destination storage for output data
- `archive_write_all(archive* arc, const path* src, buffer data) -> b32`
  - What it does: Writes data for archive write all
  - Parameters:
    - `arc` (`archive*`): Input parameter
    - `src` (`const path*`): Input data used by the operation
    - `data` (`buffer`): Input data used by the operation
- `archive_remove(archive* arc, const path* src) -> b32`
  - What it does: Removes archive remove
  - Parameters:
    - `arc` (`archive*`): Input parameter
    - `src` (`const path*`): Input data used by the operation
- `archive_read_all(const archive* arc, const path* src, allocator* alloc, buffer* out_data) -> b32`
  - What it does: Reads data for archive read all
  - Parameters:
    - `arc` (`const archive*`): Input parameter
    - `src` (`const path*`): Input data used by the operation
    - `alloc` (`allocator*`): Allocator/context used for memory management
    - `out_data` (`buffer*`): Output value written by the function
- `archive_add_file(archive* arc, const path* archive_path, const path* disk_path) -> b32`
  - What it does: Adds archive add file
  - Parameters:
    - `arc` (`archive*`): Input parameter
    - `archive_path` (`const path*`): Path used by the operation
    - `disk_path` (`const path*`): Path used by the operation
- `archive_iterate(const archive* arc, archive_iterate_callback* callback, void* user_data) -> b32`
  - What it does: Performs archive iterate
  - Parameters:
    - `arc` (`const archive*`): Input parameter
    - `callback` (`archive_iterate_callback*`): Input parameter
    - `user_data` (`void*`): Input parameter
- `archive_get_entry_info(const archive* arc, const path* src, archive_entry_info* out_info) -> b32`
  - What it does: Gets archive get entry info
  - Parameters:
    - `arc` (`const archive*`): Input parameter
    - `src` (`const path*`): Input data used by the operation
    - `out_info` (`archive_entry_info*`): Output value written by the function
- `archive_get_entry_data(const archive* arc, const path* src, buffer* out_data) -> b32`
  - What it does: Gets archive get entry data
  - Parameters:
    - `arc` (`const archive*`): Input parameter
    - `src` (`const path*`): Input data used by the operation
    - `out_data` (`buffer*`): Output value written by the function

### `filesystem/directory.h`

- Types: **3**
- Functions: **12**

#### Types

- `dir_entry` (struct)
  - What it represents: Represents `dir entry` data grouped in a struct
  - Members:
    - `full_path`: `path`
    - `relative_path`: `path`
    - `is_directory`: `b32`
- `dir_system_path` (enum)
  - What it represents: Represents a set of named constants for `dir system path`
  - Members:
    - `DIR_SYSTEM_PATH_HOME`: `enum value`
    - `DIR_SYSTEM_PATH_DESKTOP`: `enum value`
    - `DIR_SYSTEM_PATH_DOCUMENTS`: `enum value`
    - `DIR_SYSTEM_PATH_DOWNLOADS`: `enum value`
    - `DIR_SYSTEM_PATH_MUSIC`: `enum value`
    - `DIR_SYSTEM_PATH_PICTURES`: `enum value`
    - `DIR_SYSTEM_PATH_PUBLICSHARE`: `enum value`
    - `DIR_SYSTEM_PATH_SAVEDGAMES`: `enum value`
    - `DIR_SYSTEM_PATH_SCREENSHOTS`: `enum value`
    - `DIR_SYSTEM_PATH_TEMPLATES`: `enum value`
    - `DIR_SYSTEM_PATH_VIDEOS`: `enum value`
    - `DIR_SYSTEM_PATH_COUNT`: `enum value`
- `dir_iterate_callback` (alias)
  - What it represents: Type alias used for `dir iterate callback`
  - Members: none (alias/function type)

#### Functions

- `dir_get_base() -> path`
  - What it does: Gets dir get base
  - Parameters: none
- `dir_get_pref(cstr8 org_name, cstr8 app_name) -> path`
  - What it does: Gets dir get pref
  - Parameters:
    - `org_name` (`cstr8`): Input parameter
    - `app_name` (`cstr8`): Input parameter
- `dir_get_system(dir_system_path location) -> path`
  - What it does: Gets dir get system
  - Parameters:
    - `location` (`dir_system_path`): Input parameter
- `dir_create(const path* src) -> b32`
  - What it does: Creates dir create resources or state
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `dir_remove(const path* src) -> b32`
  - What it does: Removes dir remove
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `dir_rename(const path* old_src, const path* new_src) -> b32`
  - What it does: Performs dir rename
  - Parameters:
    - `old_src` (`const path*`): Input parameter
    - `new_src` (`const path*`): Input parameter
- `dir_cpy_recursive(const path* src, const path* dst, b32 overwrite_existing) -> b32`
  - What it does: Performs dir cpy recursive
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `dst` (`const path*`): Destination storage for output data
    - `overwrite_existing` (`b32`): Input parameter
- `dir_exists(const path* src) -> b32`
  - What it does: Performs dir exists
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `dir_create_recursive(const path* src) -> b32`
  - What it does: Creates dir create recursive resources or state
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `dir_remove_recursive(const path* src) -> b32`
  - What it does: Removes dir remove recursive
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `dir_iterate(const path* src, dir_iterate_callback* callback, void* user_data) -> b32`
  - What it does: Performs dir iterate
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `callback` (`dir_iterate_callback*`): Input parameter
    - `user_data` (`void*`): Input parameter
- `dir_iterate_recursive(const path* src, dir_iterate_callback* callback, void* user_data) -> b32`
  - What it does: Performs dir iterate recursive
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `callback` (`dir_iterate_callback*`): Input parameter
    - `user_data` (`void*`): Input parameter

### `filesystem/file.h`

- Types: **0**
- Functions: **10**

#### Types

- None

#### Functions

- `file_create(const path* src) -> b32`
  - What it does: Creates file create resources or state
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `file_delete(const path* src) -> b32`
  - What it does: Performs file delete
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `file_rename(const path* old_src, const path* new_src) -> b32`
  - What it does: Performs file rename
  - Parameters:
    - `old_src` (`const path*`): Input parameter
    - `new_src` (`const path*`): Input parameter
- `file_cpy(const path* src, const path* dst, b32 overwrite_existing) -> b32`
  - What it does: Performs file cpy
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `dst` (`const path*`): Destination storage for output data
    - `overwrite_existing` (`b32`): Input parameter
- `file_exists(const path* src) -> b32`
  - What it does: Performs file exists
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `file_get_size(const path* src, sz* out_size) -> b32`
  - What it does: Gets file get size
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `out_size` (`sz*`): Output value written by the function
- `file_read_all(const path* src, allocator* alloc, buffer* out_data) -> b32`
  - What it does: Reads data for file read all
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `alloc` (`allocator*`): Allocator/context used for memory management
    - `out_data` (`buffer*`): Output value written by the function
- `file_write_all(const path* src, buffer data) -> b32`
  - What it does: Writes data for file write all
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `data` (`buffer`): Input data used by the operation
- `file_append_all(const path* src, buffer data) -> b32`
  - What it does: Performs file append all
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `data` (`buffer`): Input data used by the operation
- `file_write_all_atomic(const path* src, buffer data) -> b32`
  - What it does: Writes data for file write all atomic
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `data` (`buffer`): Input data used by the operation

### `filesystem/filemap.h`

- Types: **3**
- Functions: **7**

#### Types

- `filemap_access` (enum)
  - What it represents: Represents a set of named constants for `filemap access`
  - Members:
    - `FILEMAP_ACCESS_READ`: `enum value`
    - `FILEMAP_ACCESS_READ_WRITE`: `enum value`
    - `FILEMAP_ACCESS_COPY_ON_WRITE`: `enum value`
- `filemap_error` (enum)
  - What it represents: Represents a set of named constants for `filemap error`
  - Members:
    - `FILEMAP_ERROR_NONE`: `enum value`
    - `FILEMAP_ERROR_INVALID_ARGUMENT`: `enum value`
    - `FILEMAP_ERROR_OPEN_FAILED`: `enum value`
    - `FILEMAP_ERROR_MAP_FAILED`: `enum value`
    - `FILEMAP_ERROR_IO_FAILED`: `enum value`
- `filemap` (struct)
  - What it represents: Represents `filemap` data grouped in a struct
  - Members:
    - `data_ptr`: `void*`
    - `data_size`: `sz`
    - `native_file`: `void*`
    - `native_mapping`: `void*`
    - `writable`: `b32`
    - `dirty`: `b32`
    - `uses_fallback_cpy`: `b32`
    - `source_path`: `path`

#### Functions

- `_filemap_open(const path* src, filemap_access access, callsite site) -> filemap`
  - What it does: Opens  filemap open
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `access` (`filemap_access`): Input parameter
    - `site` (`callsite`): Input parameter
- `filemap_is_open(const filemap* map) -> b32`
  - What it does: Opens filemap is open
  - Parameters:
    - `map` (`const filemap*`): Input parameter
- `filemap_flush(filemap* map) -> b32`
  - What it does: Performs filemap flush
  - Parameters:
    - `map` (`filemap*`): Input parameter
- `_filemap_close(filemap* map, callsite site) -> void`
  - What it does: Closes  filemap close
  - Parameters:
    - `map` (`filemap*`): Input parameter
    - `site` (`callsite`): Input parameter
- `filemap_is_writable(const filemap* map) -> b32`
  - What it does: Performs filemap is writable
  - Parameters:
    - `map` (`const filemap*`): Input parameter
- `filemap_mark_dirty(filemap* map) -> void`
  - What it does: Performs filemap mark dirty
  - Parameters:
    - `map` (`filemap*`): Input parameter
- `filemap_get_last_error() -> filemap_error`
  - What it does: Gets filemap get last error
  - Parameters: none

### `filesystem/filestream.h`

- Types: **5**
- Functions: **14**

#### Types

- `archive` (alias)
  - What it represents: Type alias used for `archive`
  - Members: none (alias/function type)
- `filestream_kind` (enum)
  - What it represents: Represents a set of named constants for `filestream kind`
  - Members:
    - `FILESTREAM_KIND_NONE`: `enum value`
    - `FILESTREAM_KIND_NATIVE`: `enum value`
    - `FILESTREAM_KIND_ARCHIVE`: `enum value`
- `filestream_seek_basis` (enum)
  - What it represents: Represents a set of named constants for `filestream seek basis`
  - Members:
    - `FILESTREAM_SEEK_BASIS_BEGIN`: `enum value`
    - `FILESTREAM_SEEK_BASIS_CURRENT`: `enum value`
    - `FILESTREAM_SEEK_BASIS_END`: `enum value`
- `filestream_error` (enum)
  - What it represents: Represents a set of named constants for `filestream error`
  - Members:
    - `FILESTREAM_ERROR_NONE`: `enum value`
    - `FILESTREAM_ERROR_NOT_OPEN`: `enum value`
    - `FILESTREAM_ERROR_ACCESS`: `enum value`
    - `FILESTREAM_ERROR_EOF`: `enum value`
    - `FILESTREAM_ERROR_SEEK`: `enum value`
    - `FILESTREAM_ERROR_IO`: `enum value`
    - `FILESTREAM_ERROR_ALLOC`: `enum value`
- `filestream` (struct)
  - What it represents: Represents `filestream` data grouped in a struct
  - Members:
    - `kind`: `filestream_kind`
    - `mode_flags`: `u32`
    - `native_handle`: `void*`
    - `archive_ref`: `archive*`
    - `archive_path`: `path`
    - `memory_ptr`: `u8*`
    - `memory_size`: `sz`
    - `memory_capacity`: `sz`
    - `cursor`: `sz`
    - `dirty`: `b32`
    - `error_code`: `filestream_error`

#### Functions

- `_filestream_open(const path* src, u32 mode_flags, callsite site) -> filestream`
  - What it does: Opens  filestream open
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `mode_flags` (`u32`): Input parameter
    - `site` (`callsite`): Input parameter
- `_filestream_open_archive(archive* arc, const path* src, u32 mode_flags, callsite site) -> filestream`
  - What it does: Opens  filestream open archive
  - Parameters:
    - `arc` (`archive*`): Input parameter
    - `src` (`const path*`): Input data used by the operation
    - `mode_flags` (`u32`): Input parameter
    - `site` (`callsite`): Input parameter
- `_filestream_close(filestream* stm, callsite site) -> void`
  - What it does: Closes  filestream close
  - Parameters:
    - `stm` (`filestream*`): Input parameter
    - `site` (`callsite`): Input parameter
- `filestream_is_open(const filestream* stm) -> b32`
  - What it does: Opens filestream is open
  - Parameters:
    - `stm` (`const filestream*`): Input parameter
- `filestream_flush(filestream* stm) -> b32`
  - What it does: Performs filestream flush
  - Parameters:
    - `stm` (`filestream*`): Input parameter
- `filestream_read(filestream* stm, void* dst, sz size) -> sz`
  - What it does: Reads data for filestream read
  - Parameters:
    - `stm` (`filestream*`): Input parameter
    - `dst` (`void*`): Destination storage for output data
    - `size` (`sz`): Size or capacity value used by the operation
- `filestream_read_exact(filestream* stm, void* dst, sz size) -> b32`
  - What it does: Reads data for filestream read exact
  - Parameters:
    - `stm` (`filestream*`): Input parameter
    - `dst` (`void*`): Destination storage for output data
    - `size` (`sz`): Size or capacity value used by the operation
- `filestream_write(filestream* stm, const void* src, sz size) -> sz`
  - What it does: Writes data for filestream write
  - Parameters:
    - `stm` (`filestream*`): Input parameter
    - `src` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `filestream_write_exact(filestream* stm, const void* src, sz size) -> b32`
  - What it does: Writes data for filestream write exact
  - Parameters:
    - `stm` (`filestream*`): Input parameter
    - `src` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `filestream_seek(filestream* stm, i64 offset, filestream_seek_basis basis) -> b32`
  - What it does: Performs filestream seek
  - Parameters:
    - `stm` (`filestream*`): Input parameter
    - `offset` (`i64`): Input parameter
    - `basis` (`filestream_seek_basis`): Input parameter
- `filestream_tell(const filestream* stm) -> sz`
  - What it does: Performs filestream tell
  - Parameters:
    - `stm` (`const filestream*`): Input parameter
- `filestream_size(const filestream* stm) -> sz`
  - What it does: Performs filestream size
  - Parameters:
    - `stm` (`const filestream*`): Input parameter
- `filestream_eof(const filestream* stm) -> b32`
  - What it does: Performs filestream eof
  - Parameters:
    - `stm` (`const filestream*`): Input parameter
- `filestream_get_error(const filestream* stm) -> filestream_error`
  - What it does: Gets filestream get error
  - Parameters:
    - `stm` (`const filestream*`): Input parameter

### `filesystem/module.h`

- Types: **3**
- Functions: **5**

#### Types

- `mod_init_func` (alias)
  - What it represents: Type alias used for `mod init func`
  - Members: none (alias/function type)
- `mod_quit_func` (alias)
  - What it represents: Type alias used for `mod quit func`
  - Members: none (alias/function type)
- `mod` (struct)
  - What it represents: Represents `mod` data grouped in a struct
  - Members:
    - `native_handle`: `void*`
    - `init_func`: `mod_init_func*`
    - `quit_func`: `mod_quit_func*`
    - `source_path`: `path`
    - `initialized`: `b32`

#### Functions

- `mod_open(const path* src) -> mod`
  - What it does: Opens mod open
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `mod_is_open(const mod* mod_ptr) -> b32`
  - What it does: Opens mod is open
  - Parameters:
    - `mod_ptr` (`const mod*`): Input parameter
- `mod_get_func(const mod* mod_ptr, cstr8 name) -> void*`
  - What it does: Gets mod get func
  - Parameters:
    - `mod_ptr` (`const mod*`): Input parameter
    - `name` (`cstr8`): Input parameter
- `mod_get_extension() -> cstr8`
  - What it does: Gets mod get extension
  - Parameters: none
- `mod_close(mod* mod_ptr) -> void`
  - What it does: Closes mod close
  - Parameters:
    - `mod_ptr` (`mod*`): Input parameter

### `filesystem/path.h`

- Types: **1**
- Functions: **29**

#### Types

- `path` (struct)
  - What it represents: Represents `path` data grouped in a struct
  - Members:
    - `buf`: `str8_medium`

#### Functions

- `path_from_cstr(cstr8 src) -> path`
  - What it does: Performs path from cstr
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
- `path_from_str8(str8 src) -> path`
  - What it does: Performs path from str8
  - Parameters:
    - `src` (`str8`): Input data used by the operation
- `path_join(const path* lhs, const path* rhs) -> path`
  - What it does: Performs path join
  - Parameters:
    - `lhs` (`const path*`): Input data used by the operation
    - `rhs` (`const path*`): Input data used by the operation
- `path_join_cstr(const path* lhs, cstr8 rhs) -> path`
  - What it does: Performs path join cstr
  - Parameters:
    - `lhs` (`const path*`): Input data used by the operation
    - `rhs` (`cstr8`): Input data used by the operation
- `path_append(path* dst, const path* src) -> sz`
  - What it does: Performs path append
  - Parameters:
    - `dst` (`path*`): Destination storage for output data
    - `src` (`const path*`): Input data used by the operation
- `path_append_cstr(path* dst, cstr8 src) -> sz`
  - What it does: Performs path append cstr
  - Parameters:
    - `dst` (`path*`): Destination storage for output data
    - `src` (`cstr8`): Input data used by the operation
- `path_norm(path* src) -> void`
  - What it does: Performs path norm
  - Parameters:
    - `src` (`path*`): Input data used by the operation
- `path_norm_trimmed_cpy(const path* src) -> path`
  - What it does: Performs path norm trimmed cpy
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_cmd_normd(const path* lhs, const path* rhs) -> b32`
  - What it does: Performs path cmd normd
  - Parameters:
    - `lhs` (`const path*`): Input data used by the operation
    - `rhs` (`const path*`): Input data used by the operation
- `path_ends_with(const path* src, cstr8 suffix) -> b32`
  - What it does: Ends path ends with
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `suffix` (`cstr8`): Input parameter
- `path_is_absolute(const path* src) -> b32`
  - What it does: Performs path is absolute
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_is_relative(const path* src) -> b32`
  - What it does: Performs path is relative
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_remove_extension(path* src) -> void`
  - What it does: Removes path remove extension
  - Parameters:
    - `src` (`path*`): Input data used by the operation
- `path_remove_name(path* src) -> void`
  - What it does: Removes path remove name
  - Parameters:
    - `src` (`path*`): Input data used by the operation
- `path_remove_directory(path* src) -> void`
  - What it does: Removes path remove directory
  - Parameters:
    - `src` (`path*`): Input data used by the operation
- `path_remove_trailing_slash(path* src) -> void`
  - What it does: Removes path remove trailing slash
  - Parameters:
    - `src` (`path*`): Input data used by the operation
- `path_get_extension(const path* src) -> path`
  - What it does: Gets path get extension
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_get_name(const path* src) -> path`
  - What it does: Gets path get name
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_get_basename(const path* src) -> path`
  - What it does: Gets path get basename
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_get_directory(const path* src) -> path`
  - What it does: Gets path get directory
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_get_common(const path* src_list, sz path_count) -> path`
  - What it does: Gets path get common
  - Parameters:
    - `src_list` (`const path*`): Input parameter
    - `path_count` (`sz`): Number of elements/items involved
- `path_get_current() -> path`
  - What it does: Gets path get current
  - Parameters: none
- `path_set_current(const path* src) -> b32`
  - What it does: Sets path set current
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_resolve(const path* src) -> path`
  - What it does: Performs path resolve
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_make_relative(const path* src, const path* root) -> path`
  - What it does: Performs path make relative
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `root` (`const path*`): Input parameter
- `path_exists(const path* src) -> b32`
  - What it does: Performs path exists
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_remove(const path* src) -> b32`
  - What it does: Removes path remove
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
- `path_rename(const path* old_src, const path* new_src) -> b32`
  - What it does: Performs path rename
  - Parameters:
    - `old_src` (`const path*`): Input parameter
    - `new_src` (`const path*`): Input parameter
- `path_get_last_write_time(const path* src) -> timestamp`
  - What it does: Gets path get last write time
  - Parameters:
    - `src` (`const path*`): Input data used by the operation

### `filesystem/pathinfo.h`

- Types: **2**
- Functions: **1**

#### Types

- `pathinfo_type` (enum)
  - What it represents: Represents a set of named constants for `pathinfo type`
  - Members:
    - `PATHINFO_TYPE_NONE`: `enum value`
    - `PATHINFO_TYPE_FILE`: `enum value`
    - `PATHINFO_TYPE_DIRECTORY`: `enum value`
    - `PATHINFO_TYPE_SYMLINK`: `enum value`
    - `PATHINFO_TYPE_OTHER`: `enum value`
- `pathinfo` (struct)
  - What it represents: Represents `pathinfo` data grouped in a struct
  - Members:
    - `kind`: `pathinfo_type`
    - `size`: `sz`
    - `create_time`: `timestamp`
    - `access_time`: `timestamp`
    - `write_time`: `timestamp`
    - `exists`: `b32`
    - `is_read_only`: `b32`
    - `hidden`: `b32`

#### Functions

- `pathinfo_get(const path* src, pathinfo* out_info) -> b32`
  - What it does: Performs pathinfo get
  - Parameters:
    - `src` (`const path*`): Input data used by the operation
    - `out_info` (`pathinfo*`): Output value written by the function

### `filesystem/pathwatch.h`

- Types: **5**
- Functions: **13**

#### Types

- `pathwatch_id` (alias)
  - What it represents: Type alias used for `pathwatch id`
  - Members: none (alias/function type)
- `pathwatch_watch_id` (alias)
  - What it represents: Type alias used for `pathwatch watch id`
  - Members: none (alias/function type)
- `pathwatch_action` (enum)
  - What it represents: Represents a set of named constants for `pathwatch action`
  - Members:
    - `PATHWATCH_ACTION_CREATE`: `enum value`
    - `PATHWATCH_ACTION_DELETE`: `enum value`
    - `PATHWATCH_ACTION_MODIFY`: `enum value`
    - `PATHWATCH_ACTION_MOVE`: `enum value`
- `pathwatch_event` (struct)
  - What it represents: Represents `pathwatch event` data grouped in a struct
  - Members:
    - `pathwatch_id`: `pathwatch_id`
    - `watch_id`: `pathwatch_watch_id`
    - `action`: `pathwatch_action`
    - `watch_path`: `path`
    - `item_path`: `path`
    - `old_item_path`: `path`
- `pathwatch` (struct)
  - What it represents: Represents `pathwatch` data grouped in a struct
  - Members:
    - `id`: `pathwatch_id`
    - `native_handle`: `void*`

#### Functions

- `_pathwatch_create(b32 use_generic_mode, callsite site) -> pathwatch`
  - What it does: Creates  pathwatch create resources or state
  - Parameters:
    - `use_generic_mode` (`b32`): Input parameter
    - `site` (`callsite`): Input parameter
- `_pathwatch_destroy(pathwatch* watcher, callsite site) -> void`
  - What it does: Destroys  pathwatch destroy resources or state
  - Parameters:
    - `watcher` (`pathwatch*`): Input parameter
    - `site` (`callsite`): Input parameter
- `pathwatch_start(pathwatch* watcher) -> b32`
  - What it does: Performs pathwatch start
  - Parameters:
    - `watcher` (`pathwatch*`): Input parameter
- `pathwatch_stop(pathwatch* watcher) -> b32`
  - What it does: Performs pathwatch stop
  - Parameters:
    - `watcher` (`pathwatch*`): Input parameter
- `pathwatch_pause(pathwatch* watcher) -> b32`
  - What it does: Performs pathwatch pause
  - Parameters:
    - `watcher` (`pathwatch*`): Input parameter
- `pathwatch_resume(pathwatch* watcher) -> b32`
  - What it does: Performs pathwatch resume
  - Parameters:
    - `watcher` (`pathwatch*`): Input parameter
- `pathwatch_add(pathwatch* watcher, const path* src, b32 recursive) -> pathwatch_watch_id`
  - What it does: Adds pathwatch add
  - Parameters:
    - `watcher` (`pathwatch*`): Input parameter
    - `src` (`const path*`): Input data used by the operation
    - `recursive` (`b32`): Input parameter
- `pathwatch_remove(pathwatch* watcher, pathwatch_watch_id watch_id) -> b32`
  - What it does: Removes pathwatch remove
  - Parameters:
    - `watcher` (`pathwatch*`): Input parameter
    - `watch_id` (`pathwatch_watch_id`): Input parameter
- `pathwatch_remove_path(pathwatch* watcher, const path* src) -> b32`
  - What it does: Removes pathwatch remove path
  - Parameters:
    - `watcher` (`pathwatch*`): Input parameter
    - `src` (`const path*`): Input data used by the operation
- `pathwatch_get_path(pathwatch_watch_id watch_id, path* out_watch_path) -> b32`
  - What it does: Gets pathwatch get path
  - Parameters:
    - `watch_id` (`pathwatch_watch_id`): Input parameter
    - `out_watch_path` (`path*`): Output value written by the function
- `pathwatch_follow_symlinks(pathwatch* watcher, b32 enabled) -> b32`
  - What it does: Performs pathwatch follow symlinks
  - Parameters:
    - `watcher` (`pathwatch*`): Input parameter
    - `enabled` (`b32`): Input parameter
- `pathwatch_allow_out_of_scope_links(pathwatch* watcher, b32 enabled) -> b32`
  - What it does: Performs pathwatch allow out of scope links
  - Parameters:
    - `watcher` (`pathwatch*`): Input parameter
    - `enabled` (`b32`): Input parameter
- `pathwatch_get_last_error() -> cstr8`
  - What it does: Gets pathwatch get last error
  - Parameters: none

## input

Device input APIs and message/event distribution.

### `input/audio_device.h`

- Types: **2**
- Functions: **8**

#### Types

- `audio_device` (alias)
  - What it represents: Type alias used for `audio device`
  - Members: none (alias/function type)
- `audio_device_type` (enum)
  - What it represents: Represents a set of named constants for `audio device type`
  - Members:
    - `AUDIO_DEVICE_TYPE_UNKNOWN`: `enum value`
    - `AUDIO_DEVICE_TYPE_PLAYBACK`: `enum value`
    - `AUDIO_DEVICE_TYPE_RECORDING`: `enum value`

#### Functions

- `audio_device_is_valid(audio_device src) -> b32`
  - What it does: Performs audio device is valid
  - Parameters:
    - `src` (`audio_device`): Input data used by the operation
- `audio_device_from_device(device src) -> audio_device`
  - What it does: Performs audio device from device
  - Parameters:
    - `src` (`device`): Input data used by the operation
- `audio_device_type_is_valid(audio_device_type src) -> b32`
  - What it does: Performs audio device type is valid
  - Parameters:
    - `src` (`audio_device_type`): Input data used by the operation
- `audio_device_get_type_name(audio_device_type audio_type) -> cstr8`
  - What it does: Gets audio device get type name
  - Parameters:
    - `audio_type` (`audio_device_type`): Input parameter
- `audio_device_get_total_count(audio_device_type audio_type) -> sz`
  - What it does: Gets audio device get total count
  - Parameters:
    - `audio_type` (`audio_device_type`): Input parameter
- `audio_device_get_id(audio_device_type audio_type, sz idx, audio_device* out_id) -> b32`
  - What it does: Gets audio device get id
  - Parameters:
    - `audio_type` (`audio_device_type`): Input parameter
    - `idx` (`sz`): Zero-based index used to select an item
    - `out_id` (`audio_device*`): Output value written by the function
- `audio_device_get_type(audio_device aud_id) -> audio_device_type`
  - What it does: Gets audio device get type
  - Parameters:
    - `aud_id` (`audio_device`): Input parameter
- `audio_device_get_name(audio_device aud_id) -> cstr8`
  - What it does: Gets audio device get name
  - Parameters:
    - `aud_id` (`audio_device`): Input parameter

### `input/camera.h`

- Types: **2**
- Functions: **11**

#### Types

- `camera` (alias)
  - What it represents: Type alias used for `camera`
  - Members: none (alias/function type)
- `camera_pos` (enum)
  - What it represents: Represents a set of named constants for `camera pos`
  - Members:
    - `CAMERA_POS_UNKNOWN`: `enum value`
    - `CAMERA_POS_FRONT_FACING`: `enum value`
    - `CAMERA_POS_BACK_FACING`: `enum value`

#### Functions

- `camera_is_valid(camera src) -> b32`
  - What it does: Performs camera is valid
  - Parameters:
    - `src` (`camera`): Input data used by the operation
- `camera_from_device(device src) -> camera`
  - What it does: Performs camera from device
  - Parameters:
    - `src` (`device`): Input data used by the operation
- `camera_get_total_count() -> sz`
  - What it does: Gets camera get total count
  - Parameters: none
- `camera_get_id(sz idx, camera* out_id) -> b32`
  - What it does: Gets camera get id
  - Parameters:
    - `idx` (`sz`): Zero-based index used to select an item
    - `out_id` (`camera*`): Output value written by the function
- `camera_get_name(camera cam_id) -> cstr8`
  - What it does: Gets camera get name
  - Parameters:
    - `cam_id` (`camera`): Input parameter
- `camera_get_pos(camera cam_id) -> camera_pos`
  - What it does: Gets camera get pos
  - Parameters:
    - `cam_id` (`camera`): Input parameter
- `camera_open(camera cam_id) -> b32`
  - What it does: Opens camera open
  - Parameters:
    - `cam_id` (`camera`): Input parameter
- `camera_close(camera cam_id) -> b32`
  - What it does: Closes camera close
  - Parameters:
    - `cam_id` (`camera`): Input parameter
- `camera_start(camera cam_id) -> b32`
  - What it does: Performs camera start
  - Parameters:
    - `cam_id` (`camera`): Input parameter
- `camera_stop(camera cam_id) -> b32`
  - What it does: Performs camera stop
  - Parameters:
    - `cam_id` (`camera`): Input parameter
- `camera_read(camera cam_id, buffer* out_frame) -> b32`
  - What it does: Reads data for camera read
  - Parameters:
    - `cam_id` (`camera`): Input parameter
    - `out_frame` (`buffer*`): Output value written by the function

### `input/clipboard.h`

- Types: **0**
- Functions: **7**

#### Types

- None

#### Functions

- `clipboard_has_text() -> b32`
  - What it does: Performs clipboard has text
  - Parameters: none
- `clipboard_has_data(cstr8 mime_type) -> b32`
  - What it does: Performs clipboard has data
  - Parameters:
    - `mime_type` (`cstr8`): Input parameter
- `clipboard_set_text(cstr8 src) -> b32`
  - What it does: Sets clipboard set text
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
- `clipboard_set_data(cstr8 mime_type, buffer src_data) -> b32`
  - What it does: Sets clipboard set data
  - Parameters:
    - `mime_type` (`cstr8`): Input parameter
    - `src_data` (`buffer`): Input data used by the operation
- `clipboard_get_text(c8* out_text, sz out_capacity) -> b32`
  - What it does: Gets clipboard get text
  - Parameters:
    - `out_text` (`c8*`): Output value written by the function
    - `out_capacity` (`sz`): Output value written by the function
- `clipboard_get_data(cstr8 mime_type, buffer out_data, sz* out_size) -> b32`
  - What it does: Gets clipboard get data
  - Parameters:
    - `mime_type` (`cstr8`): Input parameter
    - `out_data` (`buffer`): Output value written by the function
    - `out_size` (`sz*`): Output value written by the function
- `clipboard_clear() -> b32`
  - What it does: Performs clipboard clear
  - Parameters: none

### `input/devices.h`

- Types: **3**
- Functions: **7**

#### Types

- `device` (alias)
  - What it represents: Type alias used for `device`
  - Members: none (alias/function type)
- `device_type` (enum)
  - What it represents: Represents a set of named constants for `device type`
  - Members:
    - `DEVICE_TYPE_UNKNOWN`: `enum value`
    - `DEVICE_TYPE_KEYBOARD`: `enum value`
    - `DEVICE_TYPE_MOUSE`: `enum value`
    - `DEVICE_TYPE_GAMEPAD`: `enum value`
    - `DEVICE_TYPE_JOYSTICK`: `enum value`
    - `DEVICE_TYPE_TABLET`: `enum value`
    - `DEVICE_TYPE_TOUCH`: `enum value`
    - `DEVICE_TYPE_AUDIO`: `enum value`
    - `DEVICE_TYPE_CAMERA`: `enum value`
    - `DEVICE_TYPE_SENSOR`: `enum value`
    - `DEVICE_TYPE_MONITOR`: `enum value`
- `device_info` (struct)
  - What it represents: Represents `device info` data grouped in a struct
  - Members:
    - `id`: `device`
    - `connected`: `b32`
    - `name`: `str8_short`
    - `vendor_id`: `u16`
    - `product_id`: `u16`
    - `usage_page`: `u16`
    - `usage`: `u16`

#### Functions

- `device_is_valid(device src) -> b32`
  - What it does: Performs device is valid
  - Parameters:
    - `src` (`device`): Input data used by the operation
- `devices_get_type(device src) -> device_type`
  - What it does: Gets devices get type
  - Parameters:
    - `src` (`device`): Input data used by the operation
- `devices_get_type_name(device_type type) -> cstr8`
  - What it does: Gets devices get type name
  - Parameters:
    - `type` (`device_type`): Input parameter
- `devices_get_count(device_type type) -> sz`
  - What it does: Gets devices get count
  - Parameters:
    - `type` (`device_type`): Input parameter
- `devices_get_device(device_type type, sz idx) -> device`
  - What it does: Gets devices get device
  - Parameters:
    - `type` (`device_type`): Input parameter
    - `idx` (`sz`): Zero-based index used to select an item
- `devices_is_connected(device dev_id) -> b32`
  - What it does: Performs devices is connected
  - Parameters:
    - `dev_id` (`device`): Input parameter
- `devices_get_info(device dev_id, device_info* out_info) -> b32`
  - What it does: Gets devices get info
  - Parameters:
    - `dev_id` (`device`): Input parameter
    - `out_info` (`device_info*`): Output value written by the function

### `input/gamepads.h`

- Types: **5**
- Functions: **12**

#### Types

- `gamepad_button` (enum)
  - What it represents: Represents a set of named constants for `gamepad button`
  - Members:
    - `GAMEPAD_BUTTON_INVALID`: `enum value`
    - `GAMEPAD_BUTTON_SOUTH`: `enum value`
    - `GAMEPAD_BUTTON_EAST`: `enum value`
    - `GAMEPAD_BUTTON_WEST`: `enum value`
    - `GAMEPAD_BUTTON_NORTH`: `enum value`
    - `GAMEPAD_BUTTON_BACK`: `enum value`
    - `GAMEPAD_BUTTON_GUIDE`: `enum value`
    - `GAMEPAD_BUTTON_START`: `enum value`
    - `GAMEPAD_BUTTON_LEFT_STICK`: `enum value`
    - `GAMEPAD_BUTTON_RIGHT_STICK`: `enum value`
    - `GAMEPAD_BUTTON_LEFT_SHOULDER`: `enum value`
    - `GAMEPAD_BUTTON_RIGHT_SHOULDER`: `enum value`
    - `GAMEPAD_BUTTON_DPAD_UP`: `enum value`
    - `GAMEPAD_BUTTON_DPAD_DOWN`: `enum value`
    - `GAMEPAD_BUTTON_DPAD_LEFT`: `enum value`
    - `GAMEPAD_BUTTON_DPAD_RIGHT`: `enum value`
    - `GAMEPAD_BUTTON_MISC1`: `enum value`
    - `GAMEPAD_BUTTON_RIGHT_PADDLE1`: `enum value`
    - `GAMEPAD_BUTTON_LEFT_PADDLE1`: `enum value`
    - `GAMEPAD_BUTTON_RIGHT_PADDLE2`: `enum value`
    - `GAMEPAD_BUTTON_LEFT_PADDLE2`: `enum value`
    - `GAMEPAD_BUTTON_TOUCHPAD`: `enum value`
    - `GAMEPAD_BUTTON_MISC2`: `enum value`
    - `GAMEPAD_BUTTON_MISC3`: `enum value`
    - `GAMEPAD_BUTTON_MISC4`: `enum value`
    - `GAMEPAD_BUTTON_MISC5`: `enum value`
    - `GAMEPAD_BUTTON_MISC6`: `enum value`
    - `GAMEPAD_BUTTON_COUNT`: `enum value`
- `gamepad_axis` (enum)
  - What it represents: Represents a set of named constants for `gamepad axis`
  - Members:
    - `GAMEPAD_AXIS_INVALID`: `enum value`
    - `GAMEPAD_AXIS_LEFTX`: `enum value`
    - `GAMEPAD_AXIS_LEFTY`: `enum value`
    - `GAMEPAD_AXIS_RIGHTX`: `enum value`
    - `GAMEPAD_AXIS_RIGHTY`: `enum value`
    - `GAMEPAD_AXIS_LEFT_TRIGGER`: `enum value`
    - `GAMEPAD_AXIS_RIGHT_TRIGGER`: `enum value`
    - `GAMEPAD_AXIS_COUNT`: `enum value`
- `gamepad_touchpad_idx` (alias)
  - What it represents: Type alias used for `gamepad touchpad idx`
  - Members: none (alias/function type)
- `gamepad_finger_idx` (alias)
  - What it represents: Type alias used for `gamepad finger idx`
  - Members: none (alias/function type)
- `gamepad_sensor_kind` (enum)
  - What it represents: Represents a set of named constants for `gamepad sensor kind`
  - Members:
    - `GAMEPAD_SENSOR_KIND_INVALID`: `enum value`
    - `GAMEPAD_SENSOR_KIND_UNKNOWN`: `enum value`
    - `GAMEPAD_SENSOR_KIND_ACCEL`: `enum value`
    - `GAMEPAD_SENSOR_KIND_GYRO`: `enum value`
    - `GAMEPAD_SENSOR_KIND_ACCEL_L`: `enum value`
    - `GAMEPAD_SENSOR_KIND_GYRO_L`: `enum value`
    - `GAMEPAD_SENSOR_KIND_ACCEL_R`: `enum value`
    - `GAMEPAD_SENSOR_KIND_GYRO_R`: `enum value`

#### Functions

- `gamepads_get_count() -> sz`
  - What it does: Gets gamepads get count
  - Parameters: none
- `gamepads_is_connected(sz slot_idx) -> b32`
  - What it does: Performs gamepads is connected
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
- `gamepads_get_device(sz slot_idx) -> device`
  - What it does: Gets gamepads get device
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
- `gamepads_get_name(sz slot_idx) -> cstr8`
  - What it does: Gets gamepads get name
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
- `gamepads_has_button(sz slot_idx, gamepad_button button) -> b32`
  - What it does: Performs gamepads has button
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
    - `button` (`gamepad_button`): Input parameter
- `gamepads_get_button(sz slot_idx, gamepad_button button) -> b32`
  - What it does: Gets gamepads get button
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
    - `button` (`gamepad_button`): Input parameter
- `gamepads_has_axis(sz slot_idx, gamepad_axis axis) -> b32`
  - What it does: Performs gamepads has axis
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
    - `axis` (`gamepad_axis`): Input parameter
- `gamepads_get_axis(sz slot_idx, gamepad_axis axis) -> i16`
  - What it does: Gets gamepads get axis
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
    - `axis` (`gamepad_axis`): Input parameter
- `gamepads_set_rumble(sz slot_idx, u16 low_freq, u16 high_freq, u32 duration_ms) -> b32`
  - What it does: Sets gamepads set rumble
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
    - `low_freq` (`u16`): Input parameter
    - `high_freq` (`u16`): Input parameter
    - `duration_ms` (`u32`): Input parameter
- `gamepads_set_led(sz slot_idx, u8 red, u8 green, u8 blue) -> b32`
  - What it does: Sets gamepads set led
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
    - `red` (`u8`): Input parameter
    - `green` (`u8`): Input parameter
    - `blue` (`u8`): Input parameter
- `gamepads_set_axis_deadzone(sz slot_idx, gamepad_axis axis, i16 deadzone) -> b32`
  - What it does: Sets gamepads set axis deadzone
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
    - `axis` (`gamepad_axis`): Input parameter
    - `deadzone` (`i16`): Input parameter
- `gamepads_get_axis_deadzone(sz slot_idx, gamepad_axis axis) -> i16`
  - What it does: Gets gamepads get axis deadzone
  - Parameters:
    - `slot_idx` (`sz`): Zero-based index used to select an item
    - `axis` (`gamepad_axis`): Input parameter

### `input/joystick.h`

- Types: **3**
- Functions: **15**

#### Types

- `joystick` (alias)
  - What it represents: Type alias used for `joystick`
  - Members: none (alias/function type)
- `battery_state` (enum)
  - What it represents: Represents a set of named constants for `battery state`
  - Members:
    - `BATTERY_STATE_UNKNOWN`: `enum value`
    - `BATTERY_STATE_ERROR`: `enum value`
    - `BATTERY_STATE_ON_BATTERY`: `enum value`
    - `BATTERY_STATE_NO_BATTERY`: `enum value`
    - `BATTERY_STATE_CHARGING`: `enum value`
    - `BATTERY_STATE_CHARGED`: `enum value`
- `joystick_hat_state` (enum)
  - What it represents: Represents a set of named constants for `joystick hat state`
  - Members:
    - `JOYSTICK_HAT_STATE_CENTERED`: `enum value`
    - `JOYSTICK_HAT_STATE_UP`: `enum value`
    - `JOYSTICK_HAT_STATE_RIGHT`: `enum value`
    - `JOYSTICK_HAT_STATE_DOWN`: `enum value`
    - `JOYSTICK_HAT_STATE_LEFT`: `enum value`
    - `JOYSTICK_HAT_STATE_RIGHT_UP`: `enum value`
    - `JOYSTICK_HAT_STATE_RIGHT_DOWN`: `enum value`
    - `JOYSTICK_HAT_STATE_LEFT_UP`: `enum value`
    - `JOYSTICK_HAT_STATE_LEFT_DOWN`: `enum value`

#### Functions

- `joystick_is_valid(joystick src) -> b32`
  - What it does: Performs joystick is valid
  - Parameters:
    - `src` (`joystick`): Input data used by the operation
- `joystick_from_device(device src) -> joystick`
  - What it does: Performs joystick from device
  - Parameters:
    - `src` (`device`): Input data used by the operation
- `joystick_get_total_count() -> sz`
  - What it does: Gets joystick get total count
  - Parameters: none
- `joystick_get_id(sz idx, joystick* out_id) -> b32`
  - What it does: Gets joystick get id
  - Parameters:
    - `idx` (`sz`): Zero-based index used to select an item
    - `out_id` (`joystick*`): Output value written by the function
- `joystick_get_name(joystick joy_id) -> cstr8`
  - What it does: Gets joystick get name
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
- `joystick_is_connected(joystick joy_id) -> b32`
  - What it does: Performs joystick is connected
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
- `joystick_get_axis_count(joystick joy_id) -> sz`
  - What it does: Gets joystick get axis count
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
- `joystick_get_ball_count(joystick joy_id) -> sz`
  - What it does: Gets joystick get ball count
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
- `joystick_get_hat_count(joystick joy_id) -> sz`
  - What it does: Gets joystick get hat count
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
- `joystick_get_button_count(joystick joy_id) -> sz`
  - What it does: Gets joystick get button count
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
- `joystick_get_axis(joystick joy_id, sz axis_idx) -> i16`
  - What it does: Gets joystick get axis
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
    - `axis_idx` (`sz`): Zero-based index used to select an item
- `joystick_get_ball(joystick joy_id, sz ball_idx, i32* out_xrel, i32* out_yrel) -> b32`
  - What it does: Gets joystick get ball
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
    - `ball_idx` (`sz`): Zero-based index used to select an item
    - `out_xrel` (`i32*`): Output value written by the function
    - `out_yrel` (`i32*`): Output value written by the function
- `joystick_get_hat(joystick joy_id, sz hat_idx) -> joystick_hat_state`
  - What it does: Gets joystick get hat
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
    - `hat_idx` (`sz`): Zero-based index used to select an item
- `joystick_get_button(joystick joy_id, sz button_idx) -> b32`
  - What it does: Gets joystick get button
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
    - `button_idx` (`sz`): Zero-based index used to select an item
- `joystick_get_battery(joystick joy_id, i32* out_percent) -> battery_state`
  - What it does: Gets joystick get battery
  - Parameters:
    - `joy_id` (`joystick`): Input parameter
    - `out_percent` (`i32*`): Output value written by the function

### `input/keyboard.h`

- Types: **0**
- Functions: **9**

#### Types

- None

#### Functions

- `keyboard_is_available() -> b32`
  - What it does: Performs keyboard is available
  - Parameters: none
- `keyboard_get_primary_device() -> device`
  - What it does: Gets keyboard get primary device
  - Parameters: none
- `keyboard_is_key_down(vkey key) -> b32`
  - What it does: Performs keyboard is key down
  - Parameters:
    - `key` (`vkey`): Input parameter
- `keyboard_get_mods() -> keymod`
  - What it does: Gets keyboard get mods
  - Parameters: none
- `keyboard_has_mods(keymod required_mods) -> b32`
  - What it does: Performs keyboard has mods
  - Parameters:
    - `required_mods` (`keymod`): Input parameter
- `keyboard_has_mods_exact(keymod required_mods, keymod forbidden_mods) -> b32`
  - What it does: Performs keyboard has mods exact
  - Parameters:
    - `required_mods` (`keymod`): Input parameter
    - `forbidden_mods` (`keymod`): Input parameter
- `keyboard_is_key_down_mod(vkey key, keymod required_mods, keymod forbidden_mods) -> b32`
  - What it does: Performs keyboard is key down mod
  - Parameters:
    - `key` (`vkey`): Input parameter
    - `required_mods` (`keymod`): Input parameter
    - `forbidden_mods` (`keymod`): Input parameter
- `keyboard_get_key_name(vkey key) -> cstr8`
  - What it does: Gets keyboard get key name
  - Parameters:
    - `key` (`vkey`): Input parameter
- `keyboard_get_key_display_name(vkey key, keymod modifiers, b32 key_event) -> cstr8`
  - What it does: Gets keyboard get key display name
  - Parameters:
    - `key` (`vkey`): Input parameter
    - `modifiers` (`keymod`): Input parameter
    - `key_event` (`b32`): Input parameter

### `input/mouse.h`

- Types: **3**
- Functions: **4**

#### Types

- `mouse_button` (enum)
  - What it represents: Represents a set of named constants for `mouse button`
  - Members:
    - `MOUSE_BUTTON_LEFT`: `enum value`
    - `MOUSE_BUTTON_MIDDLE`: `enum value`
    - `MOUSE_BUTTON_RIGHT`: `enum value`
    - `MOUSE_BUTTON_X1`: `enum value`
    - `MOUSE_BUTTON_X2`: `enum value`
    - `MOUSE_BUTTON_COUNT`: `enum value`
- `mouse_wheel_direction` (enum)
  - What it represents: Represents a set of named constants for `mouse wheel direction`
  - Members:
    - `MOUSE_WHEEL_DIRECTION_NORMAL`: `enum value`
    - `MOUSE_WHEEL_DIRECTION_FLIPPED`: `enum value`
- `mouse_state` (alias)
  - What it represents: Type alias used for `mouse state`
  - Members: none (alias/function type)

#### Functions

- `mouse_is_available() -> b32`
  - What it does: Performs mouse is available
  - Parameters: none
- `mouse_get_primary_device() -> device`
  - What it does: Gets mouse get primary device
  - Parameters: none
- `mouse_get_state() -> mouse_state`
  - What it does: Gets mouse get state
  - Parameters: none
- `mouse_is_button_down(mouse_button button) -> b32`
  - What it does: Performs mouse is button down
  - Parameters:
    - `button` (`mouse_button`): Input parameter

### `input/msg.h`

- Types: **5**
- Functions: **6**

#### Types

- `msg_category` (enum)
  - What it represents: Represents a set of named constants for `msg category`
  - Members:
    - `MSG_CATEGORY_CORE`: `enum value`
    - `MSG_CATEGORY_GRAPHICS`: `enum value`
    - `MSG_CATEGORY_AUDIO`: `enum value`
    - `MSG_CATEGORY_USER0`: `enum value`
    - `MSG_CATEGORY_USER1`: `enum value`
    - `MSG_CATEGORY_USER2`: `enum value`
    - `MSG_CATEGORY_USER3`: `enum value`
    - `MSG_CATEGORY_MAX`: `enum value`
- `msg` (struct)
  - What it represents: Represents `msg` data grouped in a struct
  - Members:
    - `type`: `u32`
    - `timestamp`: `u64`
    - `post_site`: `callsite`
    - `data`: `u8 [MSG_DATA_SIZE]`
    - `category`: `msg_category`
- `msg_handler_fn` (alias)
  - What it represents: Type alias used for `msg handler fn`
  - Members: none (alias/function type)
- `msg_filter_fn` (alias)
  - What it represents: Type alias used for `msg filter fn`
  - Members: none (alias/function type)
- `msg_handler_desc` (struct)
  - What it represents: Represents `msg handler desc` data grouped in a struct
  - Members:
    - `handler_fn`: `msg_handler_fn`
    - `user_data`: `void*`
    - `priority`: `i32`
    - `category`: `msg_category`
    - `type`: `u32`

#### Functions

- `msg_poll(msg* out_msg) -> b32`
  - What it does: Performs msg poll
  - Parameters:
    - `out_msg` (`msg*`): Output value written by the function
- `_msg_post(const msg* src, callsite site) -> b32`
  - What it does: Performs  msg post
  - Parameters:
    - `src` (`const msg*`): Input data used by the operation
    - `site` (`callsite`): Input parameter
- `msg_add_handler(const msg_handler_desc* desc) -> u64`
  - What it does: Adds msg add handler
  - Parameters:
    - `desc` (`const msg_handler_desc*`): Input parameter
- `msg_remove_handler(u64 handler_id) -> b32`
  - What it does: Removes msg remove handler
  - Parameters:
    - `handler_id` (`u64`): Input parameter
- `msg_clear_handlers() -> void`
  - What it does: Performs msg clear handlers
  - Parameters: none
- `msg_set_filter(msg_filter_fn filter_fn, void* user_data) -> void`
  - What it does: Sets msg set filter
  - Parameters:
    - `filter_fn` (`msg_filter_fn`): Input parameter
    - `user_data` (`void*`): Input parameter

### `input/msg_core.h`

- Types: **49**
- Functions: **84**

#### Types

- `msg_core_type` (enum)
  - What it represents: Represents a set of named constants for `msg core type`
  - Members:
    - `MSG_CORE_TYPE_NONE`: `enum value`
    - `MSG_CORE_TYPE_QUIT`: `enum value`
    - `MSG_CORE_TYPE_TERMINATING`: `enum value`
    - `MSG_CORE_TYPE_LOW_MEMORY`: `enum value`
    - `MSG_CORE_TYPE_WILL_ENTER_BACKGROUND`: `enum value`
    - `MSG_CORE_TYPE_DID_ENTER_BACKGROUND`: `enum value`
    - `MSG_CORE_TYPE_WILL_ENTER_FOREGROUND`: `enum value`
    - `MSG_CORE_TYPE_DID_ENTER_FOREGROUND`: `enum value`
    - `MSG_CORE_TYPE_LOCALE_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_SYSTEM_THEME_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_MONITOR_ORIENTATION`: `enum value`
    - `MSG_CORE_TYPE_MONITOR_ADDED`: `enum value`
    - `MSG_CORE_TYPE_MONITOR_REMOVED`: `enum value`
    - `MSG_CORE_TYPE_MONITOR_MOVED`: `enum value`
    - `MSG_CORE_TYPE_MONITOR_DESKTOP_MODE_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_MONITOR_CURRENT_MODE_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_MONITOR_CONTENT_SCALE_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_SHOWN`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_HIDDEN`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_EXPOSED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_MOVED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_RESIZED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_PIXEL_SIZE_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_METAL_VIEW_RESIZED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_MINIMIZED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_MAXIMIZED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_RESTORED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_MOUSE_ENTER`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_MOUSE_LEAVE`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_FOCUS_GAINED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_FOCUS_LOST`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_CLOSE_REQUESTED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_HIT_TEST`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_ICCPROF_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_MONITOR_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_MONITOR_SCALE_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_SAFE_AREA_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_OCCLUDED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_ENTER_FULLSCREEN`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_LEAVE_FULLSCREEN`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_DESTROYED`: `enum value`
    - `MSG_CORE_TYPE_WINDOW_HDR_STATE_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_KEY_DOWN`: `enum value`
    - `MSG_CORE_TYPE_KEY_UP`: `enum value`
    - `MSG_CORE_TYPE_TEXT_EDITING`: `enum value`
    - `MSG_CORE_TYPE_TEXT_INPUT`: `enum value`
    - `MSG_CORE_TYPE_KEYMAP_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_KEYBOARD_ADDED`: `enum value`
    - `MSG_CORE_TYPE_KEYBOARD_REMOVED`: `enum value`
    - `MSG_CORE_TYPE_TEXT_EDITING_CANDIDATES`: `enum value`
    - `MSG_CORE_TYPE_MOUSE_MOTION`: `enum value`
    - `MSG_CORE_TYPE_MOUSE_BUTTON_DOWN`: `enum value`
    - `MSG_CORE_TYPE_MOUSE_BUTTON_UP`: `enum value`
    - `MSG_CORE_TYPE_MOUSE_WHEEL`: `enum value`
    - `MSG_CORE_TYPE_MOUSE_ADDED`: `enum value`
    - `MSG_CORE_TYPE_MOUSE_REMOVED`: `enum value`
    - `MSG_CORE_TYPE_JOYSTICK_AXIS_MOTION`: `enum value`
    - `MSG_CORE_TYPE_JOYSTICK_BALL_MOTION`: `enum value`
    - `MSG_CORE_TYPE_JOYSTICK_HAT_MOTION`: `enum value`
    - `MSG_CORE_TYPE_JOYSTICK_BUTTON_DOWN`: `enum value`
    - `MSG_CORE_TYPE_JOYSTICK_BUTTON_UP`: `enum value`
    - `MSG_CORE_TYPE_JOYSTICK_ADDED`: `enum value`
    - `MSG_CORE_TYPE_JOYSTICK_REMOVED`: `enum value`
    - `MSG_CORE_TYPE_JOYSTICK_BATTERY_UPDATED`: `enum value`
    - `MSG_CORE_TYPE_JOYSTICK_UPDATE_COMPLETE`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_AXIS_MOTION`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_BUTTON_DOWN`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_BUTTON_UP`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_ADDED`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_REMOVED`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_REMAPPED`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_DOWN`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_MOTION`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_TOUCHPAD_UP`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_SENSOR_UPDATE`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_UPDATE_COMPLETE`: `enum value`
    - `MSG_CORE_TYPE_GAMEPAD_STEAM_HANDLE_UPDATED`: `enum value`
    - `MSG_CORE_TYPE_FINGER_DOWN`: `enum value`
    - `MSG_CORE_TYPE_FINGER_UP`: `enum value`
    - `MSG_CORE_TYPE_FINGER_MOTION`: `enum value`
    - `MSG_CORE_TYPE_FINGER_CANCELED`: `enum value`
    - `MSG_CORE_TYPE_CLIPBOARD_UPDATE`: `enum value`
    - `MSG_CORE_TYPE_DROP_FILE`: `enum value`
    - `MSG_CORE_TYPE_DROP_TEXT`: `enum value`
    - `MSG_CORE_TYPE_DROP_BEGIN`: `enum value`
    - `MSG_CORE_TYPE_DROP_COMPLETE`: `enum value`
    - `MSG_CORE_TYPE_DROP_POS`: `enum value`
    - `MSG_CORE_TYPE_AUDIO_DEVICE_ADDED`: `enum value`
    - `MSG_CORE_TYPE_AUDIO_DEVICE_REMOVED`: `enum value`
    - `MSG_CORE_TYPE_AUDIO_DEVICE_FORMAT_CHANGED`: `enum value`
    - `MSG_CORE_TYPE_SENSOR_UPDATE`: `enum value`
    - `MSG_CORE_TYPE_PEN_PROXIMITY_IN`: `enum value`
    - `MSG_CORE_TYPE_PEN_PROXIMITY_OUT`: `enum value`
    - `MSG_CORE_TYPE_PEN_DOWN`: `enum value`
    - `MSG_CORE_TYPE_PEN_UP`: `enum value`
    - `MSG_CORE_TYPE_PEN_BUTTON_DOWN`: `enum value`
    - `MSG_CORE_TYPE_PEN_BUTTON_UP`: `enum value`
    - `MSG_CORE_TYPE_PEN_MOTION`: `enum value`
    - `MSG_CORE_TYPE_PEN_AXIS`: `enum value`
    - `MSG_CORE_TYPE_CAMERA_DEVICE_ADDED`: `enum value`
    - `MSG_CORE_TYPE_CAMERA_DEVICE_REMOVED`: `enum value`
    - `MSG_CORE_TYPE_CAMERA_DEVICE_APPROVED`: `enum value`
    - `MSG_CORE_TYPE_CAMERA_DEVICE_DENIED`: `enum value`
    - `MSG_CORE_TYPE_RENDER_TARGETS_RESET`: `enum value`
    - `MSG_CORE_TYPE_RENDER_DEVICE_RESET`: `enum value`
    - `MSG_CORE_TYPE_RENDER_DEVICE_LOST`: `enum value`
    - `MSG_CORE_TYPE_USER`: `enum value`
    - `MSG_CORE_TYPE_OBJECT_LIFECYCLE`: `enum value`
    - `MSG_CORE_TYPE_THREAD_CTX`: `enum value`
    - `MSG_CORE_TYPE_PATHWATCH`: `enum value`
    - `MSG_CORE_TYPE_LOG`: `enum value`
    - `MSG_CORE_TYPE_ASSERT`: `enum value`
    - `MSG_CORE_TYPE_TOUCH_ADDED`: `enum value`
    - `MSG_CORE_TYPE_TOUCH_REMOVED`: `enum value`
    - `MSG_CORE_TYPE_TABLET_ADDED`: `enum value`
    - `MSG_CORE_TYPE_TABLET_REMOVED`: `enum value`
    - `MSG_CORE_TYPE_GLOBAL_CTX`: `enum value`
- `msg_core_object_event_kind` (enum)
  - What it represents: Represents a set of named constants for `msg core object event kind`
  - Members:
    - `MSG_CORE_OBJECT_EVENT_CREATE`: `enum value`
    - `MSG_CORE_OBJECT_EVENT_DESTROY`: `enum value`
- `msg_core_object_type` (enum)
  - What it represents: Represents a set of named constants for `msg core object type`
  - Members:
    - `MSG_CORE_OBJECT_TYPE_THREAD`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_THREAD_GROUP`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_MUTEX`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_CONDVAR`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_RWLOCK`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_SEMAPHORE`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_SPINLOCK`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_ARENA`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_HEAP`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_POOL`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_RING`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_PROCESS`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_ARCHIVE`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_PATHWATCH`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_LOG_STATE`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_FILEMAP`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_FILESTREAM`: `enum value`
    - `MSG_CORE_OBJECT_TYPE_PIPE`: `enum value`
- `msg_core_thread_ctx_event_kind` (enum)
  - What it represents: Represents a set of named constants for `msg core thread ctx event kind`
  - Members:
    - `MSG_CORE_THREAD_CTX_EVENT_INIT`: `enum value`
    - `MSG_CORE_THREAD_CTX_EVENT_QUIT`: `enum value`
- `msg_core_pathwatch_event_kind` (enum)
  - What it represents: Represents a set of named constants for `msg core pathwatch event kind`
  - Members:
    - `MSG_CORE_PATHWATCH_EVENT_ITEM`: `enum value`
    - `MSG_CORE_PATHWATCH_EVENT_MISSED`: `enum value`
- `msg_core_global_ctx_event_kind` (enum)
  - What it represents: Represents a set of named constants for `msg core global ctx event kind`
  - Members:
    - `MSG_CORE_GLOBAL_CTX_EVENT_INIT`: `enum value`
    - `MSG_CORE_GLOBAL_CTX_EVENT_QUIT`: `enum value`
- `msg_core_monitor_data` (struct)
  - What it represents: Represents `msg core monitor data` data grouped in a struct
  - Members:
    - `monitor`: `monitor`
    - `data1`: `i32`
    - `data2`: `i32`
- `msg_core_window_data` (struct)
  - What it represents: Represents `msg core window data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `data1`: `i32`
    - `data2`: `i32`
- `msg_core_keyboard_device_data` (struct)
  - What it represents: Represents `msg core keyboard device data` data grouped in a struct
  - Members:
    - `device`: `device`
- `msg_core_keyboard_data` (struct)
  - What it represents: Represents `msg core keyboard data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `device`: `device`
    - `key`: `vkey`
    - `modifiers`: `keymod`
    - `down`: `b32`
    - `repeat`: `b32`
- `msg_core_text_editing_data` (struct)
  - What it represents: Represents `msg core text editing data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `text`: `cstr8`
    - `start`: `i32`
    - `length`: `i32`
- `msg_core_text_editing_candidates_data` (struct)
  - What it represents: Represents `msg core text editing candidates data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `candidates`: `cstr8 const*`
    - `num_candidates`: `i32`
    - `selected_candidate`: `i32`
    - `horizontal`: `b32`
- `msg_core_text_input_data` (struct)
  - What it represents: Represents `msg core text input data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `text`: `cstr8`
- `msg_core_mouse_device_data` (struct)
  - What it represents: Represents `msg core mouse device data` data grouped in a struct
  - Members:
    - `device`: `device`
- `msg_core_mouse_motion_data` (struct)
  - What it represents: Represents `msg core mouse motion data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `device`: `device`
    - `button_mask`: `u32`
    - `x`: `f32`
    - `y`: `f32`
    - `xrel`: `f32`
    - `yrel`: `f32`
- `msg_core_mouse_button_data` (struct)
  - What it represents: Represents `msg core mouse button data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `device`: `device`
    - `button`: `mouse_button`
    - `down`: `b32`
    - `clicks`: `u8`
    - `x`: `f32`
    - `y`: `f32`
- `msg_core_mouse_wheel_data` (struct)
  - What it represents: Represents `msg core mouse wheel data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `device`: `device`
    - `x`: `f32`
    - `y`: `f32`
    - `direction`: `mouse_wheel_direction`
    - `mouse_x`: `f32`
    - `mouse_y`: `f32`
- `msg_core_joystick_device_data` (struct)
  - What it represents: Represents `msg core joystick device data` data grouped in a struct
  - Members:
    - `joystick`: `joystick`
- `msg_core_joystick_axis_data` (struct)
  - What it represents: Represents `msg core joystick axis data` data grouped in a struct
  - Members:
    - `joystick`: `joystick`
    - `axis`: `u8`
    - `value`: `i16`
- `msg_core_joystick_ball_data` (struct)
  - What it represents: Represents `msg core joystick ball data` data grouped in a struct
  - Members:
    - `joystick`: `joystick`
    - `ball`: `u8`
    - `xrel`: `i16`
    - `yrel`: `i16`
- `msg_core_joystick_hat_data` (struct)
  - What it represents: Represents `msg core joystick hat data` data grouped in a struct
  - Members:
    - `joystick`: `joystick`
    - `hat`: `u8`
    - `value`: `joystick_hat_state`
- `msg_core_joystick_button_data` (struct)
  - What it represents: Represents `msg core joystick button data` data grouped in a struct
  - Members:
    - `joystick`: `joystick`
    - `button`: `u8`
    - `down`: `b32`
- `msg_core_joystick_battery_data` (struct)
  - What it represents: Represents `msg core joystick battery data` data grouped in a struct
  - Members:
    - `joystick`: `joystick`
    - `state`: `battery_state`
    - `percent`: `i32`
- `msg_core_gamepad_device_data` (struct)
  - What it represents: Represents `msg core gamepad device data` data grouped in a struct
  - Members:
    - `device`: `device`
- `msg_core_gamepad_axis_data` (struct)
  - What it represents: Represents `msg core gamepad axis data` data grouped in a struct
  - Members:
    - `device`: `device`
    - `axis`: `gamepad_axis`
    - `value`: `i16`
- `msg_core_gamepad_button_data` (struct)
  - What it represents: Represents `msg core gamepad button data` data grouped in a struct
  - Members:
    - `device`: `device`
    - `button`: `gamepad_button`
    - `down`: `b32`
- `msg_core_gamepad_touchpad_data` (struct)
  - What it represents: Represents `msg core gamepad touchpad data` data grouped in a struct
  - Members:
    - `device`: `device`
    - `touchpad`: `gamepad_touchpad_idx`
    - `finger`: `gamepad_finger_idx`
    - `x`: `f32`
    - `y`: `f32`
    - `pressure`: `f32`
- `msg_core_gamepad_sensor_data` (struct)
  - What it represents: Represents `msg core gamepad sensor data` data grouped in a struct
  - Members:
    - `device`: `device`
    - `sensor`: `gamepad_sensor_kind`
    - `data`: `f32 [3]`
    - `sensor_timestamp`: `u64`
- `msg_core_audio_device_data` (struct)
  - What it represents: Represents `msg core audio device data` data grouped in a struct
  - Members:
    - `audio`: `audio_device`
- `msg_core_camera_device_data` (struct)
  - What it represents: Represents `msg core camera device data` data grouped in a struct
  - Members:
    - `camera`: `camera`
- `msg_core_render_data` (struct)
  - What it represents: Represents `msg core render data` data grouped in a struct
  - Members:
    - `window`: `window`
- `msg_core_touch_device_data` (struct)
  - What it represents: Represents `msg core touch device data` data grouped in a struct
  - Members:
    - `device`: `device`
- `msg_core_tablet_device_data` (struct)
  - What it represents: Represents `msg core tablet device data` data grouped in a struct
  - Members:
    - `device`: `device`
- `msg_core_touch_data` (struct)
  - What it represents: Represents `msg core touch data` data grouped in a struct
  - Members:
    - `device`: `device`
    - `finger_id`: `finger_id`
    - `x`: `f32`
    - `y`: `f32`
    - `dx`: `f32`
    - `dy`: `f32`
    - `pressure`: `f32`
    - `window`: `window`
- `msg_core_pen_proximity_data` (struct)
  - What it represents: Represents `msg core pen proximity data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `device`: `device`
    - `pen_id`: `pen_id`
- `msg_core_pen_motion_data` (struct)
  - What it represents: Represents `msg core pen motion data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `device`: `device`
    - `pen_id`: `pen_id`
    - `pen_state`: `tablet_input_flags`
    - `x`: `f32`
    - `y`: `f32`
- `msg_core_pen_touch_data` (struct)
  - What it represents: Represents `msg core pen touch data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `device`: `device`
    - `pen_id`: `pen_id`
    - `pen_state`: `tablet_input_flags`
    - `x`: `f32`
    - `y`: `f32`
    - `eraser`: `b32`
    - `down`: `b32`
- `msg_core_pen_button_data` (struct)
  - What it represents: Represents `msg core pen button data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `device`: `device`
    - `pen_id`: `pen_id`
    - `pen_state`: `tablet_input_flags`
    - `x`: `f32`
    - `y`: `f32`
    - `button`: `tablet_button`
    - `down`: `b32`
- `msg_core_pen_axis_data` (struct)
  - What it represents: Represents `msg core pen axis data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `device`: `device`
    - `pen_id`: `pen_id`
    - `pen_state`: `tablet_input_flags`
    - `x`: `f32`
    - `y`: `f32`
    - `axis`: `tablet_axis`
    - `value`: `f32`
- `msg_core_drop_data` (struct)
  - What it represents: Represents `msg core drop data` data grouped in a struct
  - Members:
    - `window`: `window`
    - `x`: `f32`
    - `y`: `f32`
    - `source`: `cstr8`
    - `data`: `cstr8`
- `msg_core_clipboard_data` (struct)
  - What it represents: Represents `msg core clipboard data` data grouped in a struct
  - Members:
    - `owner`: `b32`
    - `num_mime_types`: `i32`
    - `mime_types`: `cstr8 const*`
- `msg_core_sensor_data` (struct)
  - What it represents: Represents `msg core sensor data` data grouped in a struct
  - Members:
    - `sensor`: `sensor`
    - `data`: `f32 [6]`
    - `sensor_timestamp`: `u64`
- `msg_core_object_lifecycle_data` (struct)
  - What it represents: Represents `msg core object lifecycle data` data grouped in a struct
  - Members:
    - `object_type`: `msg_core_object_type`
    - `event_kind`: `msg_core_object_event_kind`
    - `object_ptr`: `void*`
    - `site`: `callsite`
- `msg_core_thread_ctx_data` (struct)
  - What it represents: Represents `msg core thread ctx data` data grouped in a struct
  - Members:
    - `event_kind`: `msg_core_thread_ctx_event_kind`
    - `thread_id`: `u64`
    - `ctx_ptr`: `ctx*`
- `msg_core_pathwatch_data` (struct)
  - What it represents: Represents `msg core pathwatch data` data grouped in a struct
  - Members:
    - `event_kind`: `msg_core_pathwatch_event_kind`
    - `pathwatch_id`: `pathwatch_id`
    - `watch_id`: `pathwatch_watch_id`
    - `action`: `pathwatch_action`
- `msg_core_log_data` (struct)
  - What it represents: Represents `msg core log data` data grouped in a struct
  - Members:
    - `state_ptr`: `log_state*`
    - `level`: `log_level`
    - `source_site`: `callsite`
    - `text`: `str8_medium`
- `msg_core_global_ctx_data` (struct)
  - What it represents: Represents `msg core global ctx data` data grouped in a struct
  - Members:
    - `event_kind`: `msg_core_global_ctx_event_kind`
    - `ctx_ptr`: `ctx*`
- `msg_core_assert_data` (struct)
  - What it represents: Represents `msg core assert data` data grouped in a struct
  - Members:
    - `mode`: `assert_mode`
    - `source_site`: `callsite`
    - `text`: `cstr8`
- `msg_core_data` (union)
  - What it represents: Represents `msg core data` data in alternative memory layouts
  - Members:
    - `monitor`: `msg_core_monitor_data`
    - `window`: `msg_core_window_data`
    - `keyboard_device`: `msg_core_keyboard_device_data`
    - `keyboard`: `msg_core_keyboard_data`
    - `text_editing`: `msg_core_text_editing_data`
    - `text_editing_candidates`: `msg_core_text_editing_candidates_data`
    - `text_input`: `msg_core_text_input_data`
    - `mouse_device`: `msg_core_mouse_device_data`
    - `mouse_motion`: `msg_core_mouse_motion_data`
    - `mouse_button`: `msg_core_mouse_button_data`
    - `mouse_wheel`: `msg_core_mouse_wheel_data`
    - `joystick_device`: `msg_core_joystick_device_data`
    - `joystick_axis`: `msg_core_joystick_axis_data`
    - `joystick_ball`: `msg_core_joystick_ball_data`
    - `joystick_hat`: `msg_core_joystick_hat_data`
    - `joystick_button`: `msg_core_joystick_button_data`
    - `joystick_battery`: `msg_core_joystick_battery_data`
    - `gamepad_device`: `msg_core_gamepad_device_data`
    - `gamepad_axis`: `msg_core_gamepad_axis_data`
    - `gamepad_button`: `msg_core_gamepad_button_data`
    - `gamepad_touchpad`: `msg_core_gamepad_touchpad_data`
    - `gamepad_sensor`: `msg_core_gamepad_sensor_data`
    - `audio_device`: `msg_core_audio_device_data`
    - `camera_device`: `msg_core_camera_device_data`
    - `render`: `msg_core_render_data`
    - `touch_device`: `msg_core_touch_device_data`
    - `touch`: `msg_core_touch_data`
    - `tablet_device`: `msg_core_tablet_device_data`
    - `pen_proximity`: `msg_core_pen_proximity_data`
    - `pen_motion`: `msg_core_pen_motion_data`
    - `pen_touch`: `msg_core_pen_touch_data`
    - `pen_button`: `msg_core_pen_button_data`
    - `pen_axis`: `msg_core_pen_axis_data`
    - `drop`: `msg_core_drop_data`
    - `clipboard`: `msg_core_clipboard_data`
    - `sensor`: `msg_core_sensor_data`
    - `object_lifecycle`: `msg_core_object_lifecycle_data`
    - `thread_ctx`: `msg_core_thread_ctx_data`
    - `pathwatch`: `msg_core_pathwatch_data`
    - `log`: `msg_core_log_data`
    - `assert`: `msg_core_assert_data`
    - `global_ctx`: `msg_core_global_ctx_data`

#### Functions

- `msg_core_fill_monitor(msg* src, const msg_core_monitor_data* core_data) -> void`
  - What it does: Performs msg core fill monitor
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_monitor_data*`): Input parameter
- `msg_core_fill_window(msg* src, const msg_core_window_data* core_data) -> void`
  - What it does: Performs msg core fill window
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_window_data*`): Input parameter
- `msg_core_fill_keyboard_device(msg* src, const msg_core_keyboard_device_data* core_data) -> void`
  - What it does: Performs msg core fill keyboard device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_keyboard_device_data*`): Input parameter
- `msg_core_fill_keyboard(msg* src, const msg_core_keyboard_data* core_data) -> void`
  - What it does: Performs msg core fill keyboard
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_keyboard_data*`): Input parameter
- `msg_core_fill_text_editing(msg* src, const msg_core_text_editing_data* core_data) -> void`
  - What it does: Performs msg core fill text editing
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_text_editing_data*`): Input parameter
- `msg_core_fill_text_editing_candidates(msg* src, const msg_core_text_editing_candidates_data* core_data) -> void`
  - What it does: Performs msg core fill text editing candidates
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_text_editing_candidates_data*`): Input parameter
- `msg_core_fill_text_input(msg* src, const msg_core_text_input_data* core_data) -> void`
  - What it does: Performs msg core fill text input
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_text_input_data*`): Input parameter
- `msg_core_fill_mouse_device(msg* src, const msg_core_mouse_device_data* core_data) -> void`
  - What it does: Performs msg core fill mouse device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_mouse_device_data*`): Input parameter
- `msg_core_fill_mouse_motion(msg* src, const msg_core_mouse_motion_data* core_data) -> void`
  - What it does: Performs msg core fill mouse motion
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_mouse_motion_data*`): Input parameter
- `msg_core_fill_mouse_button(msg* src, const msg_core_mouse_button_data* core_data) -> void`
  - What it does: Performs msg core fill mouse button
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_mouse_button_data*`): Input parameter
- `msg_core_fill_mouse_wheel(msg* src, const msg_core_mouse_wheel_data* core_data) -> void`
  - What it does: Performs msg core fill mouse wheel
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_mouse_wheel_data*`): Input parameter
- `msg_core_fill_joystick_device(msg* src, const msg_core_joystick_device_data* core_data) -> void`
  - What it does: Performs msg core fill joystick device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_joystick_device_data*`): Input parameter
- `msg_core_fill_joystick_axis(msg* src, const msg_core_joystick_axis_data* core_data) -> void`
  - What it does: Performs msg core fill joystick axis
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_joystick_axis_data*`): Input parameter
- `msg_core_fill_joystick_ball(msg* src, const msg_core_joystick_ball_data* core_data) -> void`
  - What it does: Performs msg core fill joystick ball
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_joystick_ball_data*`): Input parameter
- `msg_core_fill_joystick_hat(msg* src, const msg_core_joystick_hat_data* core_data) -> void`
  - What it does: Performs msg core fill joystick hat
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_joystick_hat_data*`): Input parameter
- `msg_core_fill_joystick_button(msg* src, const msg_core_joystick_button_data* core_data) -> void`
  - What it does: Performs msg core fill joystick button
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_joystick_button_data*`): Input parameter
- `msg_core_fill_joystick_battery(msg* src, const msg_core_joystick_battery_data* core_data) -> void`
  - What it does: Performs msg core fill joystick battery
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_joystick_battery_data*`): Input parameter
- `msg_core_fill_gamepad_device(msg* src, const msg_core_gamepad_device_data* core_data) -> void`
  - What it does: Performs msg core fill gamepad device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_gamepad_device_data*`): Input parameter
- `msg_core_fill_gamepad_axis(msg* src, const msg_core_gamepad_axis_data* core_data) -> void`
  - What it does: Performs msg core fill gamepad axis
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_gamepad_axis_data*`): Input parameter
- `msg_core_fill_gamepad_button(msg* src, const msg_core_gamepad_button_data* core_data) -> void`
  - What it does: Performs msg core fill gamepad button
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_gamepad_button_data*`): Input parameter
- `msg_core_fill_gamepad_touchpad(msg* src, const msg_core_gamepad_touchpad_data* core_data) -> void`
  - What it does: Performs msg core fill gamepad touchpad
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_gamepad_touchpad_data*`): Input parameter
- `msg_core_fill_gamepad_sensor(msg* src, const msg_core_gamepad_sensor_data* core_data) -> void`
  - What it does: Performs msg core fill gamepad sensor
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_gamepad_sensor_data*`): Input parameter
- `msg_core_fill_audio_device(msg* src, const msg_core_audio_device_data* core_data) -> void`
  - What it does: Performs msg core fill audio device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_audio_device_data*`): Input parameter
- `msg_core_fill_camera_device(msg* src, const msg_core_camera_device_data* core_data) -> void`
  - What it does: Performs msg core fill camera device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_camera_device_data*`): Input parameter
- `msg_core_fill_render(msg* src, const msg_core_render_data* core_data) -> void`
  - What it does: Performs msg core fill render
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_render_data*`): Input parameter
- `msg_core_fill_touch_device(msg* src, const msg_core_touch_device_data* core_data) -> void`
  - What it does: Performs msg core fill touch device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_touch_device_data*`): Input parameter
- `msg_core_fill_touch(msg* src, const msg_core_touch_data* core_data) -> void`
  - What it does: Performs msg core fill touch
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_touch_data*`): Input parameter
- `msg_core_fill_tablet_device(msg* src, const msg_core_tablet_device_data* core_data) -> void`
  - What it does: Performs msg core fill tablet device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_tablet_device_data*`): Input parameter
- `msg_core_fill_pen_proximity(msg* src, const msg_core_pen_proximity_data* core_data) -> void`
  - What it does: Performs msg core fill pen proximity
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_pen_proximity_data*`): Input parameter
- `msg_core_fill_pen_motion(msg* src, const msg_core_pen_motion_data* core_data) -> void`
  - What it does: Performs msg core fill pen motion
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_pen_motion_data*`): Input parameter
- `msg_core_fill_pen_touch(msg* src, const msg_core_pen_touch_data* core_data) -> void`
  - What it does: Performs msg core fill pen touch
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_pen_touch_data*`): Input parameter
- `msg_core_fill_pen_button(msg* src, const msg_core_pen_button_data* core_data) -> void`
  - What it does: Performs msg core fill pen button
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_pen_button_data*`): Input parameter
- `msg_core_fill_pen_axis(msg* src, const msg_core_pen_axis_data* core_data) -> void`
  - What it does: Performs msg core fill pen axis
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_pen_axis_data*`): Input parameter
- `msg_core_fill_drop(msg* src, const msg_core_drop_data* core_data) -> void`
  - What it does: Performs msg core fill drop
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_drop_data*`): Input parameter
- `msg_core_fill_clipboard(msg* src, const msg_core_clipboard_data* core_data) -> void`
  - What it does: Performs msg core fill clipboard
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_clipboard_data*`): Input parameter
- `msg_core_fill_sensor(msg* src, const msg_core_sensor_data* core_data) -> void`
  - What it does: Performs msg core fill sensor
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_sensor_data*`): Input parameter
- `msg_core_fill_object_lifecycle(msg* src, const msg_core_object_lifecycle_data* core_data) -> void`
  - What it does: Performs msg core fill object lifecycle
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_object_lifecycle_data*`): Input parameter
- `msg_core_fill_thread_ctx(msg* src, const msg_core_thread_ctx_data* core_data) -> void`
  - What it does: Performs msg core fill thread ctx
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_thread_ctx_data*`): Input parameter
- `msg_core_fill_pathwatch(msg* src, const msg_core_pathwatch_data* core_data) -> void`
  - What it does: Performs msg core fill pathwatch
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_pathwatch_data*`): Input parameter
- `msg_core_fill_log(msg* src, const msg_core_log_data* core_data) -> void`
  - What it does: Performs msg core fill log
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_log_data*`): Input parameter
- `msg_core_fill_assert(msg* src, const msg_core_assert_data* core_data) -> void`
  - What it does: Performs msg core fill assert
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_assert_data*`): Input parameter
- `msg_core_fill_global_ctx(msg* src, const msg_core_global_ctx_data* core_data) -> void`
  - What it does: Performs msg core fill global ctx
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
    - `core_data` (`const msg_core_global_ctx_data*`): Input parameter
- `msg_core_get_monitor(msg* src) -> msg_core_monitor_data*`
  - What it does: Gets msg core get monitor
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_window(msg* src) -> msg_core_window_data*`
  - What it does: Gets msg core get window
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_keyboard_device(msg* src) -> msg_core_keyboard_device_data*`
  - What it does: Gets msg core get keyboard device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_keyboard(msg* src) -> msg_core_keyboard_data*`
  - What it does: Gets msg core get keyboard
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_text_editing(msg* src) -> msg_core_text_editing_data*`
  - What it does: Gets msg core get text editing
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_text_editing_candidates(msg* src) -> msg_core_text_editing_candidates_data*`
  - What it does: Gets msg core get text editing candidates
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_text_input(msg* src) -> msg_core_text_input_data*`
  - What it does: Gets msg core get text input
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_mouse_device(msg* src) -> msg_core_mouse_device_data*`
  - What it does: Gets msg core get mouse device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_mouse_motion(msg* src) -> msg_core_mouse_motion_data*`
  - What it does: Gets msg core get mouse motion
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_mouse_button(msg* src) -> msg_core_mouse_button_data*`
  - What it does: Gets msg core get mouse button
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_mouse_wheel(msg* src) -> msg_core_mouse_wheel_data*`
  - What it does: Gets msg core get mouse wheel
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_joystick_device(msg* src) -> msg_core_joystick_device_data*`
  - What it does: Gets msg core get joystick device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_joystick_axis(msg* src) -> msg_core_joystick_axis_data*`
  - What it does: Gets msg core get joystick axis
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_joystick_ball(msg* src) -> msg_core_joystick_ball_data*`
  - What it does: Gets msg core get joystick ball
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_joystick_hat(msg* src) -> msg_core_joystick_hat_data*`
  - What it does: Gets msg core get joystick hat
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_joystick_button(msg* src) -> msg_core_joystick_button_data*`
  - What it does: Gets msg core get joystick button
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_joystick_battery(msg* src) -> msg_core_joystick_battery_data*`
  - What it does: Gets msg core get joystick battery
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_gamepad_device(msg* src) -> msg_core_gamepad_device_data*`
  - What it does: Gets msg core get gamepad device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_gamepad_axis(msg* src) -> msg_core_gamepad_axis_data*`
  - What it does: Gets msg core get gamepad axis
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_gamepad_button(msg* src) -> msg_core_gamepad_button_data*`
  - What it does: Gets msg core get gamepad button
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_gamepad_touchpad(msg* src) -> msg_core_gamepad_touchpad_data*`
  - What it does: Gets msg core get gamepad touchpad
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_gamepad_sensor(msg* src) -> msg_core_gamepad_sensor_data*`
  - What it does: Gets msg core get gamepad sensor
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_audio_device(msg* src) -> msg_core_audio_device_data*`
  - What it does: Gets msg core get audio device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_camera_device(msg* src) -> msg_core_camera_device_data*`
  - What it does: Gets msg core get camera device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_render(msg* src) -> msg_core_render_data*`
  - What it does: Gets msg core get render
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_touch_device(msg* src) -> msg_core_touch_device_data*`
  - What it does: Gets msg core get touch device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_touch(msg* src) -> msg_core_touch_data*`
  - What it does: Gets msg core get touch
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_tablet_device(msg* src) -> msg_core_tablet_device_data*`
  - What it does: Gets msg core get tablet device
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_pen_proximity(msg* src) -> msg_core_pen_proximity_data*`
  - What it does: Gets msg core get pen proximity
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_pen_motion(msg* src) -> msg_core_pen_motion_data*`
  - What it does: Gets msg core get pen motion
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_pen_touch(msg* src) -> msg_core_pen_touch_data*`
  - What it does: Gets msg core get pen touch
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_pen_button(msg* src) -> msg_core_pen_button_data*`
  - What it does: Gets msg core get pen button
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_pen_axis(msg* src) -> msg_core_pen_axis_data*`
  - What it does: Gets msg core get pen axis
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_drop(msg* src) -> msg_core_drop_data*`
  - What it does: Gets msg core get drop
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_clipboard(msg* src) -> msg_core_clipboard_data*`
  - What it does: Gets msg core get clipboard
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_sensor(msg* src) -> msg_core_sensor_data*`
  - What it does: Gets msg core get sensor
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_object_lifecycle(msg* src) -> msg_core_object_lifecycle_data*`
  - What it does: Gets msg core get object lifecycle
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_thread_ctx(msg* src) -> msg_core_thread_ctx_data*`
  - What it does: Gets msg core get thread ctx
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_pathwatch(msg* src) -> msg_core_pathwatch_data*`
  - What it does: Gets msg core get pathwatch
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_log(msg* src) -> msg_core_log_data*`
  - What it does: Gets msg core get log
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_assert(msg* src) -> msg_core_assert_data*`
  - What it does: Gets msg core get assert
  - Parameters:
    - `src` (`msg*`): Input data used by the operation
- `msg_core_get_global_ctx(msg* src) -> msg_core_global_ctx_data*`
  - What it does: Gets msg core get global ctx
  - Parameters:
    - `src` (`msg*`): Input data used by the operation

### `input/sensor.h`

- Types: **2**
- Functions: **11**

#### Types

- `sensor` (alias)
  - What it represents: Type alias used for `sensor`
  - Members: none (alias/function type)
- `sensor_kind` (enum)
  - What it represents: Represents a set of named constants for `sensor kind`
  - Members:
    - `SENSOR_KIND_INVALID`: `enum value`
    - `SENSOR_KIND_UNKNOWN`: `enum value`
    - `SENSOR_KIND_ACCEL`: `enum value`
    - `SENSOR_KIND_GYRO`: `enum value`
    - `SENSOR_KIND_ACCEL_L`: `enum value`
    - `SENSOR_KIND_GYRO_L`: `enum value`
    - `SENSOR_KIND_ACCEL_R`: `enum value`
    - `SENSOR_KIND_GYRO_R`: `enum value`

#### Functions

- `sensor_is_valid(sensor src) -> b32`
  - What it does: Performs sensor is valid
  - Parameters:
    - `src` (`sensor`): Input data used by the operation
- `sensor_from_device(device src) -> sensor`
  - What it does: Performs sensor from device
  - Parameters:
    - `src` (`device`): Input data used by the operation
- `sensor_get_total_count() -> sz`
  - What it does: Gets sensor get total count
  - Parameters: none
- `sensor_get_from_idx(sz idx, sensor* out_id) -> b32`
  - What it does: Gets sensor get from idx
  - Parameters:
    - `idx` (`sz`): Zero-based index used to select an item
    - `out_id` (`sensor*`): Output value written by the function
- `sensor_get_name(sensor sen_id) -> cstr8`
  - What it does: Gets sensor get name
  - Parameters:
    - `sen_id` (`sensor`): Input parameter
- `sensor_get_kind(sensor sen_id) -> sensor_kind`
  - What it does: Gets sensor get kind
  - Parameters:
    - `sen_id` (`sensor`): Input parameter
- `sensor_open(sensor sen_id) -> b32`
  - What it does: Opens sensor open
  - Parameters:
    - `sen_id` (`sensor`): Input parameter
- `sensor_close(sensor sen_id) -> b32`
  - What it does: Closes sensor close
  - Parameters:
    - `sen_id` (`sensor`): Input parameter
- `sensor_start(sensor sen_id) -> b32`
  - What it does: Performs sensor start
  - Parameters:
    - `sen_id` (`sensor`): Input parameter
- `sensor_stop(sensor sen_id) -> b32`
  - What it does: Performs sensor stop
  - Parameters:
    - `sen_id` (`sensor`): Input parameter
- `sensor_read(sensor sen_id, buffer* out_samples) -> b32`
  - What it does: Reads data for sensor read
  - Parameters:
    - `sen_id` (`sensor`): Input parameter
    - `out_samples` (`buffer*`): Output value written by the function

### `input/tablet.h`

- Types: **6**
- Functions: **5**

#### Types

- `pen_id` (alias)
  - What it represents: Type alias used for `pen id`
  - Members: none (alias/function type)
- `tablet_axis` (enum)
  - What it represents: Represents a set of named constants for `tablet axis`
  - Members:
    - `TABLET_AXIS_PRESSURE`: `enum value`
    - `TABLET_AXIS_XTILT`: `enum value`
    - `TABLET_AXIS_YTILT`: `enum value`
    - `TABLET_AXIS_DISTANCE`: `enum value`
    - `TABLET_AXIS_ROTATION`: `enum value`
    - `TABLET_AXIS_SLIDER`: `enum value`
    - `TABLET_AXIS_TANGENTIAL_PRESSURE`: `enum value`
    - `TABLET_AXIS_COUNT`: `enum value`
- `tablet_input_flag` (enum)
  - What it represents: Represents a set of named constants for `tablet input flag`
  - Members:
    - `TABLET_INPUT_FLAG_NONE`: `enum value`
    - `TABLET_INPUT_FLAG_DOWN`: `enum value`
    - `TABLET_INPUT_FLAG_BUTTON_1`: `enum value`
    - `TABLET_INPUT_FLAG_BUTTON_2`: `enum value`
    - `TABLET_INPUT_FLAG_BUTTON_3`: `enum value`
    - `TABLET_INPUT_FLAG_ERASER_TIP`: `enum value`
- `tablet_input_flags` (alias)
  - What it represents: Type alias used for `tablet input flags`
  - Members: none (alias/function type)
- `tablet_button` (enum)
  - What it represents: Represents a set of named constants for `tablet button`
  - Members:
    - `TABLET_BUTTON_PRIMARY`: `enum value`
    - `TABLET_BUTTON_SECONDARY`: `enum value`
    - `TABLET_BUTTON_TERTIARY`: `enum value`
- `tablet_pen_state` (struct)
  - What it represents: Represents `tablet pen state` data grouped in a struct
  - Members:
    - `id`: `device`
    - `pen_id`: `pen_id`
    - `in_proximity`: `b32`
    - `touching`: `b32`
    - `eraser`: `b32`
    - `input_mask`: `tablet_input_flags`
    - `window`: `window`
    - `x`: `f32`
    - `y`: `f32`
    - `axis_values`: `f32 [TABLET_AXIS_COUNT]`

#### Functions

- `tablet_is_available() -> b32`
  - What it does: Performs tablet is available
  - Parameters: none
- `tablet_get_total_count() -> sz`
  - What it does: Gets tablet get total count
  - Parameters: none
- `tablet_get_device(sz idx) -> device`
  - What it does: Gets tablet get device
  - Parameters:
    - `idx` (`sz`): Zero-based index used to select an item
- `tablet_get_last_pen_state(tablet_pen_state* out_state) -> b32`
  - What it does: Gets tablet get last pen state
  - Parameters:
    - `out_state` (`tablet_pen_state*`): Output value written by the function
- `tablet_read_hid_report(device dev_id, void* dst, sz capacity, sz* out_size, i32 timeout_ms) -> b32`
  - What it does: Reads data for tablet read hid report
  - Parameters:
    - `dev_id` (`device`): Input parameter
    - `dst` (`void*`): Destination storage for output data
    - `capacity` (`sz`): Size or capacity value used by the operation
    - `out_size` (`sz*`): Output value written by the function
    - `timeout_ms` (`i32`): Input parameter

### `input/touch.h`

- Types: **3**
- Functions: **6**

#### Types

- `finger_id` (alias)
  - What it represents: Type alias used for `finger id`
  - Members: none (alias/function type)
- `touch_device_kind` (enum)
  - What it represents: Represents a set of named constants for `touch device kind`
  - Members:
    - `TOUCH_DEVICE_INVALID`: `enum value`
    - `TOUCH_DEVICE_DIRECT`: `enum value`
    - `TOUCH_DEVICE_INDIRECT_ABSOLUTE`: `enum value`
    - `TOUCH_DEVICE_INDIRECT_RELATIVE`: `enum value`
- `touch_finger_state` (struct)
  - What it represents: Represents `touch finger state` data grouped in a struct
  - Members:
    - `id`: `finger_id`
    - `x`: `f32`
    - `y`: `f32`
    - `pressure`: `f32`

#### Functions

- `touch_is_available() -> b32`
  - What it does: Performs touch is available
  - Parameters: none
- `touch_get_total_count() -> sz`
  - What it does: Gets touch get total count
  - Parameters: none
- `touch_get_device(sz idx) -> device`
  - What it does: Gets touch get device
  - Parameters:
    - `idx` (`sz`): Zero-based index used to select an item
- `touch_get_device_kind(device dev_id) -> touch_device_kind`
  - What it does: Gets touch get device kind
  - Parameters:
    - `dev_id` (`device`): Input parameter
- `touch_get_finger_count(device dev_id) -> sz`
  - What it does: Gets touch get finger count
  - Parameters:
    - `dev_id` (`device`): Input parameter
- `touch_get_finger(device dev_id, sz idx, touch_finger_state* out_finger) -> b32`
  - What it does: Gets touch get finger
  - Parameters:
    - `dev_id` (`device`): Input parameter
    - `idx` (`sz`): Zero-based index used to select an item
    - `out_finger` (`touch_finger_state*`): Output value written by the function

### `input/vkeys.h`

- Types: **2**
- Functions: **0**

#### Types

- `vkey` (enum)
  - What it represents: Represents a set of named constants for `vkey`
  - Members: none (alias/function type)
- `keymod` (enum)
  - What it represents: Represents a set of named constants for `keymod`
  - Members:
    - `KEYMOD_NONE`: `enum value`
    - `KEYMOD_LSHIFT`: `enum value`
    - `KEYMOD_RSHIFT`: `enum value`
    - `KEYMOD_LEVEL5`: `enum value`
    - `KEYMOD_LCTRL`: `enum value`
    - `KEYMOD_RCTRL`: `enum value`
    - `KEYMOD_LALT`: `enum value`
    - `KEYMOD_RALT`: `enum value`
    - `KEYMOD_LGUI`: `enum value`
    - `KEYMOD_RGUI`: `enum value`
    - `KEYMOD_NUM`: `enum value`
    - `KEYMOD_CAPS`: `enum value`
    - `KEYMOD_MODE`: `enum value`
    - `KEYMOD_SCROLL`: `enum value`
    - `KEYMOD_SHIFT`: `enum value`
    - `KEYMOD_CTRL`: `enum value`
    - `KEYMOD_ALT`: `enum value`
    - `KEYMOD_GUI`: `enum value`

#### Functions

- None

## interface

Windowing and desktop interface abstractions.

### `interface/cursor.h`

- Types: **1**
- Functions: **12**

#### Types

- `cursor_pos` (struct)
  - What it represents: Represents `cursor pos` data grouped in a struct
  - Members:
    - `x`: `f32`
    - `y`: `f32`

#### Functions

- `cursor_set_icon(icon icon_id) -> b32`
  - What it does: Sets cursor set icon
  - Parameters:
    - `icon_id` (`icon`): Input parameter
- `cursor_reset_icon() -> b32`
  - What it does: Performs cursor reset icon
  - Parameters: none
- `cursor_set_visible(b32 visible) -> b32`
  - What it does: Sets cursor set visible
  - Parameters:
    - `visible` (`b32`): Input parameter
- `cursor_is_visible() -> b32`
  - What it does: Performs cursor is visible
  - Parameters: none
- `cursor_set_capture(window opt_window, b32 enabled) -> b32`
  - What it does: Sets cursor set capture
  - Parameters:
    - `opt_window` (`window`): Input parameter
    - `enabled` (`b32`): Input parameter
- `cursor_is_captured(window opt_window) -> b32`
  - What it does: Performs cursor is captured
  - Parameters:
    - `opt_window` (`window`): Input parameter
- `cursor_set_relative_mode(window opt_window, b32 enabled) -> b32`
  - What it does: Sets cursor set relative mode
  - Parameters:
    - `opt_window` (`window`): Input parameter
    - `enabled` (`b32`): Input parameter
- `cursor_is_relative_mode(window opt_window) -> b32`
  - What it does: Performs cursor is relative mode
  - Parameters:
    - `opt_window` (`window`): Input parameter
- `cursor_get_pos() -> cursor_pos`
  - What it does: Gets cursor get pos
  - Parameters: none
- `cursor_get_global_pos() -> cursor_pos`
  - What it does: Gets cursor get global pos
  - Parameters: none
- `cursor_get_relative_pos() -> cursor_pos`
  - What it does: Gets cursor get relative pos
  - Parameters: none
- `cursor_warp(window opt_window, f32 xpos, f32 ypos) -> b32`
  - What it does: Performs cursor warp
  - Parameters:
    - `opt_window` (`window`): Input parameter
    - `xpos` (`f32`): Input parameter
    - `ypos` (`f32`): Input parameter

### `interface/dialog.h`

- Types: **9**
- Functions: **5**

#### Types

- `dialog_message_kind` (enum)
  - What it represents: Represents a set of named constants for `dialog message kind`
  - Members:
    - `DIALOG_MESSAGE_KIND_INFORMATION`: `enum value`
    - `DIALOG_MESSAGE_KIND_WARNING`: `enum value`
    - `DIALOG_MESSAGE_KIND_ERROR`: `enum value`
- `dialog_message_box_flag` (enum)
  - What it represents: Represents a set of named constants for `dialog message box flag`
  - Members:
    - `DIALOG_MESSAGE_BOX_FLAG_ERROR`: `enum value`
    - `DIALOG_MESSAGE_BOX_FLAG_WARNING`: `enum value`
    - `DIALOG_MESSAGE_BOX_FLAG_INFORMATION`: `enum value`
    - `DIALOG_MESSAGE_BOX_FLAG_BUTTONS_LEFT_TO_RIGHT`: `enum value`
    - `DIALOG_MESSAGE_BOX_FLAG_BUTTONS_RIGHT_TO_LEFT`: `enum value`
- `dialog_message_box_button_flag` (enum)
  - What it represents: Represents a set of named constants for `dialog message box button flag`
  - Members:
    - `DIALOG_MESSAGE_BOX_BUTTON_FLAG_RETURNKEY_DEFAULT`: `enum value`
    - `DIALOG_MESSAGE_BOX_BUTTON_FLAG_ESCAPEKEY_DEFAULT`: `enum value`
- `dialog_message_box_button` (struct)
  - What it represents: Represents `dialog message box button` data grouped in a struct
  - Members:
    - `flags`: `u32`
    - `button_id`: `i32`
    - `text`: `cstr8`
- `dialog_message_box_color` (struct)
  - What it represents: Represents `dialog message box color` data grouped in a struct
  - Members:
    - `red`: `u8`
    - `green`: `u8`
    - `blue`: `u8`
- `dialog_message_box_color_scheme` (struct)
  - What it represents: Represents `dialog message box color scheme` data grouped in a struct
  - Members:
    - `background`: `dialog_message_box_color`
    - `text`: `dialog_message_box_color`
    - `button_border`: `dialog_message_box_color`
    - `button_background`: `dialog_message_box_color`
    - `button_selected`: `dialog_message_box_color`
- `dialog_message_box` (struct)
  - What it represents: Represents `dialog message box` data grouped in a struct
  - Members:
    - `flags`: `u32`
    - `title`: `cstr8`
    - `message`: `cstr8`
    - `buttons`: `const dialog_message_box_button*`
    - `button_count`: `sz`
    - `color_scheme`: `const dialog_message_box_color_scheme*`
- `dialog_file_filter` (struct)
  - What it represents: Represents `dialog file filter` data grouped in a struct
  - Members:
    - `name`: `cstr8`
    - `pattern`: `cstr8`
- `dialog_file_callback` (alias)
  - What it represents: Type alias used for `dialog file callback`
  - Members: none (alias/function type)

#### Functions

- `dialog_show_message(window owner, dialog_message_kind message_kind, cstr8 title, cstr8 message) -> b32`
  - What it does: Performs dialog show message
  - Parameters:
    - `owner` (`window`): Input parameter
    - `message_kind` (`dialog_message_kind`): Input parameter
    - `title` (`cstr8`): Input parameter
    - `message` (`cstr8`): Input parameter
- `dialog_show_message_box(window owner, const dialog_message_box* message_box, i32* out_button_id) -> b32`
  - What it does: Performs dialog show message box
  - Parameters:
    - `owner` (`window`): Input parameter
    - `message_box` (`const dialog_message_box*`): Input parameter
    - `out_button_id` (`i32*`): Output value written by the function
- `dialog_open_file(window owner, const dialog_file_filter* filters, sz filter_count, cstr8 default_location, b32 allow_many, dialog_file_callback* callback, void* user_data) -> b32`
  - What it does: Opens dialog open file
  - Parameters:
    - `owner` (`window`): Input parameter
    - `filters` (`const dialog_file_filter*`): Input parameter
    - `filter_count` (`sz`): Number of elements/items involved
    - `default_location` (`cstr8`): Input parameter
    - `allow_many` (`b32`): Input parameter
    - `callback` (`dialog_file_callback*`): Input parameter
    - `user_data` (`void*`): Input parameter
- `dialog_save_file(window owner, const dialog_file_filter* filters, sz filter_count, cstr8 default_location, dialog_file_callback* callback, void* user_data) -> b32`
  - What it does: Performs dialog save file
  - Parameters:
    - `owner` (`window`): Input parameter
    - `filters` (`const dialog_file_filter*`): Input parameter
    - `filter_count` (`sz`): Number of elements/items involved
    - `default_location` (`cstr8`): Input parameter
    - `callback` (`dialog_file_callback*`): Input parameter
    - `user_data` (`void*`): Input parameter
- `dialog_open_folder(window owner, cstr8 default_location, b32 allow_many, dialog_file_callback* callback, void* user_data) -> b32`
  - What it does: Opens dialog open folder
  - Parameters:
    - `owner` (`window`): Input parameter
    - `default_location` (`cstr8`): Input parameter
    - `allow_many` (`b32`): Input parameter
    - `callback` (`dialog_file_callback*`): Input parameter
    - `user_data` (`void*`): Input parameter

### `interface/dpi.h`

- Types: **1**
- Functions: **5**

#### Types

- `dpi_metrics` (struct)
  - What it represents: Represents `dpi metrics` data grouped in a struct
  - Members:
    - `content_scale`: `f32`
    - `pixel_density`: `f32`
    - `display_scale`: `f32`

#### Functions

- `dpi_metrics_make_default() -> dpi_metrics`
  - What it does: Performs dpi metrics make default
  - Parameters: none
- `dpi_get_monitor_metrics(monitor monitor_id, dpi_metrics* out_metrics) -> b32`
  - What it does: Gets dpi get monitor metrics
  - Parameters:
    - `monitor_id` (`monitor`): Input parameter
    - `out_metrics` (`dpi_metrics*`): Output value written by the function
- `dpi_get_window_metrics(window window_id, dpi_metrics* out_metrics) -> b32`
  - What it does: Gets dpi get window metrics
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `out_metrics` (`dpi_metrics*`): Output value written by the function
- `dpi_scale_f32(f32 value, f32 scale) -> f32`
  - What it does: Performs dpi scale f32
  - Parameters:
    - `value` (`f32`): Input parameter
    - `scale` (`f32`): Input parameter
- `dpi_scale_i32(i32 value, f32 scale) -> i32`
  - What it does: Performs dpi scale i32
  - Parameters:
    - `value` (`i32`): Input parameter
    - `scale` (`f32`): Input parameter

### `interface/icon.h`

- Types: **2**
- Functions: **9**

#### Types

- `icon` (alias)
  - What it represents: Type alias used for `icon`
  - Members: none (alias/function type)
- `icon_system` (enum)
  - What it represents: Represents a set of named constants for `icon system`
  - Members:
    - `ICON_SYSTEM_DEFAULT`: `enum value`
    - `ICON_SYSTEM_TEXT`: `enum value`
    - `ICON_SYSTEM_WAIT`: `enum value`
    - `ICON_SYSTEM_CROSSHAIR`: `enum value`
    - `ICON_SYSTEM_PROGRESS`: `enum value`
    - `ICON_SYSTEM_NWSE_RESIZE`: `enum value`
    - `ICON_SYSTEM_NESW_RESIZE`: `enum value`
    - `ICON_SYSTEM_EW_RESIZE`: `enum value`
    - `ICON_SYSTEM_NS_RESIZE`: `enum value`
    - `ICON_SYSTEM_MOVE`: `enum value`
    - `ICON_SYSTEM_NOT_ALLOWED`: `enum value`
    - `ICON_SYSTEM_POINTER`: `enum value`
    - `ICON_SYSTEM_NW_RESIZE`: `enum value`
    - `ICON_SYSTEM_N_RESIZE`: `enum value`
    - `ICON_SYSTEM_NE_RESIZE`: `enum value`
    - `ICON_SYSTEM_E_RESIZE`: `enum value`
    - `ICON_SYSTEM_SE_RESIZE`: `enum value`
    - `ICON_SYSTEM_S_RESIZE`: `enum value`
    - `ICON_SYSTEM_SW_RESIZE`: `enum value`
    - `ICON_SYSTEM_W_RESIZE`: `enum value`

#### Functions

- `icon_id_is_valid(icon src) -> b32`
  - What it does: Performs icon id is valid
  - Parameters:
    - `src` (`icon`): Input data used by the operation
- `icon_create_rgba(i32 width, i32 height, const u8* rgba_pixels, i32 hot_xpos, i32 hot_ypos) -> icon`
  - What it does: Creates icon create rgba resources or state
  - Parameters:
    - `width` (`i32`): Input parameter
    - `height` (`i32`): Input parameter
    - `rgba_pixels` (`const u8*`): Input parameter
    - `hot_xpos` (`i32`): Input parameter
    - `hot_ypos` (`i32`): Input parameter
- `icon_create_system(icon_system system_icon) -> icon`
  - What it does: Creates icon create system resources or state
  - Parameters:
    - `system_icon` (`icon_system`): Input parameter
- `icon_destroy(icon icon_id) -> b32`
  - What it does: Destroys icon destroy resources or state
  - Parameters:
    - `icon_id` (`icon`): Input parameter
- `icon_get_size(icon icon_id, i32* out_width, i32* out_height) -> b32`
  - What it does: Gets icon get size
  - Parameters:
    - `icon_id` (`icon`): Input parameter
    - `out_width` (`i32*`): Output value written by the function
    - `out_height` (`i32*`): Output value written by the function
- `icon_get_hotspot(icon icon_id, i32* out_xpos, i32* out_ypos) -> b32`
  - What it does: Gets icon get hotspot
  - Parameters:
    - `icon_id` (`icon`): Input parameter
    - `out_xpos` (`i32*`): Output value written by the function
    - `out_ypos` (`i32*`): Output value written by the function
- `icon_get_system(icon icon_id, icon_system* out_system_icon) -> b32`
  - What it does: Gets icon get system
  - Parameters:
    - `icon_id` (`icon`): Input parameter
    - `out_system_icon` (`icon_system*`): Output value written by the function
- `icon_is_rgba(icon icon_id) -> b32`
  - What it does: Performs icon is rgba
  - Parameters:
    - `icon_id` (`icon`): Input parameter
- `icon_is_system(icon icon_id) -> b32`
  - What it does: Performs icon is system
  - Parameters:
    - `icon_id` (`icon`): Input parameter

### `interface/monitor.h`

- Types: **3**
- Functions: **15**

#### Types

- `monitor` (alias)
  - What it represents: Type alias used for `monitor`
  - Members: none (alias/function type)
- `monitor_mode` (struct)
  - What it represents: Represents `monitor mode` data grouped in a struct
  - Members:
    - `width`: `i32`
    - `height`: `i32`
    - `pixel_format`: `u32`
    - `refresh_rate`: `f32`
- `monitor_orientation` (enum)
  - What it represents: Represents a set of named constants for `monitor orientation`
  - Members:
    - `MONITOR_ORIENTATION_UNKNOWN`: `enum value`
    - `MONITOR_ORIENTATION_LANDSCAPE`: `enum value`
    - `MONITOR_ORIENTATION_LANDSCAPE_FLIPPED`: `enum value`
    - `MONITOR_ORIENTATION_PORTRAIT`: `enum value`
    - `MONITOR_ORIENTATION_PORTRAIT_FLIPPED`: `enum value`

#### Functions

- `monitor_id_is_valid(monitor src) -> b32`
  - What it does: Performs monitor id is valid
  - Parameters:
    - `src` (`monitor`): Input data used by the operation
- `monitor_from_device(device src) -> monitor`
  - What it does: Performs monitor from device
  - Parameters:
    - `src` (`device`): Input data used by the operation
- `monitor_get_total_count() -> sz`
  - What it does: Gets monitor get total count
  - Parameters: none
- `monitor_get_from_idx(sz idx) -> monitor`
  - What it does: Gets monitor get from idx
  - Parameters:
    - `idx` (`sz`): Zero-based index used to select an item
- `monitor_get_primary_id() -> monitor`
  - What it does: Gets monitor get primary id
  - Parameters: none
- `monitor_get_bounds(monitor mon_id) -> r2_i32`
  - What it does: Gets monitor get bounds
  - Parameters:
    - `mon_id` (`monitor`): Input parameter
- `monitor_get_usable_bounds(monitor mon_id) -> r2_i32`
  - What it does: Gets monitor get usable bounds
  - Parameters:
    - `mon_id` (`monitor`): Input parameter
- `monitor_get_mode_count(monitor mon_id) -> sz`
  - What it does: Gets monitor get mode count
  - Parameters:
    - `mon_id` (`monitor`): Input parameter
- `monitor_get_mode(monitor mon_id, sz idx, monitor_mode* out_mode) -> b32`
  - What it does: Gets monitor get mode
  - Parameters:
    - `mon_id` (`monitor`): Input parameter
    - `idx` (`sz`): Zero-based index used to select an item
    - `out_mode` (`monitor_mode*`): Output value written by the function
- `monitor_get_current_mode(monitor mon_id, monitor_mode* out_mode) -> b32`
  - What it does: Gets monitor get current mode
  - Parameters:
    - `mon_id` (`monitor`): Input parameter
    - `out_mode` (`monitor_mode*`): Output value written by the function
- `monitor_get_desktop_mode(monitor mon_id, monitor_mode* out_mode) -> b32`
  - What it does: Gets monitor get desktop mode
  - Parameters:
    - `mon_id` (`monitor`): Input parameter
    - `out_mode` (`monitor_mode*`): Output value written by the function
- `monitor_get_name(monitor mon_id) -> cstr8`
  - What it does: Gets monitor get name
  - Parameters:
    - `mon_id` (`monitor`): Input parameter
- `monitor_get_refresh_rate(monitor mon_id) -> f32`
  - What it does: Gets monitor get refresh rate
  - Parameters:
    - `mon_id` (`monitor`): Input parameter
- `monitor_get_content_scale(monitor mon_id) -> f32`
  - What it does: Gets monitor get content scale
  - Parameters:
    - `mon_id` (`monitor`): Input parameter
- `monitor_get_orientation(monitor mon_id) -> monitor_orientation`
  - What it does: Gets monitor get orientation
  - Parameters:
    - `mon_id` (`monitor`): Input parameter

### `interface/text_input.h`

- Types: **0**
- Functions: **4**

#### Types

- None

#### Functions

- `text_input_start(window opt_window) -> b32`
  - What it does: Performs text input start
  - Parameters:
    - `opt_window` (`window`): Input parameter
- `text_input_stop(window opt_window) -> b32`
  - What it does: Performs text input stop
  - Parameters:
    - `opt_window` (`window`): Input parameter
- `text_input_is_active(window opt_window) -> b32`
  - What it does: Performs text input is active
  - Parameters:
    - `opt_window` (`window`): Input parameter
- `text_input_set_area(window opt_window, i32 xpos, i32 ypos, i32 width, i32 height) -> b32`
  - What it does: Sets text input set area
  - Parameters:
    - `opt_window` (`window`): Input parameter
    - `xpos` (`i32`): Input parameter
    - `ypos` (`i32`): Input parameter
    - `width` (`i32`): Input parameter
    - `height` (`i32`): Input parameter

### `interface/window.h`

- Types: **3**
- Functions: **33**

#### Types

- `window` (alias)
  - What it represents: Type alias used for `window`
  - Members: none (alias/function type)
- `window_mode` (enum)
  - What it represents: Represents a set of named constants for `window mode`
  - Members:
    - `WINDOW_MODE_WINDOWED`: `enum value`
    - `WINDOW_MODE_UNDECORATED`: `enum value`
    - `WINDOW_MODE_FULLSCREEN`: `enum value`
    - `WINDOW_MODE_WINDOWED_FULLSCREEN`: `enum value`
- `window_center_axis` (enum)
  - What it represents: Represents a set of named constants for `window center axis`
  - Members:
    - `WINDOW_CENTER_AXIS_NONE`: `enum value`
    - `WINDOW_CENTER_AXIS_HORIZONTAL`: `enum value`
    - `WINDOW_CENTER_AXIS_VERTICAL`: `enum value`
    - `WINDOW_CENTER_AXIS_BOTH`: `enum value`

#### Functions

- `window_id_is_valid(window src) -> b32`
  - What it does: Performs window id is valid
  - Parameters:
    - `src` (`window`): Input data used by the operation
- `window_get_total_count() -> sz`
  - What it does: Gets window get total count
  - Parameters: none
- `window_get_from_idx(sz idx) -> window`
  - What it does: Gets window get from idx
  - Parameters:
    - `idx` (`sz`): Zero-based index used to select an item
- `window_is_valid(window window_id) -> b32`
  - What it does: Performs window is valid
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_create(cstr8 title, i32 width, i32 height, window_mode mode, u64 flags) -> window`
  - What it does: Creates window create resources or state
  - Parameters:
    - `title` (`cstr8`): Input parameter
    - `width` (`i32`): Input parameter
    - `height` (`i32`): Input parameter
    - `mode` (`window_mode`): Configuration value controlling behavior
    - `flags` (`u64`): Configuration value controlling behavior
- `window_destroy(window window_id) -> b32`
  - What it does: Destroys window destroy resources or state
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_is_hidden(window window_id) -> b32`
  - What it does: Performs window is hidden
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_show(window window_id) -> b32`
  - What it does: Performs window show
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_hide(window window_id) -> b32`
  - What it does: Performs window hide
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_set_pos(window window_id, i32 xpos, i32 ypos) -> b32`
  - What it does: Sets window set pos
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `xpos` (`i32`): Input parameter
    - `ypos` (`i32`): Input parameter
- `window_get_pos(window window_id, i32* out_xpos, i32* out_ypos) -> b32`
  - What it does: Gets window get pos
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `out_xpos` (`i32*`): Output value written by the function
    - `out_ypos` (`i32*`): Output value written by the function
- `window_set_size(window window_id, i32 width, i32 height) -> b32`
  - What it does: Sets window set size
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `width` (`i32`): Input parameter
    - `height` (`i32`): Input parameter
- `window_get_size(window window_id, i32* out_width, i32* out_height) -> b32`
  - What it does: Gets window get size
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `out_width` (`i32*`): Output value written by the function
    - `out_height` (`i32*`): Output value written by the function
- `window_set_mode(window window_id, window_mode mode) -> b32`
  - What it does: Sets window set mode
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `mode` (`window_mode`): Configuration value controlling behavior
- `window_get_mode(window window_id) -> window_mode`
  - What it does: Gets window get mode
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_is_minimized(window window_id) -> b32`
  - What it does: Performs window is minimized
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_is_maximized(window window_id) -> b32`
  - What it does: Performs window is maximized
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_has_input_focus(window window_id) -> b32`
  - What it does: Performs window has input focus
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_has_mouse_focus(window window_id) -> b32`
  - What it does: Performs window has mouse focus
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_is_topmost(window window_id) -> b32`
  - What it does: Performs window is topmost
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_minimize(window window_id) -> b32`
  - What it does: Performs window minimize
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_maximize(window window_id) -> b32`
  - What it does: Performs window maximize
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_restore(window window_id) -> b32`
  - What it does: Performs window restore
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_focus(window window_id) -> b32`
  - What it does: Performs window focus
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_set_topmost(window window_id, b32 enabled) -> b32`
  - What it does: Sets window set topmost
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `enabled` (`b32`): Input parameter
- `window_get_title(window window_id) -> cstr8`
  - What it does: Gets window get title
  - Parameters:
    - `window_id` (`window`): Input parameter
- `window_set_title(window window_id, cstr8 title) -> b32`
  - What it does: Sets window set title
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `title` (`cstr8`): Input parameter
- `window_set_icon(window window_id, icon icon_id) -> b32`
  - What it does: Sets window set icon
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `icon_id` (`icon`): Input parameter
- `window_get_monitor(window window_id, monitor* out_monitor_id) -> b32`
  - What it does: Gets window get monitor
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `out_monitor_id` (`monitor*`): Output value written by the function
- `window_get_creation_monitor(window window_id, monitor* out_monitor_id) -> b32`
  - What it does: Gets window get creation monitor
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `out_monitor_id` (`monitor*`): Output value written by the function
- `window_center_in_monitor(window window_id, monitor monitor_id, window_center_axis axis_mask) -> b32`
  - What it does: Performs window center in monitor
  - Parameters:
    - `window_id` (`window`): Input parameter
    - `monitor_id` (`monitor`): Input parameter
    - `axis_mask` (`window_center_axis`): Input parameter
- `window_get_keyboard_focus() -> window`
  - What it does: Gets window get keyboard focus
  - Parameters: none
- `window_get_cursor_focus() -> window`
  - What it does: Gets window get cursor focus
  - Parameters: none

## memory

Allocators, arenas, buffers and memory utility APIs.

### `memory/allocator.h`

- Types: **4**
- Functions: **4**

#### Types

- `allocator_callback_realloc` (alias)
  - What it represents: Type alias used for `allocator callback realloc`
  - Members: none (alias/function type)
- `allocator_callback_alloc` (alias)
  - What it represents: Type alias used for `allocator callback alloc`
  - Members: none (alias/function type)
- `allocator_callback_free` (alias)
  - What it represents: Type alias used for `allocator callback free`
  - Members: none (alias/function type)
- `allocator` (struct)
  - What it represents: Represents `allocator` data grouped in a struct
  - Members:
    - `user_data`: `void*`
    - `alloc_fn`: `allocator_callback_alloc*`
    - `dealloc_fn`: `allocator_callback_free*`
    - `realloc_fn`: `allocator_callback_realloc*`

#### Functions

- `_allocator_alloc(allocator alloc, sz size, callsite site) -> void*`
  - What it does: Performs  allocator alloc
  - Parameters:
    - `alloc` (`allocator`): Allocator/context used for memory management
    - `size` (`sz`): Size or capacity value used by the operation
    - `site` (`callsite`): Input parameter
- `_allocator_calloc(allocator alloc, sz count, sz size, callsite site) -> void*`
  - What it does: Performs  allocator calloc
  - Parameters:
    - `alloc` (`allocator`): Allocator/context used for memory management
    - `count` (`sz`): Number of elements/items involved
    - `size` (`sz`): Size or capacity value used by the operation
    - `site` (`callsite`): Input parameter
- `_allocator_dealloc(allocator alloc, void* ptr, callsite site) -> void`
  - What it does: Performs  allocator dealloc
  - Parameters:
    - `alloc` (`allocator`): Allocator/context used for memory management
    - `ptr` (`void*`): Input data used by the operation
    - `site` (`callsite`): Input parameter
- `_allocator_realloc(allocator alloc, void* ptr, sz new_size, callsite site) -> void*`
  - What it does: Performs  allocator realloc
  - Parameters:
    - `alloc` (`allocator`): Allocator/context used for memory management
    - `ptr` (`void*`): Input data used by the operation
    - `new_size` (`sz`): Size or capacity value used by the operation
    - `site` (`callsite`): Input parameter

### `memory/arena.h`

- Types: **2**
- Functions: **13**

#### Types

- `arena_block` (struct)
  - What it represents: Represents `arena block` data grouped in a struct
  - Members:
    - `next`: `struct arena_block*`
    - `size`: `sz`
    - `used`: `sz`
    - `owned`: `b8`
- `arena` (struct)
  - What it represents: Represents `arena` data grouped in a struct
  - Members:
    - `blocks_head`: `arena_block*`
    - `blocks_tail`: `arena_block*`
    - `parent`: `allocator`
    - `opt_mutex`: `mutex`
    - `default_block_sz`: `sz`
    - `mutex_owned`: `b8`

#### Functions

- `_arena_create(allocator parent_alloc, mutex opt_mutex, sz default_block_sz, callsite site) -> arena`
  - What it does: Creates  arena create resources or state
  - Parameters:
    - `parent_alloc` (`allocator`): Allocator/context used for memory management
    - `opt_mutex` (`mutex`): Input parameter
    - `default_block_sz` (`sz`): Input parameter
    - `site` (`callsite`): Input parameter
- `_arena_create_mutexed(allocator parent_alloc, sz default_block_sz, callsite site) -> arena`
  - What it does: Creates  arena create mutexed resources or state
  - Parameters:
    - `parent_alloc` (`allocator`): Allocator/context used for memory management
    - `default_block_sz` (`sz`): Input parameter
    - `site` (`callsite`): Input parameter
- `_arena_destroy(arena* arn, callsite site) -> void`
  - What it does: Destroys  arena destroy resources or state
  - Parameters:
    - `arn` (`arena*`): Input parameter
    - `site` (`callsite`): Input parameter
- `arena_get_allocator(arena* arn) -> allocator`
  - What it does: Gets arena get allocator
  - Parameters:
    - `arn` (`arena*`): Input parameter
- `arena_add_block(arena* arn, void* ptr, sz size) -> void`
  - What it does: Adds arena add block
  - Parameters:
    - `arn` (`arena*`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `arena_remove_block(arena* arn, void* ptr) -> b32`
  - What it does: Removes arena remove block
  - Parameters:
    - `arn` (`arena*`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
- `_arena_alloc(arena* arn, sz size, sz align, callsite site) -> void*`
  - What it does: Performs  arena alloc
  - Parameters:
    - `arn` (`arena*`): Input parameter
    - `size` (`sz`): Size or capacity value used by the operation
    - `align` (`sz`): Input parameter
    - `site` (`callsite`): Input parameter
- `_arena_realloc(arena* arn, void* ptr, sz old_size, sz new_size, sz align, callsite site) -> void*`
  - What it does: Performs  arena realloc
  - Parameters:
    - `arn` (`arena*`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
    - `old_size` (`sz`): Size or capacity value used by the operation
    - `new_size` (`sz`): Size or capacity value used by the operation
    - `align` (`sz`): Input parameter
    - `site` (`callsite`): Input parameter
- `arena_clear(arena* arn) -> void`
  - What it does: Performs arena clear
  - Parameters:
    - `arn` (`arena*`): Input parameter
- `arena_block_count(arena* arn) -> sz`
  - What it does: Performs arena block count
  - Parameters:
    - `arn` (`arena*`): Input parameter
- `arena_total_size(arena* arn) -> sz`
  - What it does: Performs arena total size
  - Parameters:
    - `arn` (`arena*`): Input parameter
- `arena_total_used(arena* arn) -> sz`
  - What it does: Performs arena total used
  - Parameters:
    - `arn` (`arena*`): Input parameter
- `arena_total_free(arena* arn) -> sz`
  - What it does: Performs arena total free
  - Parameters:
    - `arn` (`arena*`): Input parameter

### `memory/buffer.h`

- Types: **1**
- Functions: **15**

#### Types

- `buffer` (struct)
  - What it represents: Represents `buffer` data grouped in a struct
  - Members:
    - `size`: `sz`
    - `ptr`: `void*`

#### Functions

- `buffer_from(void* ptr, sz size) -> buffer`
  - What it does: Performs buffer from
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `buffer_sub_from(buffer buff, sz offset) -> buffer`
  - What it does: Performs buffer sub from
  - Parameters:
    - `buff` (`buffer`): Input parameter
    - `offset` (`sz`): Input parameter
- `buffer_sub_from_sized(buffer buff, sz offset, sz size) -> buffer`
  - What it does: Performs buffer sub from sized
  - Parameters:
    - `buff` (`buffer`): Input parameter
    - `offset` (`sz`): Input parameter
    - `size` (`sz`): Size or capacity value used by the operation
- `buffer_slice(buffer buff, sz start, sz end) -> buffer`
  - What it does: Performs buffer slice
  - Parameters:
    - `buff` (`buffer`): Input parameter
    - `start` (`sz`): Input parameter
    - `end` (`sz`): Input parameter
- `buffer_split_offset(buffer* buff, sz offset) -> buffer`
  - What it does: Performs buffer split offset
  - Parameters:
    - `buff` (`buffer*`): Input parameter
    - `offset` (`sz`): Input parameter
- `buffer_split_size(buffer* buff, sz size) -> buffer`
  - What it does: Performs buffer split size
  - Parameters:
    - `buff` (`buffer*`): Input parameter
    - `size` (`sz`): Size or capacity value used by the operation
- `buffer_cmp(buffer a, buffer b) -> b32`
  - What it does: Performs buffer cmp
  - Parameters:
    - `a` (`buffer`): Input parameter
    - `b` (`buffer`): Input parameter
- `buffer_cmp_common(buffer a, buffer b) -> b32`
  - What it does: Performs buffer cmp common
  - Parameters:
    - `a` (`buffer`): Input parameter
    - `b` (`buffer`): Input parameter
- `buffer_get_ptr(buffer buff, sz offset) -> void*`
  - What it does: Gets buffer get ptr
  - Parameters:
    - `buff` (`buffer`): Input parameter
    - `offset` (`sz`): Input parameter
- `buffer_get_data(buffer buff, sz offset, sz read_size) -> void*`
  - What it does: Gets buffer get data
  - Parameters:
    - `buff` (`buffer`): Input parameter
    - `offset` (`sz`): Input parameter
    - `read_size` (`sz`): Size or capacity value used by the operation
- `buffer_set8(buffer buff, u8 value) -> void`
  - What it does: Performs buffer set8
  - Parameters:
    - `buff` (`buffer`): Input parameter
    - `value` (`u8`): Input parameter
- `buffer_set16(buffer buff, u16 value) -> void`
  - What it does: Performs buffer set16
  - Parameters:
    - `buff` (`buffer`): Input parameter
    - `value` (`u16`): Input parameter
- `buffer_set32(buffer buff, u32 value) -> void`
  - What it does: Performs buffer set32
  - Parameters:
    - `buff` (`buffer`): Input parameter
    - `value` (`u32`): Input parameter
- `buffer_set64(buffer buff, u64 value) -> void`
  - What it does: Performs buffer set64
  - Parameters:
    - `buff` (`buffer`): Input parameter
    - `value` (`u64`): Input parameter
- `buffer_zero(buffer buff) -> void`
  - What it does: Performs buffer zero
  - Parameters:
    - `buff` (`buffer`): Input parameter

### `memory/heap.h`

- Types: **3**
- Functions: **13**

#### Types

- `heap_chunk` (struct)
  - What it represents: Represents `heap chunk` data grouped in a struct
  - Members:
    - `next_in_block`: `struct heap_chunk*`
    - `next_free`: `struct heap_chunk*`
    - `size`: `sz`
    - `align_pad`: `sz`
    - `is_free`: `b8`
- `heap_block` (struct)
  - What it represents: Represents `heap block` data grouped in a struct
  - Members:
    - `next`: `struct heap_block*`
    - `size`: `sz`
    - `owned`: `b8`
- `heap` (struct)
  - What it represents: Represents `heap` data grouped in a struct
  - Members:
    - `blocks_head`: `heap_block*`
    - `blocks_tail`: `heap_block*`
    - `free_head`: `heap_chunk*`
    - `parent`: `allocator`
    - `opt_mutex`: `mutex`
    - `default_block_sz`: `sz`
    - `mutex_owned`: `b8`

#### Functions

- `_heap_create(allocator parent_alloc, mutex opt_mutex, sz default_block_sz, callsite site) -> heap`
  - What it does: Creates  heap create resources or state
  - Parameters:
    - `parent_alloc` (`allocator`): Allocator/context used for memory management
    - `opt_mutex` (`mutex`): Input parameter
    - `default_block_sz` (`sz`): Input parameter
    - `site` (`callsite`): Input parameter
- `_heap_create_mutexed(allocator parent_alloc, sz default_block_sz, callsite site) -> heap`
  - What it does: Creates  heap create mutexed resources or state
  - Parameters:
    - `parent_alloc` (`allocator`): Allocator/context used for memory management
    - `default_block_sz` (`sz`): Input parameter
    - `site` (`callsite`): Input parameter
- `_heap_destroy(heap* hep, callsite site) -> void`
  - What it does: Destroys  heap destroy resources or state
  - Parameters:
    - `hep` (`heap*`): Input parameter
    - `site` (`callsite`): Input parameter
- `heap_get_allocator(heap* hep) -> allocator`
  - What it does: Gets heap get allocator
  - Parameters:
    - `hep` (`heap*`): Input parameter
- `heap_add_block(heap* hep, void* ptr, sz size) -> void`
  - What it does: Adds heap add block
  - Parameters:
    - `hep` (`heap*`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `heap_remove_block(heap* hep, void* ptr) -> b32`
  - What it does: Removes heap remove block
  - Parameters:
    - `hep` (`heap*`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
- `_heap_alloc(heap* hep, sz size, sz align, callsite site) -> void*`
  - What it does: Performs  heap alloc
  - Parameters:
    - `hep` (`heap*`): Input parameter
    - `size` (`sz`): Size or capacity value used by the operation
    - `align` (`sz`): Input parameter
    - `site` (`callsite`): Input parameter
- `_heap_dealloc(heap* hep, void* ptr, callsite site) -> void`
  - What it does: Performs  heap dealloc
  - Parameters:
    - `hep` (`heap*`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
    - `site` (`callsite`): Input parameter
- `_heap_realloc(heap* hep, void* ptr, sz old_size, sz new_size, sz align, callsite site) -> void*`
  - What it does: Performs  heap realloc
  - Parameters:
    - `hep` (`heap*`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
    - `old_size` (`sz`): Size or capacity value used by the operation
    - `new_size` (`sz`): Size or capacity value used by the operation
    - `align` (`sz`): Input parameter
    - `site` (`callsite`): Input parameter
- `heap_clear(heap* hep) -> void`
  - What it does: Performs heap clear
  - Parameters:
    - `hep` (`heap*`): Input parameter
- `heap_block_count(heap* hep) -> sz`
  - What it does: Performs heap block count
  - Parameters:
    - `hep` (`heap*`): Input parameter
- `heap_total_size(heap* hep) -> sz`
  - What it does: Performs heap total size
  - Parameters:
    - `hep` (`heap*`): Input parameter
- `heap_total_free(heap* hep) -> sz`
  - What it does: Performs heap total free
  - Parameters:
    - `hep` (`heap*`): Input parameter

### `memory/memops.h`

- Types: **0**
- Functions: **12**

#### Types

- None

#### Functions

- `mem_set8(void* ptr, u8 value, sz size) -> void`
  - What it does: Performs mem set8
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `value` (`u8`): Input parameter
    - `size` (`sz`): Size or capacity value used by the operation
- `mem_set16(void* ptr, u16 value, sz count) -> void`
  - What it does: Performs mem set16
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `value` (`u16`): Input parameter
    - `count` (`sz`): Number of elements/items involved
- `mem_set32(void* ptr, u32 value, sz count) -> void`
  - What it does: Performs mem set32
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `value` (`u32`): Input parameter
    - `count` (`sz`): Number of elements/items involved
- `mem_set64(void* ptr, u64 value, sz count) -> void`
  - What it does: Performs mem set64
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `value` (`u64`): Input parameter
    - `count` (`sz`): Number of elements/items involved
- `mem_zero(void* ptr, sz size) -> void`
  - What it does: Performs mem zero
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `mem_cpy(void* dst, const void* src, sz size) -> void*`
  - What it does: Performs mem cpy
  - Parameters:
    - `dst` (`void*`): Destination storage for output data
    - `src` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `mem_mv(void* dst, const void* src, sz size) -> void*`
  - What it does: Performs mem mv
  - Parameters:
    - `dst` (`void*`): Destination storage for output data
    - `src` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `mem_cmp(const void* lhs, const void* rhs, sz size) -> b32`
  - What it does: Performs mem cmp
  - Parameters:
    - `lhs` (`const void*`): Input data used by the operation
    - `rhs` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `mem_align_forward(void* ptr, sz align) -> void*`
  - What it does: Performs mem align forward
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `align` (`sz`): Input parameter
- `mem_align_backward(void* ptr, sz align) -> void*`
  - What it does: Performs mem align backward
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `align` (`sz`): Input parameter
- `mem_align_forward_up(up ptr, sz align) -> up`
  - What it does: Performs mem align forward up
  - Parameters:
    - `ptr` (`up`): Input data used by the operation
    - `align` (`sz`): Input parameter
- `mem_align_backward_up(up ptr, sz align) -> up`
  - What it does: Performs mem align backward up
  - Parameters:
    - `ptr` (`up`): Input data used by the operation
    - `align` (`sz`): Input parameter

### `memory/pool.h`

- Types: **2**
- Functions: **12**

#### Types

- `pool_block` (struct)
  - What it represents: Represents `pool block` data grouped in a struct
  - Members:
    - `next`: `struct pool_block*`
    - `size`: `sz`
    - `owned`: `b8`
- `pool` (struct)
  - What it represents: Represents `pool` data grouped in a struct
  - Members:
    - `blocks_head`: `pool_block*`
    - `blocks_tail`: `pool_block*`
    - `free_head`: `void*`
    - `parent`: `allocator`
    - `opt_mutex`: `mutex`
    - `default_block_sz`: `sz`
    - `object_size`: `sz`
    - `object_align`: `sz`
    - `mutex_owned`: `b8`

#### Functions

- `_pool_create(allocator parent_alloc, mutex opt_mutex, sz default_block_sz, sz object_size, sz object_align, callsite site) -> pool`
  - What it does: Creates  pool create resources or state
  - Parameters:
    - `parent_alloc` (`allocator`): Allocator/context used for memory management
    - `opt_mutex` (`mutex`): Input parameter
    - `default_block_sz` (`sz`): Input parameter
    - `object_size` (`sz`): Size or capacity value used by the operation
    - `object_align` (`sz`): Input parameter
    - `site` (`callsite`): Input parameter
- `_pool_create_mutexed(allocator parent_alloc, sz default_block_sz, sz object_size, sz object_align, callsite site) -> pool`
  - What it does: Creates  pool create mutexed resources or state
  - Parameters:
    - `parent_alloc` (`allocator`): Allocator/context used for memory management
    - `default_block_sz` (`sz`): Input parameter
    - `object_size` (`sz`): Size or capacity value used by the operation
    - `object_align` (`sz`): Input parameter
    - `site` (`callsite`): Input parameter
- `_pool_destroy(pool* pol, callsite site) -> void`
  - What it does: Destroys  pool destroy resources or state
  - Parameters:
    - `pol` (`pool*`): Input parameter
    - `site` (`callsite`): Input parameter
- `pool_get_allocator(pool* pol) -> allocator`
  - What it does: Gets pool get allocator
  - Parameters:
    - `pol` (`pool*`): Input parameter
- `pool_add_block(pool* pol, void* ptr, sz size) -> void`
  - What it does: Adds pool add block
  - Parameters:
    - `pol` (`pool*`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `pool_remove_block(pool* pol, void* ptr) -> b32`
  - What it does: Removes pool remove block
  - Parameters:
    - `pol` (`pool*`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
- `_pool_alloc(pool* pol, callsite site) -> void*`
  - What it does: Performs  pool alloc
  - Parameters:
    - `pol` (`pool*`): Input parameter
    - `site` (`callsite`): Input parameter
- `_pool_dealloc(pool* pol, void* ptr, callsite site) -> void`
  - What it does: Performs  pool dealloc
  - Parameters:
    - `pol` (`pool*`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
    - `site` (`callsite`): Input parameter
- `pool_clear(pool* pol) -> void`
  - What it does: Performs pool clear
  - Parameters:
    - `pol` (`pool*`): Input parameter
- `pool_block_count(pool* pol) -> sz`
  - What it does: Performs pool block count
  - Parameters:
    - `pol` (`pool*`): Input parameter
- `pool_slot_size(pool* pol) -> sz`
  - What it does: Performs pool slot size
  - Parameters:
    - `pol` (`pool*`): Input parameter
- `pool_free_count(pool* pol) -> sz`
  - What it does: Performs pool free count
  - Parameters:
    - `pol` (`pool*`): Input parameter

### `memory/ring.h`

- Types: **1**
- Functions: **17**

#### Types

- `ring` (struct)
  - What it represents: Represents `ring` data grouped in a struct
  - Members:
    - `ptr`: `u8*`
    - `capacity`: `sz`
    - `read_pos`: `sz`
    - `write_pos`: `sz`
    - `count`: `sz`
    - `parent`: `allocator`
    - `opt_mutex`: `mutex`
    - `buf_owned`: `b8`
    - `mutex_owned`: `b8`

#### Functions

- `_ring_create(void* ptr, sz capacity, mutex opt_mutex, callsite site) -> ring`
  - What it does: Creates  ring create resources or state
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `capacity` (`sz`): Size or capacity value used by the operation
    - `opt_mutex` (`mutex`): Input parameter
    - `site` (`callsite`): Input parameter
- `_ring_create_mutexed(void* ptr, sz capacity, callsite site) -> ring`
  - What it does: Creates  ring create mutexed resources or state
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `capacity` (`sz`): Size or capacity value used by the operation
    - `site` (`callsite`): Input parameter
- `_ring_create_alloc(allocator parent_alloc, sz capacity, mutex opt_mutex, callsite site) -> ring`
  - What it does: Creates  ring create alloc resources or state
  - Parameters:
    - `parent_alloc` (`allocator`): Allocator/context used for memory management
    - `capacity` (`sz`): Size or capacity value used by the operation
    - `opt_mutex` (`mutex`): Input parameter
    - `site` (`callsite`): Input parameter
- `_ring_create_alloc_mutexed(allocator parent_alloc, sz capacity, callsite site) -> ring`
  - What it does: Creates  ring create alloc mutexed resources or state
  - Parameters:
    - `parent_alloc` (`allocator`): Allocator/context used for memory management
    - `capacity` (`sz`): Size or capacity value used by the operation
    - `site` (`callsite`): Input parameter
- `_ring_destroy(ring* rng, callsite site) -> void`
  - What it does: Destroys  ring destroy resources or state
  - Parameters:
    - `rng` (`ring*`): Input parameter
    - `site` (`callsite`): Input parameter
- `ring_size(ring* rng) -> sz`
  - What it does: Performs ring size
  - Parameters:
    - `rng` (`ring*`): Input parameter
- `ring_space(ring* rng) -> sz`
  - What it does: Performs ring space
  - Parameters:
    - `rng` (`ring*`): Input parameter
- `ring_capacity(ring* rng) -> sz`
  - What it does: Performs ring capacity
  - Parameters:
    - `rng` (`ring*`): Input parameter
- `ring_write(ring* rng, void* data, sz size) -> sz`
  - What it does: Writes data for ring write
  - Parameters:
    - `rng` (`ring*`): Input parameter
    - `data` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `ring_read(ring* rng, void* out, sz size) -> sz`
  - What it does: Reads data for ring read
  - Parameters:
    - `rng` (`ring*`): Input parameter
    - `out` (`void*`): Destination storage for output data
    - `size` (`sz`): Size or capacity value used by the operation
- `ring_peek(ring* rng, void* out, sz size) -> sz`
  - What it does: Performs ring peek
  - Parameters:
    - `rng` (`ring*`): Input parameter
    - `out` (`void*`): Destination storage for output data
    - `size` (`sz`): Size or capacity value used by the operation
- `ring_skip(ring* rng, sz size) -> sz`
  - What it does: Performs ring skip
  - Parameters:
    - `rng` (`ring*`): Input parameter
    - `size` (`sz`): Size or capacity value used by the operation
- `ring_reserve_write(ring* rng, sz* out_size) -> void*`
  - What it does: Writes data for ring reserve write
  - Parameters:
    - `rng` (`ring*`): Input parameter
    - `out_size` (`sz*`): Output value written by the function
- `ring_commit_write(ring* rng, sz size) -> b32`
  - What it does: Writes data for ring commit write
  - Parameters:
    - `rng` (`ring*`): Input parameter
    - `size` (`sz`): Size or capacity value used by the operation
- `ring_reserve_read(ring* rng, sz* out_size) -> const void*`
  - What it does: Reads data for ring reserve read
  - Parameters:
    - `rng` (`ring*`): Input parameter
    - `out_size` (`sz*`): Output value written by the function
- `ring_commit_read(ring* rng, sz size) -> b32`
  - What it does: Reads data for ring commit read
  - Parameters:
    - `rng` (`ring*`): Input parameter
    - `size` (`sz`): Size or capacity value used by the operation
- `ring_clear(ring* rng) -> void`
  - What it does: Performs ring clear
  - Parameters:
    - `rng` (`ring*`): Input parameter

### `memory/scratch.h`

- Types: **1**
- Functions: **2**

#### Types

- `scratch` (struct)
  - What it represents: Represents `scratch` data grouped in a struct
  - Members:
    - `arn`: `arena*`
    - `saved_tail`: `arena_block*`
    - `saved_tail_used`: `sz`

#### Functions

- `scratch_begin(arena* arn) -> scratch`
  - What it does: Begins scratch begin
  - Parameters:
    - `arn` (`arena*`): Input parameter
- `scratch_end(scratch* scr) -> void`
  - What it does: Ends scratch end
  - Parameters:
    - `scr` (`scratch*`): Input parameter

### `memory/vmem.h`

- Types: **1**
- Functions: **11**

#### Types

- `vmem_stats` (struct)
  - What it represents: Represents `vmem stats` data grouped in a struct
  - Members:
    - `page_size`: `sz`
    - `reserve_calls`: `u64`
    - `commit_calls`: `u64`
    - `decommit_calls`: `u64`
    - `release_calls`: `u64`
    - `reserved_bytes`: `sz`
    - `committed_bytes`: `sz`
    - `decommitted_bytes`: `sz`
    - `released_bytes`: `sz`
    - `alloc_calls`: `u64`
    - `calloc_calls`: `u64`
    - `realloc_calls`: `u64`
    - `free_calls`: `u64`
    - `live_allocations`: `u64`
    - `live_allocated_bytes`: `sz`
    - `peak_live_allocated_bytes`: `sz`
    - `total_allocated_bytes`: `sz`
    - `total_freed_bytes`: `sz`

#### Functions

- `vmem_page_size() -> sz`
  - What it does: Performs vmem page size
  - Parameters: none
- `vmem_reserve(sz size) -> void*`
  - What it does: Performs vmem reserve
  - Parameters:
    - `size` (`sz`): Size or capacity value used by the operation
- `vmem_commit(void* ptr, sz size) -> b32`
  - What it does: Performs vmem commit
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `vmem_decommit(void* ptr, sz size) -> b32`
  - What it does: Performs vmem decommit
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `vmem_release(void* ptr, sz size) -> b32`
  - What it does: Performs vmem release
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `vmem_alloc(sz size) -> void*`
  - What it does: Performs vmem alloc
  - Parameters:
    - `size` (`sz`): Size or capacity value used by the operation
- `vmem_calloc(sz count, sz size) -> void*`
  - What it does: Performs vmem calloc
  - Parameters:
    - `count` (`sz`): Number of elements/items involved
    - `size` (`sz`): Size or capacity value used by the operation
- `vmem_realloc(void* ptr, sz old_size, sz new_size) -> void*`
  - What it does: Performs vmem realloc
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `old_size` (`sz`): Size or capacity value used by the operation
    - `new_size` (`sz`): Size or capacity value used by the operation
- `vmem_free(void* ptr, sz size) -> b32`
  - What it does: Performs vmem free
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `vmem_get_allocator() -> allocator`
  - What it does: Gets vmem get allocator
  - Parameters: none
- `vmem_get_stats() -> vmem_stats`
  - What it does: Gets vmem get stats
  - Parameters: none

## processes

Process creation, inspection and pipe helpers.

### `processes/common_processes.h`

- Types: **0**
- Functions: **3**

#### Types

- None

#### Functions

- `process_open_weblink(cstr8 url) -> b32`
  - What it does: Opens process open weblink
  - Parameters:
    - `url` (`cstr8`): Input parameter
- `process_open_file_window(cstr8 location) -> b32`
  - What it does: Opens process open file window
  - Parameters:
    - `location` (`cstr8`): Input parameter
- `process_open_terminal(cstr8 location) -> b32`
  - What it does: Opens process open terminal
  - Parameters:
    - `location` (`cstr8`): Input parameter

### `processes/process.h`

- Types: **3**
- Functions: **14**

#### Types

- `process` (alias)
  - What it represents: Type alias used for `process`
  - Members: none (alias/function type)
- `process_snapshot_entry` (struct)
  - What it represents: Represents `process snapshot entry` data grouped in a struct
  - Members:
    - `id`: `u64`
    - `name`: `c8 [PROCESS_NAME_CAP]`
- `process_options` (struct)
  - What it represents: Represents `process options` data grouped in a struct
  - Members:
    - `pipe_stdin`: `b32`
    - `pipe_stdout`: `b32`
    - `pipe_stderr`: `b32`
    - `stderr_to_stdout`: `b32`
    - `background`: `b32`
    - `cwd`: `cstr8`
    - `envp`: `cstr8 const*`
    - `timeout_ms`: `i32`

#### Functions

- `process_options_default() -> process_options`
  - What it does: Performs process options default
  - Parameters: none
- `process_options_captured() -> process_options`
  - What it does: Performs process options captured
  - Parameters: none
- `_process_create(cstr8 const* args, callsite site) -> process`
  - What it does: Creates  process create resources or state
  - Parameters:
    - `args` (`cstr8 const*`): Input parameter
    - `site` (`callsite`): Input parameter
- `_process_create_with(cstr8 const* args, process_options options, callsite site) -> process`
  - What it does: Creates  process create with resources or state
  - Parameters:
    - `args` (`cstr8 const*`): Input parameter
    - `options` (`process_options`): Input parameter
    - `site` (`callsite`): Input parameter
- `process_is_valid(process prc) -> b32`
  - What it does: Performs process is valid
  - Parameters:
    - `prc` (`process`): Input parameter
- `process_get_id(process prc) -> u64`
  - What it does: Gets process get id
  - Parameters:
    - `prc` (`process`): Input parameter
- `process_snapshot_get(sz* out_count) -> process_snapshot_entry*`
  - What it does: Performs process snapshot get
  - Parameters:
    - `out_count` (`sz*`): Output value written by the function
- `process_snapshot_free(process_snapshot_entry* ptr) -> void`
  - What it does: Performs process snapshot free
  - Parameters:
    - `ptr` (`process_snapshot_entry*`): Input data used by the operation
- `process_read(process prc, sz* out_size, i32* out_exit_code) -> void*`
  - What it does: Reads data for process read
  - Parameters:
    - `prc` (`process`): Input parameter
    - `out_size` (`sz*`): Output value written by the function
    - `out_exit_code` (`i32*`): Output value written by the function
- `process_read_free(void* ptr) -> void`
  - What it does: Reads data for process read free
  - Parameters:
    - `ptr` (`void*`): Input data used by the operation
- `process_wait(process prc, b32 block, i32* out_exit_code) -> b32`
  - What it does: Performs process wait
  - Parameters:
    - `prc` (`process`): Input parameter
    - `block` (`b32`): Input parameter
    - `out_exit_code` (`i32*`): Output value written by the function
- `process_wait_timeout(process prc, i32 timeout_ms, i32* out_exit_code) -> b32`
  - What it does: Performs process wait timeout
  - Parameters:
    - `prc` (`process`): Input parameter
    - `timeout_ms` (`i32`): Input parameter
    - `out_exit_code` (`i32*`): Output value written by the function
- `process_kill(process prc, b32 force) -> b32`
  - What it does: Performs process kill
  - Parameters:
    - `prc` (`process`): Input parameter
    - `force` (`b32`): Input parameter
- `_process_destroy(process prc, callsite site) -> void`
  - What it does: Destroys  process destroy resources or state
  - Parameters:
    - `prc` (`process`): Input parameter
    - `site` (`callsite`): Input parameter

### `processes/process_current.h`

- Types: **1**
- Functions: **7**

#### Types

- `process_priority` (enum)
  - What it represents: Represents a set of named constants for `process priority`
  - Members:
    - `PROCESS_PRIORITY_LOW`: `enum value`
    - `PROCESS_PRIORITY_NORMAL`: `enum value`
    - `PROCESS_PRIORITY_HIGH`: `enum value`
    - `PROCESS_PRIORITY_REALTIME`: `enum value`

#### Functions

- `process_id() -> u64`
  - What it does: Performs process id
  - Parameters: none
- `process_get_priority() -> process_priority`
  - What it does: Gets process get priority
  - Parameters: none
- `process_set_priority(process_priority priority) -> b32`
  - What it does: Sets process set priority
  - Parameters:
    - `priority` (`process_priority`): Input parameter
- `process_is_unique() -> b32`
  - What it does: Performs process is unique
  - Parameters: none
- `process_restart() -> b32`
  - What it does: Performs process restart
  - Parameters: none
- `process_exit(i32 exit_code) -> no_return void`
  - What it does: Performs process exit
  - Parameters:
    - `exit_code` (`i32`): Input parameter
- `process_abort() -> no_return void`
  - What it does: Performs process abort
  - Parameters: none

### `processes/process_pipe.h`

- Types: **1**
- Functions: **11**

#### Types

- `process_pipe` (alias)
  - What it represents: Type alias used for `process pipe`
  - Members: none (alias/function type)

#### Functions

- `_process_pipe_stdin(process prc, callsite site) -> process_pipe`
  - What it does: Performs  process pipe stdin
  - Parameters:
    - `prc` (`process`): Input parameter
    - `site` (`callsite`): Input parameter
- `_process_pipe_stdout(process prc, callsite site) -> process_pipe`
  - What it does: Performs  process pipe stdout
  - Parameters:
    - `prc` (`process`): Input parameter
    - `site` (`callsite`): Input parameter
- `_process_pipe_stderr(process prc, callsite site) -> process_pipe`
  - What it does: Performs  process pipe stderr
  - Parameters:
    - `prc` (`process`): Input parameter
    - `site` (`callsite`): Input parameter
- `process_pipe_is_valid(process_pipe pip) -> b32`
  - What it does: Performs process pipe is valid
  - Parameters:
    - `pip` (`process_pipe`): Input parameter
- `process_pipe_read(process_pipe pip, void* ptr, sz size) -> sz`
  - What it does: Reads data for process pipe read
  - Parameters:
    - `pip` (`process_pipe`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `process_pipe_write(process_pipe pip, const void* ptr, sz size) -> sz`
  - What it does: Writes data for process pipe write
  - Parameters:
    - `pip` (`process_pipe`): Input parameter
    - `ptr` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `process_pipe_read_nonblocking(process_pipe pip, void* ptr, sz size) -> sz`
  - What it does: Reads data for process pipe read nonblocking
  - Parameters:
    - `pip` (`process_pipe`): Input parameter
    - `ptr` (`void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `process_pipe_write_nonblocking(process_pipe pip, const void* ptr, sz size) -> sz`
  - What it does: Writes data for process pipe write nonblocking
  - Parameters:
    - `pip` (`process_pipe`): Input parameter
    - `ptr` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `process_pipe_poll_readable(process_pipe pip, i32 timeout_ms) -> b32`
  - What it does: Reads data for process pipe poll readable
  - Parameters:
    - `pip` (`process_pipe`): Input parameter
    - `timeout_ms` (`i32`): Input parameter
- `process_pipe_flush(process_pipe pip) -> b32`
  - What it does: Performs process pipe flush
  - Parameters:
    - `pip` (`process_pipe`): Input parameter
- `_process_pipe_close(process_pipe pip, callsite site) -> void`
  - What it does: Closes  process pipe close
  - Parameters:
    - `pip` (`process_pipe`): Input parameter
    - `site` (`callsite`): Input parameter

## strings

Character, C-string and Unicode helpers.

### `strings/char.h`

- Types: **0**
- Functions: **48**

#### Types

- None

#### Functions

- `c8_is_alnum(c8 chr) -> b32`
  - What it does: Performs c8 is alnum
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_alpha(c8 chr) -> b32`
  - What it does: Performs c8 is alpha
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_blank(c8 chr) -> b32`
  - What it does: Performs c8 is blank
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_cntrl(c8 chr) -> b32`
  - What it does: Performs c8 is cntrl
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_digit(c8 chr) -> b32`
  - What it does: Performs c8 is digit
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_graph(c8 chr) -> b32`
  - What it does: Performs c8 is graph
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_lower(c8 chr) -> b32`
  - What it does: Performs c8 is lower
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_print(c8 chr) -> b32`
  - What it does: Performs c8 is print
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_punct(c8 chr) -> b32`
  - What it does: Performs c8 is punct
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_space(c8 chr) -> b32`
  - What it does: Performs c8 is space
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_upper(c8 chr) -> b32`
  - What it does: Performs c8 is upper
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_is_xdigit(c8 chr) -> b32`
  - What it does: Performs c8 is xdigit
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_to_lower(c8 chr) -> c8`
  - What it does: Performs c8 to lower
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_to_upper(c8 chr) -> c8`
  - What it does: Performs c8 to upper
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_hex_to_nibble(c8 chr) -> i32`
  - What it does: Performs c8 hex to nibble
  - Parameters:
    - `chr` (`c8`): Input parameter
- `c8_nibble_to_hex(u8 nibble) -> c8`
  - What it does: Performs c8 nibble to hex
  - Parameters:
    - `nibble` (`u8`): Input parameter
- `c16_is_alnum(c16 chr) -> b32`
  - What it does: Performs c16 is alnum
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_alpha(c16 chr) -> b32`
  - What it does: Performs c16 is alpha
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_blank(c16 chr) -> b32`
  - What it does: Performs c16 is blank
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_cntrl(c16 chr) -> b32`
  - What it does: Performs c16 is cntrl
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_digit(c16 chr) -> b32`
  - What it does: Performs c16 is digit
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_graph(c16 chr) -> b32`
  - What it does: Performs c16 is graph
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_lower(c16 chr) -> b32`
  - What it does: Performs c16 is lower
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_print(c16 chr) -> b32`
  - What it does: Performs c16 is print
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_punct(c16 chr) -> b32`
  - What it does: Performs c16 is punct
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_space(c16 chr) -> b32`
  - What it does: Performs c16 is space
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_upper(c16 chr) -> b32`
  - What it does: Performs c16 is upper
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_is_xdigit(c16 chr) -> b32`
  - What it does: Performs c16 is xdigit
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_to_lower(c16 chr) -> c16`
  - What it does: Performs c16 to lower
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_to_upper(c16 chr) -> c16`
  - What it does: Performs c16 to upper
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_hex_to_nibble(c16 chr) -> i32`
  - What it does: Performs c16 hex to nibble
  - Parameters:
    - `chr` (`c16`): Input parameter
- `c16_nibble_to_hex(u8 nibble) -> c16`
  - What it does: Performs c16 nibble to hex
  - Parameters:
    - `nibble` (`u8`): Input parameter
- `c32_is_alnum(c32 chr) -> b32`
  - What it does: Performs c32 is alnum
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_alpha(c32 chr) -> b32`
  - What it does: Performs c32 is alpha
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_blank(c32 chr) -> b32`
  - What it does: Performs c32 is blank
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_cntrl(c32 chr) -> b32`
  - What it does: Performs c32 is cntrl
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_digit(c32 chr) -> b32`
  - What it does: Performs c32 is digit
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_graph(c32 chr) -> b32`
  - What it does: Performs c32 is graph
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_lower(c32 chr) -> b32`
  - What it does: Performs c32 is lower
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_print(c32 chr) -> b32`
  - What it does: Performs c32 is print
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_punct(c32 chr) -> b32`
  - What it does: Performs c32 is punct
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_space(c32 chr) -> b32`
  - What it does: Performs c32 is space
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_upper(c32 chr) -> b32`
  - What it does: Performs c32 is upper
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_is_xdigit(c32 chr) -> b32`
  - What it does: Performs c32 is xdigit
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_to_lower(c32 chr) -> c32`
  - What it does: Performs c32 to lower
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_to_upper(c32 chr) -> c32`
  - What it does: Performs c32 to upper
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_hex_to_nibble(c32 chr) -> i32`
  - What it does: Performs c32 hex to nibble
  - Parameters:
    - `chr` (`c32`): Input parameter
- `c32_nibble_to_hex(u8 nibble) -> c32`
  - What it does: Performs c32 nibble to hex
  - Parameters:
    - `nibble` (`u8`): Input parameter

### `strings/cstrings.h`

- Types: **0**
- Functions: **111**

#### Types

- None

#### Functions

- `cstr8_len(cstr8 str) -> sz`
  - What it does: Performs cstr8 len
  - Parameters:
    - `str` (`cstr8`): Input parameter
- `cstr8_is_empty(cstr8 str) -> b32`
  - What it does: Performs cstr8 is empty
  - Parameters:
    - `str` (`cstr8`): Input parameter
- `cstr8_clear(c8* str) -> void`
  - What it does: Performs cstr8 clear
  - Parameters:
    - `str` (`c8*`): Input parameter
- `cstr8_cmp(cstr8 lhs, cstr8 rhs) -> b32`
  - What it does: Performs cstr8 cmp
  - Parameters:
    - `lhs` (`cstr8`): Input data used by the operation
    - `rhs` (`cstr8`): Input data used by the operation
- `cstr8_cmp_n(cstr8 lhs, cstr8 rhs, sz cnt) -> b32`
  - What it does: Performs cstr8 cmp n
  - Parameters:
    - `lhs` (`cstr8`): Input data used by the operation
    - `rhs` (`cstr8`): Input data used by the operation
    - `cnt` (`sz`): Input parameter
- `cstr8_cmp_nocase(cstr8 lhs, cstr8 rhs) -> b32`
  - What it does: Performs cstr8 cmp nocase
  - Parameters:
    - `lhs` (`cstr8`): Input data used by the operation
    - `rhs` (`cstr8`): Input data used by the operation
- `cstr8_cpy(c8* dst, sz dst_size, cstr8 src) -> sz`
  - What it does: Performs cstr8 cpy
  - Parameters:
    - `dst` (`c8*`): Destination storage for output data
    - `dst_size` (`sz`): Size or capacity value used by the operation
    - `src` (`cstr8`): Input data used by the operation
- `cstr8_cpy_n(c8* dst, sz dst_size, cstr8 src, sz cnt) -> sz`
  - What it does: Performs cstr8 cpy n
  - Parameters:
    - `dst` (`c8*`): Destination storage for output data
    - `dst_size` (`sz`): Size or capacity value used by the operation
    - `src` (`cstr8`): Input data used by the operation
    - `cnt` (`sz`): Input parameter
- `cstr8_cat(c8* dst, sz dst_cap, cstr8 src) -> sz`
  - What it does: Performs cstr8 cat
  - Parameters:
    - `dst` (`c8*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
    - `src` (`cstr8`): Input data used by the operation
- `cstr8_append_char(c8* dst, sz dst_cap, c8 chr) -> sz`
  - What it does: Performs cstr8 append char
  - Parameters:
    - `dst` (`c8*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
    - `chr` (`c8`): Input parameter
- `cstr8_truncate(c8* str, sz length) -> void`
  - What it does: Performs cstr8 truncate
  - Parameters:
    - `str` (`c8*`): Input parameter
    - `length` (`sz`): Input parameter
- `cstr8_format(c8* dst, sz dst_cap, cstr8 fmt, ...) -> b32`
  - What it does: Performs cstr8 format
  - Parameters:
    - `dst` (`c8*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
    - `fmt` (`cstr8`): Input parameter
    - `...` (`variadic`): Additional variadic arguments
- `cstr8_vformat(c8* dst, sz dst_cap, cstr8 fmt, va_list args) -> b32`
  - What it does: Performs cstr8 vformat
  - Parameters:
    - `dst` (`c8*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
    - `fmt` (`cstr8`): Input parameter
    - `args` (`va_list`): Input parameter
- `cstr8_append_format(c8* dst, sz dst_cap, cstr8 fmt, ...) -> b32`
  - What it does: Performs cstr8 append format
  - Parameters:
    - `dst` (`c8*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
    - `fmt` (`cstr8`): Input parameter
    - `...` (`variadic`): Additional variadic arguments
- `cstr8_append_vformat(c8* dst, sz dst_cap, cstr8 fmt, va_list args) -> b32`
  - What it does: Performs cstr8 append vformat
  - Parameters:
    - `dst` (`c8*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
    - `fmt` (`cstr8`): Input parameter
    - `args` (`va_list`): Input parameter
- `cstr8_scan(cstr8 str, cstr8 fmt, ...) -> b32`
  - What it does: Performs cstr8 scan
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `fmt` (`cstr8`): Input parameter
    - `...` (`variadic`): Additional variadic arguments
- `cstr8_find(cstr8 str, cstr8 sub) -> cstr8`
  - What it does: Performs cstr8 find
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `sub` (`cstr8`): Input parameter
- `cstr8_find_last(cstr8 str, cstr8 sub) -> cstr8`
  - What it does: Performs cstr8 find last
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `sub` (`cstr8`): Input parameter
- `cstr8_find_char(cstr8 str, c8 chr) -> cstr8`
  - What it does: Performs cstr8 find char
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `chr` (`c8`): Input parameter
- `cstr8_find_last_char(cstr8 str, c8 chr) -> cstr8`
  - What it does: Performs cstr8 find last char
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `chr` (`c8`): Input parameter
- `cstr8_count_char(cstr8 str, c8 chr) -> sz`
  - What it does: Performs cstr8 count char
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `chr` (`c8`): Input parameter
- `cstr8_starts_with(cstr8 str, cstr8 prefix) -> b32`
  - What it does: Performs cstr8 starts with
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `prefix` (`cstr8`): Input parameter
- `cstr8_ends_with(cstr8 str, cstr8 suffix) -> b32`
  - What it does: Ends cstr8 ends with
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `suffix` (`cstr8`): Input parameter
- `cstr8_hash32(cstr8 str) -> u32`
  - What it does: Performs cstr8 hash32
  - Parameters:
    - `str` (`cstr8`): Input parameter
- `cstr8_hash64(cstr8 str) -> u64`
  - What it does: Performs cstr8 hash64
  - Parameters:
    - `str` (`cstr8`): Input parameter
- `cstr8_to_upper(c8* str) -> void`
  - What it does: Performs cstr8 to upper
  - Parameters:
    - `str` (`c8*`): Input parameter
- `cstr8_to_lower(c8* str) -> void`
  - What it does: Performs cstr8 to lower
  - Parameters:
    - `str` (`c8*`): Input parameter
- `cstr8_trim(c8* str) -> void`
  - What it does: Performs cstr8 trim
  - Parameters:
    - `str` (`c8*`): Input parameter
- `cstr8_replace_char(c8* str, c8 from_chr, c8 to_chr) -> void`
  - What it does: Performs cstr8 replace char
  - Parameters:
    - `str` (`c8*`): Input parameter
    - `from_chr` (`c8`): Input parameter
    - `to_chr` (`c8`): Input parameter
- `cstr8_remove_char(c8* str, c8 chr) -> sz`
  - What it does: Removes cstr8 remove char
  - Parameters:
    - `str` (`c8*`): Input parameter
    - `chr` (`c8`): Input parameter
- `cstr8_remove_whitespace(c8* str) -> sz`
  - What it does: Removes cstr8 remove whitespace
  - Parameters:
    - `str` (`c8*`): Input parameter
- `cstr8_remove_prefix(c8* str, cstr8 prefix) -> b32`
  - What it does: Removes cstr8 remove prefix
  - Parameters:
    - `str` (`c8*`): Input parameter
    - `prefix` (`cstr8`): Input parameter
- `cstr8_remove_suffix(c8* str, cstr8 suffix) -> b32`
  - What it does: Removes cstr8 remove suffix
  - Parameters:
    - `str` (`c8*`): Input parameter
    - `suffix` (`cstr8`): Input parameter
- `cstr8_replace(c8* str, sz str_cap, cstr8 from, cstr8 rep) -> sz`
  - What it does: Performs cstr8 replace
  - Parameters:
    - `str` (`c8*`): Input parameter
    - `str_cap` (`sz`): Input parameter
    - `from` (`cstr8`): Input parameter
    - `rep` (`cstr8`): Input parameter
- `cstr8_common_prefix(cstr8 lhs, cstr8 rhs, c8* buf, sz buf_cap) -> sz`
  - What it does: Performs cstr8 common prefix
  - Parameters:
    - `lhs` (`cstr8`): Input data used by the operation
    - `rhs` (`cstr8`): Input data used by the operation
    - `buf` (`c8*`): Input parameter
    - `buf_cap` (`sz`): Input parameter
- `cstr8_beautify(c8* str) -> void`
  - What it does: Performs cstr8 beautify
  - Parameters:
    - `str` (`c8*`): Input parameter
- `cstr8_to_i64(cstr8 str, i64* out) -> b32`
  - What it does: Performs cstr8 to i64
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `out` (`i64*`): Destination storage for output data
- `cstr8_to_u64(cstr8 str, u64 max_value, u64* out) -> b32`
  - What it does: Performs cstr8 to u64
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `max_value` (`u64`): Input parameter
    - `out` (`u64*`): Destination storage for output data
- `cstr8_to_f64(cstr8 str, f64* out) -> b32`
  - What it does: Performs cstr8 to f64
  - Parameters:
    - `str` (`cstr8`): Input parameter
    - `out` (`f64*`): Destination storage for output data
- `cstr16_len(cstr16 str) -> sz`
  - What it does: Performs cstr16 len
  - Parameters:
    - `str` (`cstr16`): Input parameter
- `cstr16_is_empty(cstr16 str) -> b32`
  - What it does: Performs cstr16 is empty
  - Parameters:
    - `str` (`cstr16`): Input parameter
- `cstr16_clear(c16* str) -> void`
  - What it does: Performs cstr16 clear
  - Parameters:
    - `str` (`c16*`): Input parameter
- `cstr16_cmp(cstr16 lhs, cstr16 rhs) -> b32`
  - What it does: Performs cstr16 cmp
  - Parameters:
    - `lhs` (`cstr16`): Input data used by the operation
    - `rhs` (`cstr16`): Input data used by the operation
- `cstr16_cmp_n(cstr16 lhs, cstr16 rhs, sz cnt) -> b32`
  - What it does: Performs cstr16 cmp n
  - Parameters:
    - `lhs` (`cstr16`): Input data used by the operation
    - `rhs` (`cstr16`): Input data used by the operation
    - `cnt` (`sz`): Input parameter
- `cstr16_cmp_nocase(cstr16 lhs, cstr16 rhs) -> b32`
  - What it does: Performs cstr16 cmp nocase
  - Parameters:
    - `lhs` (`cstr16`): Input data used by the operation
    - `rhs` (`cstr16`): Input data used by the operation
- `cstr16_cpy(c16* dst, sz dst_size, cstr16 src) -> sz`
  - What it does: Performs cstr16 cpy
  - Parameters:
    - `dst` (`c16*`): Destination storage for output data
    - `dst_size` (`sz`): Size or capacity value used by the operation
    - `src` (`cstr16`): Input data used by the operation
- `cstr16_cpy_n(c16* dst, sz dst_size, cstr16 src, sz cnt) -> sz`
  - What it does: Performs cstr16 cpy n
  - Parameters:
    - `dst` (`c16*`): Destination storage for output data
    - `dst_size` (`sz`): Size or capacity value used by the operation
    - `src` (`cstr16`): Input data used by the operation
    - `cnt` (`sz`): Input parameter
- `cstr16_cat(c16* dst, sz dst_cap, cstr16 src) -> sz`
  - What it does: Performs cstr16 cat
  - Parameters:
    - `dst` (`c16*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
    - `src` (`cstr16`): Input data used by the operation
- `cstr16_append_char(c16* dst, sz dst_cap, c16 chr) -> sz`
  - What it does: Performs cstr16 append char
  - Parameters:
    - `dst` (`c16*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
    - `chr` (`c16`): Input parameter
- `cstr16_truncate(c16* str, sz length) -> void`
  - What it does: Performs cstr16 truncate
  - Parameters:
    - `str` (`c16*`): Input parameter
    - `length` (`sz`): Input parameter
- `cstr16_find(cstr16 str, cstr16 sub) -> cstr16`
  - What it does: Performs cstr16 find
  - Parameters:
    - `str` (`cstr16`): Input parameter
    - `sub` (`cstr16`): Input parameter
- `cstr16_find_last(cstr16 str, cstr16 sub) -> cstr16`
  - What it does: Performs cstr16 find last
  - Parameters:
    - `str` (`cstr16`): Input parameter
    - `sub` (`cstr16`): Input parameter
- `cstr16_find_char(cstr16 str, c16 chr) -> cstr16`
  - What it does: Performs cstr16 find char
  - Parameters:
    - `str` (`cstr16`): Input parameter
    - `chr` (`c16`): Input parameter
- `cstr16_find_last_char(cstr16 str, c16 chr) -> cstr16`
  - What it does: Performs cstr16 find last char
  - Parameters:
    - `str` (`cstr16`): Input parameter
    - `chr` (`c16`): Input parameter
- `cstr16_count_char(cstr16 str, c16 chr) -> sz`
  - What it does: Performs cstr16 count char
  - Parameters:
    - `str` (`cstr16`): Input parameter
    - `chr` (`c16`): Input parameter
- `cstr16_starts_with(cstr16 str, cstr16 prefix) -> b32`
  - What it does: Performs cstr16 starts with
  - Parameters:
    - `str` (`cstr16`): Input parameter
    - `prefix` (`cstr16`): Input parameter
- `cstr16_ends_with(cstr16 str, cstr16 suffix) -> b32`
  - What it does: Ends cstr16 ends with
  - Parameters:
    - `str` (`cstr16`): Input parameter
    - `suffix` (`cstr16`): Input parameter
- `cstr16_hash32(cstr16 str) -> u32`
  - What it does: Performs cstr16 hash32
  - Parameters:
    - `str` (`cstr16`): Input parameter
- `cstr16_hash64(cstr16 str) -> u64`
  - What it does: Performs cstr16 hash64
  - Parameters:
    - `str` (`cstr16`): Input parameter
- `cstr16_to_upper(c16* str) -> void`
  - What it does: Performs cstr16 to upper
  - Parameters:
    - `str` (`c16*`): Input parameter
- `cstr16_to_lower(c16* str) -> void`
  - What it does: Performs cstr16 to lower
  - Parameters:
    - `str` (`c16*`): Input parameter
- `cstr16_trim(c16* str) -> void`
  - What it does: Performs cstr16 trim
  - Parameters:
    - `str` (`c16*`): Input parameter
- `cstr16_replace_char(c16* str, c16 from_chr, c16 to_chr) -> void`
  - What it does: Performs cstr16 replace char
  - Parameters:
    - `str` (`c16*`): Input parameter
    - `from_chr` (`c16`): Input parameter
    - `to_chr` (`c16`): Input parameter
- `cstr16_remove_char(c16* str, c16 chr) -> sz`
  - What it does: Removes cstr16 remove char
  - Parameters:
    - `str` (`c16*`): Input parameter
    - `chr` (`c16`): Input parameter
- `cstr16_remove_whitespace(c16* str) -> sz`
  - What it does: Removes cstr16 remove whitespace
  - Parameters:
    - `str` (`c16*`): Input parameter
- `cstr16_remove_prefix(c16* str, cstr16 prefix) -> b32`
  - What it does: Removes cstr16 remove prefix
  - Parameters:
    - `str` (`c16*`): Input parameter
    - `prefix` (`cstr16`): Input parameter
- `cstr16_remove_suffix(c16* str, cstr16 suffix) -> b32`
  - What it does: Removes cstr16 remove suffix
  - Parameters:
    - `str` (`c16*`): Input parameter
    - `suffix` (`cstr16`): Input parameter
- `cstr16_replace(c16* str, sz str_cap, cstr16 from, cstr16 rep) -> sz`
  - What it does: Performs cstr16 replace
  - Parameters:
    - `str` (`c16*`): Input parameter
    - `str_cap` (`sz`): Input parameter
    - `from` (`cstr16`): Input parameter
    - `rep` (`cstr16`): Input parameter
- `cstr16_common_prefix(cstr16 lhs, cstr16 rhs, c16* buf, sz buf_cap) -> sz`
  - What it does: Performs cstr16 common prefix
  - Parameters:
    - `lhs` (`cstr16`): Input data used by the operation
    - `rhs` (`cstr16`): Input data used by the operation
    - `buf` (`c16*`): Input parameter
    - `buf_cap` (`sz`): Input parameter
- `cstr16_beautify(c16* str) -> void`
  - What it does: Performs cstr16 beautify
  - Parameters:
    - `str` (`c16*`): Input parameter
- `cstr16_to_i64(cstr16 str, i64* out) -> b32`
  - What it does: Performs cstr16 to i64
  - Parameters:
    - `str` (`cstr16`): Input parameter
    - `out` (`i64*`): Destination storage for output data
- `cstr16_to_f64(cstr16 str, f64* out) -> b32`
  - What it does: Performs cstr16 to f64
  - Parameters:
    - `str` (`cstr16`): Input parameter
    - `out` (`f64*`): Destination storage for output data
- `cstr32_len(cstr32 str) -> sz`
  - What it does: Performs cstr32 len
  - Parameters:
    - `str` (`cstr32`): Input parameter
- `cstr32_is_empty(cstr32 str) -> b32`
  - What it does: Performs cstr32 is empty
  - Parameters:
    - `str` (`cstr32`): Input parameter
- `cstr32_clear(c32* str) -> void`
  - What it does: Performs cstr32 clear
  - Parameters:
    - `str` (`c32*`): Input parameter
- `cstr32_cmp(cstr32 lhs, cstr32 rhs) -> b32`
  - What it does: Performs cstr32 cmp
  - Parameters:
    - `lhs` (`cstr32`): Input data used by the operation
    - `rhs` (`cstr32`): Input data used by the operation
- `cstr32_cmp_n(cstr32 lhs, cstr32 rhs, sz cnt) -> b32`
  - What it does: Performs cstr32 cmp n
  - Parameters:
    - `lhs` (`cstr32`): Input data used by the operation
    - `rhs` (`cstr32`): Input data used by the operation
    - `cnt` (`sz`): Input parameter
- `cstr32_cmp_nocase(cstr32 lhs, cstr32 rhs) -> b32`
  - What it does: Performs cstr32 cmp nocase
  - Parameters:
    - `lhs` (`cstr32`): Input data used by the operation
    - `rhs` (`cstr32`): Input data used by the operation
- `cstr32_cpy(c32* dst, sz dst_size, cstr32 src) -> sz`
  - What it does: Performs cstr32 cpy
  - Parameters:
    - `dst` (`c32*`): Destination storage for output data
    - `dst_size` (`sz`): Size or capacity value used by the operation
    - `src` (`cstr32`): Input data used by the operation
- `cstr32_cpy_n(c32* dst, sz dst_size, cstr32 src, sz cnt) -> sz`
  - What it does: Performs cstr32 cpy n
  - Parameters:
    - `dst` (`c32*`): Destination storage for output data
    - `dst_size` (`sz`): Size or capacity value used by the operation
    - `src` (`cstr32`): Input data used by the operation
    - `cnt` (`sz`): Input parameter
- `cstr32_cat(c32* dst, sz dst_cap, cstr32 src) -> sz`
  - What it does: Performs cstr32 cat
  - Parameters:
    - `dst` (`c32*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
    - `src` (`cstr32`): Input data used by the operation
- `cstr32_append_char(c32* dst, sz dst_cap, c32 chr) -> sz`
  - What it does: Performs cstr32 append char
  - Parameters:
    - `dst` (`c32*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
    - `chr` (`c32`): Input parameter
- `cstr32_truncate(c32* str, sz length) -> void`
  - What it does: Performs cstr32 truncate
  - Parameters:
    - `str` (`c32*`): Input parameter
    - `length` (`sz`): Input parameter
- `cstr32_find(cstr32 str, cstr32 sub) -> cstr32`
  - What it does: Performs cstr32 find
  - Parameters:
    - `str` (`cstr32`): Input parameter
    - `sub` (`cstr32`): Input parameter
- `cstr32_find_last(cstr32 str, cstr32 sub) -> cstr32`
  - What it does: Performs cstr32 find last
  - Parameters:
    - `str` (`cstr32`): Input parameter
    - `sub` (`cstr32`): Input parameter
- `cstr32_find_char(cstr32 str, c32 chr) -> cstr32`
  - What it does: Performs cstr32 find char
  - Parameters:
    - `str` (`cstr32`): Input parameter
    - `chr` (`c32`): Input parameter
- `cstr32_find_last_char(cstr32 str, c32 chr) -> cstr32`
  - What it does: Performs cstr32 find last char
  - Parameters:
    - `str` (`cstr32`): Input parameter
    - `chr` (`c32`): Input parameter
- `cstr32_count_char(cstr32 str, c32 chr) -> sz`
  - What it does: Performs cstr32 count char
  - Parameters:
    - `str` (`cstr32`): Input parameter
    - `chr` (`c32`): Input parameter
- `cstr32_starts_with(cstr32 str, cstr32 prefix) -> b32`
  - What it does: Performs cstr32 starts with
  - Parameters:
    - `str` (`cstr32`): Input parameter
    - `prefix` (`cstr32`): Input parameter
- `cstr32_ends_with(cstr32 str, cstr32 suffix) -> b32`
  - What it does: Ends cstr32 ends with
  - Parameters:
    - `str` (`cstr32`): Input parameter
    - `suffix` (`cstr32`): Input parameter
- `cstr32_hash32(cstr32 str) -> u32`
  - What it does: Performs cstr32 hash32
  - Parameters:
    - `str` (`cstr32`): Input parameter
- `cstr32_hash64(cstr32 str) -> u64`
  - What it does: Performs cstr32 hash64
  - Parameters:
    - `str` (`cstr32`): Input parameter
- `cstr32_to_upper(c32* str) -> void`
  - What it does: Performs cstr32 to upper
  - Parameters:
    - `str` (`c32*`): Input parameter
- `cstr32_to_lower(c32* str) -> void`
  - What it does: Performs cstr32 to lower
  - Parameters:
    - `str` (`c32*`): Input parameter
- `cstr32_trim(c32* str) -> void`
  - What it does: Performs cstr32 trim
  - Parameters:
    - `str` (`c32*`): Input parameter
- `cstr32_replace_char(c32* str, c32 from_chr, c32 to_chr) -> void`
  - What it does: Performs cstr32 replace char
  - Parameters:
    - `str` (`c32*`): Input parameter
    - `from_chr` (`c32`): Input parameter
    - `to_chr` (`c32`): Input parameter
- `cstr32_remove_char(c32* str, c32 chr) -> sz`
  - What it does: Removes cstr32 remove char
  - Parameters:
    - `str` (`c32*`): Input parameter
    - `chr` (`c32`): Input parameter
- `cstr32_remove_whitespace(c32* str) -> sz`
  - What it does: Removes cstr32 remove whitespace
  - Parameters:
    - `str` (`c32*`): Input parameter
- `cstr32_remove_prefix(c32* str, cstr32 prefix) -> b32`
  - What it does: Removes cstr32 remove prefix
  - Parameters:
    - `str` (`c32*`): Input parameter
    - `prefix` (`cstr32`): Input parameter
- `cstr32_remove_suffix(c32* str, cstr32 suffix) -> b32`
  - What it does: Removes cstr32 remove suffix
  - Parameters:
    - `str` (`c32*`): Input parameter
    - `suffix` (`cstr32`): Input parameter
- `cstr32_replace(c32* str, sz str_cap, cstr32 from, cstr32 rep) -> sz`
  - What it does: Performs cstr32 replace
  - Parameters:
    - `str` (`c32*`): Input parameter
    - `str_cap` (`sz`): Input parameter
    - `from` (`cstr32`): Input parameter
    - `rep` (`cstr32`): Input parameter
- `cstr32_common_prefix(cstr32 lhs, cstr32 rhs, c32* buf, sz buf_cap) -> sz`
  - What it does: Performs cstr32 common prefix
  - Parameters:
    - `lhs` (`cstr32`): Input data used by the operation
    - `rhs` (`cstr32`): Input data used by the operation
    - `buf` (`c32*`): Input parameter
    - `buf_cap` (`sz`): Input parameter
- `cstr32_beautify(c32* str) -> void`
  - What it does: Performs cstr32 beautify
  - Parameters:
    - `str` (`c32*`): Input parameter
- `cstr32_to_i64(cstr32 str, i64* out) -> b32`
  - What it does: Performs cstr32 to i64
  - Parameters:
    - `str` (`cstr32`): Input parameter
    - `out` (`i64*`): Destination storage for output data
- `cstr32_to_f64(cstr32 str, f64* out) -> b32`
  - What it does: Performs cstr32 to f64
  - Parameters:
    - `str` (`cstr32`): Input parameter
    - `out` (`f64*`): Destination storage for output data
- `cstr8_to_cstr16(cstr8 src, c16* buf, sz buf_cap) -> sz`
  - What it does: Performs cstr8 to cstr16
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `buf` (`c16*`): Input parameter
    - `buf_cap` (`sz`): Input parameter
- `cstr8_to_cstr32(cstr8 src, c32* buf, sz buf_cap) -> sz`
  - What it does: Performs cstr8 to cstr32
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `buf` (`c32*`): Input parameter
    - `buf_cap` (`sz`): Input parameter
- `cstr16_to_cstr8(cstr16 src, c8* buf, sz buf_cap) -> sz`
  - What it does: Performs cstr16 to cstr8
  - Parameters:
    - `src` (`cstr16`): Input data used by the operation
    - `buf` (`c8*`): Input parameter
    - `buf_cap` (`sz`): Input parameter
- `cstr16_to_cstr32(cstr16 src, c32* buf, sz buf_cap) -> sz`
  - What it does: Performs cstr16 to cstr32
  - Parameters:
    - `src` (`cstr16`): Input data used by the operation
    - `buf` (`c32*`): Input parameter
    - `buf_cap` (`sz`): Input parameter
- `cstr32_to_cstr8(cstr32 src, c8* buf, sz buf_cap) -> sz`
  - What it does: Performs cstr32 to cstr8
  - Parameters:
    - `src` (`cstr32`): Input data used by the operation
    - `buf` (`c8*`): Input parameter
    - `buf_cap` (`sz`): Input parameter
- `cstr32_to_cstr16(cstr32 src, c16* buf, sz buf_cap) -> sz`
  - What it does: Performs cstr32 to cstr16
  - Parameters:
    - `src` (`cstr32`): Input data used by the operation
    - `buf` (`c16*`): Input parameter
    - `buf_cap` (`sz`): Input parameter

### `strings/strings.h`

- Types: **3**
- Functions: **107**

#### Types

- `str8` (struct)
  - What it represents: Represents `str8` data grouped in a struct
  - Members:
    - `size`: `sz`
    - `cap`: `sz`
    - `ptr`: `c8*`
- `str16` (struct)
  - What it represents: Represents `str16` data grouped in a struct
  - Members:
    - `size`: `sz`
    - `cap`: `sz`
    - `ptr`: `c16*`
- `str32` (struct)
  - What it represents: Represents `str32` data grouped in a struct
  - Members:
    - `size`: `sz`
    - `cap`: `sz`
    - `ptr`: `c32*`

#### Functions

- `str8_make(c8* ptr, sz cap) -> str8`
  - What it does: Performs str8 make
  - Parameters:
    - `ptr` (`c8*`): Input data used by the operation
    - `cap` (`sz`): Input parameter
- `str8_empty(c8* ptr, sz cap) -> str8`
  - What it does: Performs str8 empty
  - Parameters:
    - `ptr` (`c8*`): Input data used by the operation
    - `cap` (`sz`): Input parameter
- `str8_from_cstr(c8* ptr, sz cap, cstr8 src) -> str8`
  - What it does: Performs str8 from cstr
  - Parameters:
    - `ptr` (`c8*`): Input data used by the operation
    - `cap` (`sz`): Input parameter
    - `src` (`cstr8`): Input data used by the operation
- `str8_is_empty(str8 str) -> b32`
  - What it does: Performs str8 is empty
  - Parameters:
    - `str` (`str8`): Input parameter
- `str8_cmp(str8 lhs, str8 rhs) -> b32`
  - What it does: Performs str8 cmp
  - Parameters:
    - `lhs` (`str8`): Input data used by the operation
    - `rhs` (`str8`): Input data used by the operation
- `str8_cmp_nocase(str8 lhs, str8 rhs) -> b32`
  - What it does: Performs str8 cmp nocase
  - Parameters:
    - `lhs` (`str8`): Input data used by the operation
    - `rhs` (`str8`): Input data used by the operation
- `str8_find(str8 str, cstr8 sub) -> cstr8`
  - What it does: Performs str8 find
  - Parameters:
    - `str` (`str8`): Input parameter
    - `sub` (`cstr8`): Input parameter
- `str8_find_char(str8 str, c8 chr) -> cstr8`
  - What it does: Performs str8 find char
  - Parameters:
    - `str` (`str8`): Input parameter
    - `chr` (`c8`): Input parameter
- `str8_find_last(str8 str, cstr8 sub) -> cstr8`
  - What it does: Performs str8 find last
  - Parameters:
    - `str` (`str8`): Input parameter
    - `sub` (`cstr8`): Input parameter
- `str8_find_last_char(str8 str, c8 chr) -> cstr8`
  - What it does: Performs str8 find last char
  - Parameters:
    - `str` (`str8`): Input parameter
    - `chr` (`c8`): Input parameter
- `str8_count_char(str8 str, c8 chr) -> sz`
  - What it does: Performs str8 count char
  - Parameters:
    - `str` (`str8`): Input parameter
    - `chr` (`c8`): Input parameter
- `str8_starts_with(str8 str, cstr8 prefix) -> b32`
  - What it does: Performs str8 starts with
  - Parameters:
    - `str` (`str8`): Input parameter
    - `prefix` (`cstr8`): Input parameter
- `str8_ends_with(str8 str, cstr8 suffix) -> b32`
  - What it does: Ends str8 ends with
  - Parameters:
    - `str` (`str8`): Input parameter
    - `suffix` (`cstr8`): Input parameter
- `str8_hash32(str8 str) -> u32`
  - What it does: Performs str8 hash32
  - Parameters:
    - `str` (`str8`): Input parameter
- `str8_hash64(str8 str) -> u64`
  - What it does: Performs str8 hash64
  - Parameters:
    - `str` (`str8`): Input parameter
- `str8_to_i64(str8 str, i64* out) -> b32`
  - What it does: Performs str8 to i64
  - Parameters:
    - `str` (`str8`): Input parameter
    - `out` (`i64*`): Destination storage for output data
- `str8_to_u64(str8 str, u64 max_value, u64* out) -> b32`
  - What it does: Performs str8 to u64
  - Parameters:
    - `str` (`str8`): Input parameter
    - `max_value` (`u64`): Input parameter
    - `out` (`u64*`): Destination storage for output data
- `str8_to_f64(str8 str, f64* out) -> b32`
  - What it does: Performs str8 to f64
  - Parameters:
    - `str` (`str8`): Input parameter
    - `out` (`f64*`): Destination storage for output data
- `str8_clear(str8* str) -> void`
  - What it does: Performs str8 clear
  - Parameters:
    - `str` (`str8*`): Input parameter
- `str8_cpy(str8* str, cstr8 src) -> sz`
  - What it does: Performs str8 cpy
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `src` (`cstr8`): Input data used by the operation
- `str8_cat(str8* str, cstr8 src) -> sz`
  - What it does: Performs str8 cat
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `src` (`cstr8`): Input data used by the operation
- `str8_append_char(str8* str, c8 chr) -> sz`
  - What it does: Performs str8 append char
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `chr` (`c8`): Input parameter
- `str8_truncate(str8* str, sz length) -> void`
  - What it does: Performs str8 truncate
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `length` (`sz`): Input parameter
- `str8_format(str8* str, cstr8 fmt, ...) -> b32`
  - What it does: Performs str8 format
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `fmt` (`cstr8`): Input parameter
    - `...` (`variadic`): Additional variadic arguments
- `str8_vformat(str8* str, cstr8 fmt, va_list args) -> b32`
  - What it does: Performs str8 vformat
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `fmt` (`cstr8`): Input parameter
    - `args` (`va_list`): Input parameter
- `str8_append_format(str8* str, cstr8 fmt, ...) -> b32`
  - What it does: Performs str8 append format
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `fmt` (`cstr8`): Input parameter
    - `...` (`variadic`): Additional variadic arguments
- `str8_append_vformat(str8* str, cstr8 fmt, va_list args) -> b32`
  - What it does: Performs str8 append vformat
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `fmt` (`cstr8`): Input parameter
    - `args` (`va_list`): Input parameter
- `str8_to_upper(str8* str) -> void`
  - What it does: Performs str8 to upper
  - Parameters:
    - `str` (`str8*`): Input parameter
- `str8_to_lower(str8* str) -> void`
  - What it does: Performs str8 to lower
  - Parameters:
    - `str` (`str8*`): Input parameter
- `str8_trim(str8* str) -> void`
  - What it does: Performs str8 trim
  - Parameters:
    - `str` (`str8*`): Input parameter
- `str8_replace_char(str8* str, c8 from_chr, c8 to_chr) -> void`
  - What it does: Performs str8 replace char
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `from_chr` (`c8`): Input parameter
    - `to_chr` (`c8`): Input parameter
- `str8_remove_char(str8* str, c8 chr) -> sz`
  - What it does: Removes str8 remove char
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `chr` (`c8`): Input parameter
- `str8_remove_whitespace(str8* str) -> sz`
  - What it does: Removes str8 remove whitespace
  - Parameters:
    - `str` (`str8*`): Input parameter
- `str8_remove_prefix(str8* str, cstr8 prefix) -> b32`
  - What it does: Removes str8 remove prefix
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `prefix` (`cstr8`): Input parameter
- `str8_remove_suffix(str8* str, cstr8 suffix) -> b32`
  - What it does: Removes str8 remove suffix
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `suffix` (`cstr8`): Input parameter
- `str8_replace(str8* str, cstr8 from, cstr8 rep) -> sz`
  - What it does: Performs str8 replace
  - Parameters:
    - `str` (`str8*`): Input parameter
    - `from` (`cstr8`): Input parameter
    - `rep` (`cstr8`): Input parameter
- `str8_beautify(str8* str) -> void`
  - What it does: Performs str8 beautify
  - Parameters:
    - `str` (`str8*`): Input parameter
- `str16_make(c16* ptr, sz cap) -> str16`
  - What it does: Performs str16 make
  - Parameters:
    - `ptr` (`c16*`): Input data used by the operation
    - `cap` (`sz`): Input parameter
- `str16_empty(c16* ptr, sz cap) -> str16`
  - What it does: Performs str16 empty
  - Parameters:
    - `ptr` (`c16*`): Input data used by the operation
    - `cap` (`sz`): Input parameter
- `str16_from_cstr(c16* ptr, sz cap, cstr16 src) -> str16`
  - What it does: Performs str16 from cstr
  - Parameters:
    - `ptr` (`c16*`): Input data used by the operation
    - `cap` (`sz`): Input parameter
    - `src` (`cstr16`): Input data used by the operation
- `str16_is_empty(str16 str) -> b32`
  - What it does: Performs str16 is empty
  - Parameters:
    - `str` (`str16`): Input parameter
- `str16_cmp(str16 lhs, str16 rhs) -> b32`
  - What it does: Performs str16 cmp
  - Parameters:
    - `lhs` (`str16`): Input data used by the operation
    - `rhs` (`str16`): Input data used by the operation
- `str16_cmp_nocase(str16 lhs, str16 rhs) -> b32`
  - What it does: Performs str16 cmp nocase
  - Parameters:
    - `lhs` (`str16`): Input data used by the operation
    - `rhs` (`str16`): Input data used by the operation
- `str16_find(str16 str, cstr16 sub) -> cstr16`
  - What it does: Performs str16 find
  - Parameters:
    - `str` (`str16`): Input parameter
    - `sub` (`cstr16`): Input parameter
- `str16_find_char(str16 str, c16 chr) -> cstr16`
  - What it does: Performs str16 find char
  - Parameters:
    - `str` (`str16`): Input parameter
    - `chr` (`c16`): Input parameter
- `str16_find_last(str16 str, cstr16 sub) -> cstr16`
  - What it does: Performs str16 find last
  - Parameters:
    - `str` (`str16`): Input parameter
    - `sub` (`cstr16`): Input parameter
- `str16_find_last_char(str16 str, c16 chr) -> cstr16`
  - What it does: Performs str16 find last char
  - Parameters:
    - `str` (`str16`): Input parameter
    - `chr` (`c16`): Input parameter
- `str16_count_char(str16 str, c16 chr) -> sz`
  - What it does: Performs str16 count char
  - Parameters:
    - `str` (`str16`): Input parameter
    - `chr` (`c16`): Input parameter
- `str16_starts_with(str16 str, cstr16 prefix) -> b32`
  - What it does: Performs str16 starts with
  - Parameters:
    - `str` (`str16`): Input parameter
    - `prefix` (`cstr16`): Input parameter
- `str16_ends_with(str16 str, cstr16 suffix) -> b32`
  - What it does: Ends str16 ends with
  - Parameters:
    - `str` (`str16`): Input parameter
    - `suffix` (`cstr16`): Input parameter
- `str16_hash32(str16 str) -> u32`
  - What it does: Performs str16 hash32
  - Parameters:
    - `str` (`str16`): Input parameter
- `str16_hash64(str16 str) -> u64`
  - What it does: Performs str16 hash64
  - Parameters:
    - `str` (`str16`): Input parameter
- `str16_to_i64(str16 str, i64* out) -> b32`
  - What it does: Performs str16 to i64
  - Parameters:
    - `str` (`str16`): Input parameter
    - `out` (`i64*`): Destination storage for output data
- `str16_to_f64(str16 str, f64* out) -> b32`
  - What it does: Performs str16 to f64
  - Parameters:
    - `str` (`str16`): Input parameter
    - `out` (`f64*`): Destination storage for output data
- `str16_clear(str16* str) -> void`
  - What it does: Performs str16 clear
  - Parameters:
    - `str` (`str16*`): Input parameter
- `str16_cpy(str16* str, cstr16 src) -> sz`
  - What it does: Performs str16 cpy
  - Parameters:
    - `str` (`str16*`): Input parameter
    - `src` (`cstr16`): Input data used by the operation
- `str16_cat(str16* str, cstr16 src) -> sz`
  - What it does: Performs str16 cat
  - Parameters:
    - `str` (`str16*`): Input parameter
    - `src` (`cstr16`): Input data used by the operation
- `str16_append_char(str16* str, c16 chr) -> sz`
  - What it does: Performs str16 append char
  - Parameters:
    - `str` (`str16*`): Input parameter
    - `chr` (`c16`): Input parameter
- `str16_truncate(str16* str, sz length) -> void`
  - What it does: Performs str16 truncate
  - Parameters:
    - `str` (`str16*`): Input parameter
    - `length` (`sz`): Input parameter
- `str16_to_upper(str16* str) -> void`
  - What it does: Performs str16 to upper
  - Parameters:
    - `str` (`str16*`): Input parameter
- `str16_to_lower(str16* str) -> void`
  - What it does: Performs str16 to lower
  - Parameters:
    - `str` (`str16*`): Input parameter
- `str16_trim(str16* str) -> void`
  - What it does: Performs str16 trim
  - Parameters:
    - `str` (`str16*`): Input parameter
- `str16_replace_char(str16* str, c16 from_chr, c16 to_chr) -> void`
  - What it does: Performs str16 replace char
  - Parameters:
    - `str` (`str16*`): Input parameter
    - `from_chr` (`c16`): Input parameter
    - `to_chr` (`c16`): Input parameter
- `str16_remove_char(str16* str, c16 chr) -> sz`
  - What it does: Removes str16 remove char
  - Parameters:
    - `str` (`str16*`): Input parameter
    - `chr` (`c16`): Input parameter
- `str16_remove_whitespace(str16* str) -> sz`
  - What it does: Removes str16 remove whitespace
  - Parameters:
    - `str` (`str16*`): Input parameter
- `str16_remove_prefix(str16* str, cstr16 prefix) -> b32`
  - What it does: Removes str16 remove prefix
  - Parameters:
    - `str` (`str16*`): Input parameter
    - `prefix` (`cstr16`): Input parameter
- `str16_remove_suffix(str16* str, cstr16 suffix) -> b32`
  - What it does: Removes str16 remove suffix
  - Parameters:
    - `str` (`str16*`): Input parameter
    - `suffix` (`cstr16`): Input parameter
- `str16_replace(str16* str, cstr16 from, cstr16 rep) -> sz`
  - What it does: Performs str16 replace
  - Parameters:
    - `str` (`str16*`): Input parameter
    - `from` (`cstr16`): Input parameter
    - `rep` (`cstr16`): Input parameter
- `str16_beautify(str16* str) -> void`
  - What it does: Performs str16 beautify
  - Parameters:
    - `str` (`str16*`): Input parameter
- `str32_make(c32* ptr, sz cap) -> str32`
  - What it does: Performs str32 make
  - Parameters:
    - `ptr` (`c32*`): Input data used by the operation
    - `cap` (`sz`): Input parameter
- `str32_empty(c32* ptr, sz cap) -> str32`
  - What it does: Performs str32 empty
  - Parameters:
    - `ptr` (`c32*`): Input data used by the operation
    - `cap` (`sz`): Input parameter
- `str32_from_cstr(c32* ptr, sz cap, cstr32 src) -> str32`
  - What it does: Performs str32 from cstr
  - Parameters:
    - `ptr` (`c32*`): Input data used by the operation
    - `cap` (`sz`): Input parameter
    - `src` (`cstr32`): Input data used by the operation
- `str32_is_empty(str32 str) -> b32`
  - What it does: Performs str32 is empty
  - Parameters:
    - `str` (`str32`): Input parameter
- `str32_cmp(str32 lhs, str32 rhs) -> b32`
  - What it does: Performs str32 cmp
  - Parameters:
    - `lhs` (`str32`): Input data used by the operation
    - `rhs` (`str32`): Input data used by the operation
- `str32_cmp_nocase(str32 lhs, str32 rhs) -> b32`
  - What it does: Performs str32 cmp nocase
  - Parameters:
    - `lhs` (`str32`): Input data used by the operation
    - `rhs` (`str32`): Input data used by the operation
- `str32_find(str32 str, cstr32 sub) -> cstr32`
  - What it does: Performs str32 find
  - Parameters:
    - `str` (`str32`): Input parameter
    - `sub` (`cstr32`): Input parameter
- `str32_find_char(str32 str, c32 chr) -> cstr32`
  - What it does: Performs str32 find char
  - Parameters:
    - `str` (`str32`): Input parameter
    - `chr` (`c32`): Input parameter
- `str32_find_last(str32 str, cstr32 sub) -> cstr32`
  - What it does: Performs str32 find last
  - Parameters:
    - `str` (`str32`): Input parameter
    - `sub` (`cstr32`): Input parameter
- `str32_find_last_char(str32 str, c32 chr) -> cstr32`
  - What it does: Performs str32 find last char
  - Parameters:
    - `str` (`str32`): Input parameter
    - `chr` (`c32`): Input parameter
- `str32_count_char(str32 str, c32 chr) -> sz`
  - What it does: Performs str32 count char
  - Parameters:
    - `str` (`str32`): Input parameter
    - `chr` (`c32`): Input parameter
- `str32_starts_with(str32 str, cstr32 prefix) -> b32`
  - What it does: Performs str32 starts with
  - Parameters:
    - `str` (`str32`): Input parameter
    - `prefix` (`cstr32`): Input parameter
- `str32_ends_with(str32 str, cstr32 suffix) -> b32`
  - What it does: Ends str32 ends with
  - Parameters:
    - `str` (`str32`): Input parameter
    - `suffix` (`cstr32`): Input parameter
- `str32_hash32(str32 str) -> u32`
  - What it does: Performs str32 hash32
  - Parameters:
    - `str` (`str32`): Input parameter
- `str32_hash64(str32 str) -> u64`
  - What it does: Performs str32 hash64
  - Parameters:
    - `str` (`str32`): Input parameter
- `str32_to_i64(str32 str, i64* out) -> b32`
  - What it does: Performs str32 to i64
  - Parameters:
    - `str` (`str32`): Input parameter
    - `out` (`i64*`): Destination storage for output data
- `str32_to_f64(str32 str, f64* out) -> b32`
  - What it does: Performs str32 to f64
  - Parameters:
    - `str` (`str32`): Input parameter
    - `out` (`f64*`): Destination storage for output data
- `str32_clear(str32* str) -> void`
  - What it does: Performs str32 clear
  - Parameters:
    - `str` (`str32*`): Input parameter
- `str32_cpy(str32* str, cstr32 src) -> sz`
  - What it does: Performs str32 cpy
  - Parameters:
    - `str` (`str32*`): Input parameter
    - `src` (`cstr32`): Input data used by the operation
- `str32_cat(str32* str, cstr32 src) -> sz`
  - What it does: Performs str32 cat
  - Parameters:
    - `str` (`str32*`): Input parameter
    - `src` (`cstr32`): Input data used by the operation
- `str32_append_char(str32* str, c32 chr) -> sz`
  - What it does: Performs str32 append char
  - Parameters:
    - `str` (`str32*`): Input parameter
    - `chr` (`c32`): Input parameter
- `str32_truncate(str32* str, sz length) -> void`
  - What it does: Performs str32 truncate
  - Parameters:
    - `str` (`str32*`): Input parameter
    - `length` (`sz`): Input parameter
- `str32_to_upper(str32* str) -> void`
  - What it does: Performs str32 to upper
  - Parameters:
    - `str` (`str32*`): Input parameter
- `str32_to_lower(str32* str) -> void`
  - What it does: Performs str32 to lower
  - Parameters:
    - `str` (`str32*`): Input parameter
- `str32_trim(str32* str) -> void`
  - What it does: Performs str32 trim
  - Parameters:
    - `str` (`str32*`): Input parameter
- `str32_replace_char(str32* str, c32 from_chr, c32 to_chr) -> void`
  - What it does: Performs str32 replace char
  - Parameters:
    - `str` (`str32*`): Input parameter
    - `from_chr` (`c32`): Input parameter
    - `to_chr` (`c32`): Input parameter
- `str32_remove_char(str32* str, c32 chr) -> sz`
  - What it does: Removes str32 remove char
  - Parameters:
    - `str` (`str32*`): Input parameter
    - `chr` (`c32`): Input parameter
- `str32_remove_whitespace(str32* str) -> sz`
  - What it does: Removes str32 remove whitespace
  - Parameters:
    - `str` (`str32*`): Input parameter
- `str32_remove_prefix(str32* str, cstr32 prefix) -> b32`
  - What it does: Removes str32 remove prefix
  - Parameters:
    - `str` (`str32*`): Input parameter
    - `prefix` (`cstr32`): Input parameter
- `str32_remove_suffix(str32* str, cstr32 suffix) -> b32`
  - What it does: Removes str32 remove suffix
  - Parameters:
    - `str` (`str32*`): Input parameter
    - `suffix` (`cstr32`): Input parameter
- `str32_replace(str32* str, cstr32 from, cstr32 rep) -> sz`
  - What it does: Performs str32 replace
  - Parameters:
    - `str` (`str32*`): Input parameter
    - `from` (`cstr32`): Input parameter
    - `rep` (`cstr32`): Input parameter
- `str32_beautify(str32* str) -> void`
  - What it does: Performs str32 beautify
  - Parameters:
    - `str` (`str32*`): Input parameter
- `str8_to_str16(str8 src, str16* dst) -> sz`
  - What it does: Performs str8 to str16
  - Parameters:
    - `src` (`str8`): Input data used by the operation
    - `dst` (`str16*`): Destination storage for output data
- `str8_to_str32(str8 src, str32* dst) -> sz`
  - What it does: Performs str8 to str32
  - Parameters:
    - `src` (`str8`): Input data used by the operation
    - `dst` (`str32*`): Destination storage for output data
- `str16_to_str8(str16 src, str8* dst) -> sz`
  - What it does: Performs str16 to str8
  - Parameters:
    - `src` (`str16`): Input data used by the operation
    - `dst` (`str8*`): Destination storage for output data
- `str16_to_str32(str16 src, str32* dst) -> sz`
  - What it does: Performs str16 to str32
  - Parameters:
    - `src` (`str16`): Input data used by the operation
    - `dst` (`str32*`): Destination storage for output data
- `str32_to_str8(str32 src, str8* dst) -> sz`
  - What it does: Performs str32 to str8
  - Parameters:
    - `src` (`str32`): Input data used by the operation
    - `dst` (`str8*`): Destination storage for output data
- `str32_to_str16(str32 src, str16* dst) -> sz`
  - What it does: Performs str32 to str16
  - Parameters:
    - `src` (`str32`): Input data used by the operation
    - `dst` (`str16*`): Destination storage for output data

### `strings/unicode.h`

- Types: **0**
- Functions: **16**

#### Types

- None

#### Functions

- `unicode_is_valid(c32 codepoint) -> b32`
  - What it does: Performs unicode is valid
  - Parameters:
    - `codepoint` (`c32`): Input parameter
- `utf8_encode_size(c32 codepoint) -> sz`
  - What it does: Performs utf8 encode size
  - Parameters:
    - `codepoint` (`c32`): Input parameter
- `utf8_byte_count(c8 first_byte) -> sz`
  - What it does: Performs utf8 byte count
  - Parameters:
    - `first_byte` (`c8`): Input parameter
- `utf8_decode(cstr8 ptr, sz* consumed) -> c32`
  - What it does: Performs utf8 decode
  - Parameters:
    - `ptr` (`cstr8`): Input data used by the operation
    - `consumed` (`sz*`): Input parameter
- `utf8_encode(c32 codepoint, c8* out) -> sz`
  - What it does: Performs utf8 encode
  - Parameters:
    - `codepoint` (`c32`): Input parameter
    - `out` (`c8*`): Destination storage for output data
- `utf8_codepoint_count(cstr8 src, sz src_size) -> sz`
  - What it does: Performs utf8 codepoint count
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `src_size` (`sz`): Size or capacity value used by the operation
- `utf16_encode_size(c32 codepoint) -> sz`
  - What it does: Performs utf16 encode size
  - Parameters:
    - `codepoint` (`c32`): Input parameter
- `utf16_decode(cstr16 ptr, sz* consumed) -> c32`
  - What it does: Performs utf16 decode
  - Parameters:
    - `ptr` (`cstr16`): Input data used by the operation
    - `consumed` (`sz*`): Input parameter
- `utf16_encode(c32 codepoint, c16* out) -> sz`
  - What it does: Performs utf16 encode
  - Parameters:
    - `codepoint` (`c32`): Input parameter
    - `out` (`c16*`): Destination storage for output data
- `utf16_codepoint_count(cstr16 src, sz src_size) -> sz`
  - What it does: Performs utf16 codepoint count
  - Parameters:
    - `src` (`cstr16`): Input data used by the operation
    - `src_size` (`sz`): Size or capacity value used by the operation
- `utf8_to_utf16(cstr8 src, sz src_size, c16* dst, sz dst_cap) -> sz`
  - What it does: Performs utf8 to utf16
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `src_size` (`sz`): Size or capacity value used by the operation
    - `dst` (`c16*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
- `utf8_to_utf32(cstr8 src, sz src_size, c32* dst, sz dst_cap) -> sz`
  - What it does: Performs utf8 to utf32
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `src_size` (`sz`): Size or capacity value used by the operation
    - `dst` (`c32*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
- `utf16_to_utf8(cstr16 src, sz src_size, c8* dst, sz dst_cap) -> sz`
  - What it does: Performs utf16 to utf8
  - Parameters:
    - `src` (`cstr16`): Input data used by the operation
    - `src_size` (`sz`): Size or capacity value used by the operation
    - `dst` (`c8*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
- `utf16_to_utf32(cstr16 src, sz src_size, c32* dst, sz dst_cap) -> sz`
  - What it does: Performs utf16 to utf32
  - Parameters:
    - `src` (`cstr16`): Input data used by the operation
    - `src_size` (`sz`): Size or capacity value used by the operation
    - `dst` (`c32*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
- `utf32_to_utf8(cstr32 src, sz src_size, c8* dst, sz dst_cap) -> sz`
  - What it does: Performs utf32 to utf8
  - Parameters:
    - `src` (`cstr32`): Input data used by the operation
    - `src_size` (`sz`): Size or capacity value used by the operation
    - `dst` (`c8*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter
- `utf32_to_utf16(cstr32 src, sz src_size, c16* dst, sz dst_cap) -> sz`
  - What it does: Performs utf32 to utf16
  - Parameters:
    - `src` (`cstr32`): Input data used by the operation
    - `src_size` (`sz`): Size or capacity value used by the operation
    - `dst` (`c16*`): Destination storage for output data
    - `dst_cap` (`sz`): Input parameter

## system

Hardware, runtime and platform information APIs.

### `system/cpu_info.h`

- Types: **2**
- Functions: **1**

#### Types

- `cpu_instruction_set_info` (struct)
  - What it represents: Represents `cpu instruction set info` data grouped in a struct
  - Members:
    - `mmx`: `b32`
    - `sse`: `b32`
    - `sse2`: `b32`
    - `sse3`: `b32`
    - `ssse3`: `b32`
    - `sse41`: `b32`
    - `sse42`: `b32`
    - `avx`: `b32`
    - `avx2`: `b32`
    - `avx512f`: `b32`
    - `aes`: `b32`
    - `fma`: `b32`
    - `bmi1`: `b32`
    - `bmi2`: `b32`
    - `popcnt`: `b32`
    - `neon`: `b32`
    - `crc32`: `b32`
- `cpu_info` (struct)
  - What it represents: Represents `cpu info` data grouped in a struct
  - Members:
    - `vendor_name`: `str8_short`
    - `brand_name`: `str8_short`
    - `logical_core_count`: `u32`
    - `cache_line_bytes`: `u32`
    - `instruction_sets`: `cpu_instruction_set_info`

#### Functions

- `cpu_info_query(cpu_info* out_info) -> b32`
  - What it does: Performs cpu info query
  - Parameters:
    - `out_info` (`cpu_info*`): Output value written by the function

### `system/hardware_info.h`

- Types: **1**
- Functions: **1**

#### Types

- `hardware_info` (struct)
  - What it represents: Represents `hardware info` data grouped in a struct
  - Members:
    - `cpu`: `cpu_info`
    - `memory_total`: `sz`
    - `memory_available`: `sz`
    - `monitor_count`: `u32`
    - `keyboard_count`: `u32`
    - `mouse_count`: `u32`
    - `gamepad_count`: `u32`
    - `video_driver_name`: `str8_short`
    - `primary_keyboard_name`: `str8_short`
    - `primary_mouse_name`: `str8_short`

#### Functions

- `hardware_info_query(hardware_info* out_info) -> b32`
  - What it does: Performs hardware info query
  - Parameters:
    - `out_info` (`hardware_info*`): Output value written by the function

### `system/language.h`

- Types: **1**
- Functions: **1**

#### Types

- `language_info` (struct)
  - What it represents: Represents `language info` data grouped in a struct
  - Members:
    - `desktop_language`: `str8_short`
    - `keyboard_language`: `str8_short`
    - `keyboard_layout_name`: `str8_short`

#### Functions

- `language_query(language_info* out_info) -> b32`
  - What it does: Performs language query
  - Parameters:
    - `out_info` (`language_info*`): Output value written by the function

### `system/system_info.h`

- Types: **1**
- Functions: **1**

#### Types

- `system_info` (struct)
  - What it represents: Represents `system info` data grouped in a struct
  - Members:
    - `os_name`: `str8_short`
    - `os_version`: `str8_short`
    - `architecture_name`: `str8_short`
    - `computer_name`: `str8_short`
    - `user_name`: `str8_short`
    - `user_home`: `str8_short`
    - `page_size`: `sz`
    - `allocation_granularity`: `sz`

#### Functions

- `system_info_query(system_info* out_info) -> b32`
  - What it does: Performs system info query
  - Parameters:
    - `out_info` (`system_info*`): Output value written by the function

### `system/system_runtime.h`

- Types: **1**
- Functions: **1**

#### Types

- `system_runtime_info` (struct)
  - What it represents: Represents `system runtime info` data grouped in a struct
  - Members:
    - `cpu_usage_percent`: `f32`
    - `memory_total`: `sz`
    - `memory_used`: `sz`
    - `memory_available`: `sz`
    - `process_memory_used`: `sz`
    - `process_memory_peak`: `sz`

#### Functions

- `system_runtime_query(system_runtime_info* out_info) -> b32`
  - What it does: Performs system runtime query
  - Parameters:
    - `out_info` (`system_runtime_info*`): Output value written by the function

## threads

Thread primitives and synchronization types.

### `threads/atomics.h`

- Types: **6**
- Functions: **73**

#### Types

- `atomic_i32` (struct)
  - What it represents: Represents `atomic i32` data grouped in a struct
  - Members:
    - `val`: `i32`
- `atomic_u32` (struct)
  - What it represents: Represents `atomic u32` data grouped in a struct
  - Members:
    - `val`: `u32`
- `atomic_i64` (struct)
  - What it represents: Represents `atomic i64` data grouped in a struct
  - Members:
    - `val`: `i64`
- `atomic_u64` (struct)
  - What it represents: Represents `atomic u64` data grouped in a struct
  - Members:
    - `val`: `u64`
- `atomic_ptr` (struct)
  - What it represents: Represents `atomic ptr` data grouped in a struct
  - Members:
    - `val`: `void*`
- `atomic_memory_order` (enum)
  - What it represents: Represents a set of named constants for `atomic memory order`
  - Members:
    - `ATOMIC_MEMORY_ORDER_RELAXED`: `enum value`
    - `ATOMIC_MEMORY_ORDER_CONSUME`: `enum value`
    - `ATOMIC_MEMORY_ORDER_ACQUIRE`: `enum value`
    - `ATOMIC_MEMORY_ORDER_RELEASE`: `enum value`
    - `ATOMIC_MEMORY_ORDER_ACQ_REL`: `enum value`
    - `ATOMIC_MEMORY_ORDER_SEQ_CST`: `enum value`

#### Functions

- `atomic_i32_get(atomic_i32* atom) -> i32`
  - What it does: Performs atomic i32 get
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
- `atomic_i32_get_explicit(atomic_i32* atom, atomic_memory_order order) -> i32`
  - What it does: Gets atomic i32 get explicit
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `order` (`atomic_memory_order`): Input parameter
- `atomic_i32_set(atomic_i32* atom, i32 val) -> i32`
  - What it does: Performs atomic i32 set
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `val` (`i32`): Input parameter
- `atomic_i32_set_explicit(atomic_i32* atom, i32 val, atomic_memory_order order) -> i32`
  - What it does: Sets atomic i32 set explicit
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `val` (`i32`): Input parameter
    - `order` (`atomic_memory_order`): Input parameter
- `atomic_i32_cmpex(atomic_i32* atom, i32* expected, i32 desired) -> b32`
  - What it does: Performs atomic i32 cmpex
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `expected` (`i32*`): Input parameter
    - `desired` (`i32`): Input parameter
- `atomic_i32_add(atomic_i32* atom, i32 delta) -> i32`
  - What it does: Adds atomic i32 add
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `delta` (`i32`): Input parameter
- `atomic_i32_and(atomic_i32* atom, i32 mask) -> i32`
  - What it does: Performs atomic i32 and
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `mask` (`i32`): Input parameter
- `atomic_i32_or(atomic_i32* atom, i32 mask) -> i32`
  - What it does: Performs atomic i32 or
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `mask` (`i32`): Input parameter
- `atomic_i32_xor(atomic_i32* atom, i32 mask) -> i32`
  - What it does: Performs atomic i32 xor
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `mask` (`i32`): Input parameter
- `atomic_i32_sub(atomic_i32* atom, i32 delta) -> i32`
  - What it does: Performs atomic i32 sub
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `delta` (`i32`): Input parameter
- `atomic_i32_eq(atomic_i32* atom, i32 val) -> b32`
  - What it does: Performs atomic i32 eq
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `val` (`i32`): Input parameter
- `atomic_i32_neq(atomic_i32* atom, i32 val) -> b32`
  - What it does: Performs atomic i32 neq
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `val` (`i32`): Input parameter
- `atomic_i32_lt(atomic_i32* atom, i32 val) -> b32`
  - What it does: Performs atomic i32 lt
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `val` (`i32`): Input parameter
- `atomic_i32_gt(atomic_i32* atom, i32 val) -> b32`
  - What it does: Performs atomic i32 gt
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `val` (`i32`): Input parameter
- `atomic_i32_lte(atomic_i32* atom, i32 val) -> b32`
  - What it does: Performs atomic i32 lte
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `val` (`i32`): Input parameter
- `atomic_i32_gte(atomic_i32* atom, i32 val) -> b32`
  - What it does: Performs atomic i32 gte
  - Parameters:
    - `atom` (`atomic_i32*`): Input parameter
    - `val` (`i32`): Input parameter
- `atomic_u32_get(atomic_u32* atom) -> u32`
  - What it does: Performs atomic u32 get
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
- `atomic_u32_get_explicit(atomic_u32* atom, atomic_memory_order order) -> u32`
  - What it does: Gets atomic u32 get explicit
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `order` (`atomic_memory_order`): Input parameter
- `atomic_u32_set(atomic_u32* atom, u32 val) -> u32`
  - What it does: Performs atomic u32 set
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `val` (`u32`): Input parameter
- `atomic_u32_set_explicit(atomic_u32* atom, u32 val, atomic_memory_order order) -> u32`
  - What it does: Sets atomic u32 set explicit
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `val` (`u32`): Input parameter
    - `order` (`atomic_memory_order`): Input parameter
- `atomic_u32_cmpex(atomic_u32* atom, u32* expected, u32 desired) -> b32`
  - What it does: Performs atomic u32 cmpex
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `expected` (`u32*`): Input parameter
    - `desired` (`u32`): Input parameter
- `atomic_u32_add(atomic_u32* atom, u32 delta) -> u32`
  - What it does: Adds atomic u32 add
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `delta` (`u32`): Input parameter
- `atomic_u32_and(atomic_u32* atom, u32 mask) -> u32`
  - What it does: Performs atomic u32 and
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `mask` (`u32`): Input parameter
- `atomic_u32_or(atomic_u32* atom, u32 mask) -> u32`
  - What it does: Performs atomic u32 or
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `mask` (`u32`): Input parameter
- `atomic_u32_xor(atomic_u32* atom, u32 mask) -> u32`
  - What it does: Performs atomic u32 xor
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `mask` (`u32`): Input parameter
- `atomic_u32_sub(atomic_u32* atom, u32 delta) -> u32`
  - What it does: Performs atomic u32 sub
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `delta` (`u32`): Input parameter
- `atomic_u32_eq(atomic_u32* atom, u32 val) -> b32`
  - What it does: Performs atomic u32 eq
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `val` (`u32`): Input parameter
- `atomic_u32_neq(atomic_u32* atom, u32 val) -> b32`
  - What it does: Performs atomic u32 neq
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `val` (`u32`): Input parameter
- `atomic_u32_lt(atomic_u32* atom, u32 val) -> b32`
  - What it does: Performs atomic u32 lt
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `val` (`u32`): Input parameter
- `atomic_u32_gt(atomic_u32* atom, u32 val) -> b32`
  - What it does: Performs atomic u32 gt
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `val` (`u32`): Input parameter
- `atomic_u32_lte(atomic_u32* atom, u32 val) -> b32`
  - What it does: Performs atomic u32 lte
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `val` (`u32`): Input parameter
- `atomic_u32_gte(atomic_u32* atom, u32 val) -> b32`
  - What it does: Performs atomic u32 gte
  - Parameters:
    - `atom` (`atomic_u32*`): Input parameter
    - `val` (`u32`): Input parameter
- `atomic_i64_get(atomic_i64* atom) -> i64`
  - What it does: Performs atomic i64 get
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
- `atomic_i64_get_explicit(atomic_i64* atom, atomic_memory_order order) -> i64`
  - What it does: Gets atomic i64 get explicit
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `order` (`atomic_memory_order`): Input parameter
- `atomic_i64_set(atomic_i64* atom, i64 val) -> i64`
  - What it does: Performs atomic i64 set
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `val` (`i64`): Input parameter
- `atomic_i64_set_explicit(atomic_i64* atom, i64 val, atomic_memory_order order) -> i64`
  - What it does: Sets atomic i64 set explicit
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `val` (`i64`): Input parameter
    - `order` (`atomic_memory_order`): Input parameter
- `atomic_i64_cmpex(atomic_i64* atom, i64* expected, i64 desired) -> b32`
  - What it does: Performs atomic i64 cmpex
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `expected` (`i64*`): Input parameter
    - `desired` (`i64`): Input parameter
- `atomic_i64_add(atomic_i64* atom, i64 delta) -> i64`
  - What it does: Adds atomic i64 add
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `delta` (`i64`): Input parameter
- `atomic_i64_and(atomic_i64* atom, i64 mask) -> i64`
  - What it does: Performs atomic i64 and
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `mask` (`i64`): Input parameter
- `atomic_i64_or(atomic_i64* atom, i64 mask) -> i64`
  - What it does: Performs atomic i64 or
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `mask` (`i64`): Input parameter
- `atomic_i64_xor(atomic_i64* atom, i64 mask) -> i64`
  - What it does: Performs atomic i64 xor
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `mask` (`i64`): Input parameter
- `atomic_i64_sub(atomic_i64* atom, i64 delta) -> i64`
  - What it does: Performs atomic i64 sub
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `delta` (`i64`): Input parameter
- `atomic_i64_eq(atomic_i64* atom, i64 val) -> b32`
  - What it does: Performs atomic i64 eq
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `val` (`i64`): Input parameter
- `atomic_i64_neq(atomic_i64* atom, i64 val) -> b32`
  - What it does: Performs atomic i64 neq
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `val` (`i64`): Input parameter
- `atomic_i64_lt(atomic_i64* atom, i64 val) -> b32`
  - What it does: Performs atomic i64 lt
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `val` (`i64`): Input parameter
- `atomic_i64_gt(atomic_i64* atom, i64 val) -> b32`
  - What it does: Performs atomic i64 gt
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `val` (`i64`): Input parameter
- `atomic_i64_lte(atomic_i64* atom, i64 val) -> b32`
  - What it does: Performs atomic i64 lte
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `val` (`i64`): Input parameter
- `atomic_i64_gte(atomic_i64* atom, i64 val) -> b32`
  - What it does: Performs atomic i64 gte
  - Parameters:
    - `atom` (`atomic_i64*`): Input parameter
    - `val` (`i64`): Input parameter
- `atomic_u64_get(atomic_u64* atom) -> u64`
  - What it does: Performs atomic u64 get
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
- `atomic_u64_get_explicit(atomic_u64* atom, atomic_memory_order order) -> u64`
  - What it does: Gets atomic u64 get explicit
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `order` (`atomic_memory_order`): Input parameter
- `atomic_u64_set(atomic_u64* atom, u64 val) -> u64`
  - What it does: Performs atomic u64 set
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `val` (`u64`): Input parameter
- `atomic_u64_set_explicit(atomic_u64* atom, u64 val, atomic_memory_order order) -> u64`
  - What it does: Sets atomic u64 set explicit
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `val` (`u64`): Input parameter
    - `order` (`atomic_memory_order`): Input parameter
- `atomic_u64_cmpex(atomic_u64* atom, u64* expected, u64 desired) -> b32`
  - What it does: Performs atomic u64 cmpex
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `expected` (`u64*`): Input parameter
    - `desired` (`u64`): Input parameter
- `atomic_u64_add(atomic_u64* atom, u64 delta) -> u64`
  - What it does: Adds atomic u64 add
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `delta` (`u64`): Input parameter
- `atomic_u64_and(atomic_u64* atom, u64 mask) -> u64`
  - What it does: Performs atomic u64 and
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `mask` (`u64`): Input parameter
- `atomic_u64_or(atomic_u64* atom, u64 mask) -> u64`
  - What it does: Performs atomic u64 or
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `mask` (`u64`): Input parameter
- `atomic_u64_xor(atomic_u64* atom, u64 mask) -> u64`
  - What it does: Performs atomic u64 xor
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `mask` (`u64`): Input parameter
- `atomic_u64_sub(atomic_u64* atom, u64 delta) -> u64`
  - What it does: Performs atomic u64 sub
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `delta` (`u64`): Input parameter
- `atomic_u64_eq(atomic_u64* atom, u64 val) -> b32`
  - What it does: Performs atomic u64 eq
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `val` (`u64`): Input parameter
- `atomic_u64_neq(atomic_u64* atom, u64 val) -> b32`
  - What it does: Performs atomic u64 neq
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `val` (`u64`): Input parameter
- `atomic_u64_lt(atomic_u64* atom, u64 val) -> b32`
  - What it does: Performs atomic u64 lt
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `val` (`u64`): Input parameter
- `atomic_u64_gt(atomic_u64* atom, u64 val) -> b32`
  - What it does: Performs atomic u64 gt
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `val` (`u64`): Input parameter
- `atomic_u64_lte(atomic_u64* atom, u64 val) -> b32`
  - What it does: Performs atomic u64 lte
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `val` (`u64`): Input parameter
- `atomic_u64_gte(atomic_u64* atom, u64 val) -> b32`
  - What it does: Performs atomic u64 gte
  - Parameters:
    - `atom` (`atomic_u64*`): Input parameter
    - `val` (`u64`): Input parameter
- `atomic_ptr_get(atomic_ptr* atom) -> void*`
  - What it does: Performs atomic ptr get
  - Parameters:
    - `atom` (`atomic_ptr*`): Input parameter
- `atomic_ptr_set(atomic_ptr* atom, void* val) -> void*`
  - What it does: Performs atomic ptr set
  - Parameters:
    - `atom` (`atomic_ptr*`): Input parameter
    - `val` (`void*`): Input parameter
- `atomic_ptr_cmpex(atomic_ptr* atom, void** expected, void* desired) -> b32`
  - What it does: Performs atomic ptr cmpex
  - Parameters:
    - `atom` (`atomic_ptr*`): Input parameter
    - `expected` (`void**`): Input parameter
    - `desired` (`void*`): Input parameter
- `atomic_ptr_eq(atomic_ptr* atom, void* val) -> b32`
  - What it does: Performs atomic ptr eq
  - Parameters:
    - `atom` (`atomic_ptr*`): Input parameter
    - `val` (`void*`): Input parameter
- `atomic_ptr_neq(atomic_ptr* atom, void* val) -> b32`
  - What it does: Performs atomic ptr neq
  - Parameters:
    - `atom` (`atomic_ptr*`): Input parameter
    - `val` (`void*`): Input parameter
- `atomic_fence_acquire() -> void`
  - What it does: Performs atomic fence acquire
  - Parameters: none
- `atomic_fence_release() -> void`
  - What it does: Performs atomic fence release
  - Parameters: none
- `atomic_fence() -> void`
  - What it does: Performs atomic fence
  - Parameters: none
- `atomic_pause() -> void`
  - What it does: Performs atomic pause
  - Parameters: none

### `threads/condvar.h`

- Types: **1**
- Functions: **7**

#### Types

- `condvar` (alias)
  - What it represents: Type alias used for `condvar`
  - Members: none (alias/function type)

#### Functions

- `_condvar_create(callsite site) -> condvar`
  - What it does: Creates  condvar create resources or state
  - Parameters:
    - `site` (`callsite`): Input parameter
- `_condvar_destroy(condvar cond, callsite site) -> b32`
  - What it does: Destroys  condvar destroy resources or state
  - Parameters:
    - `cond` (`condvar`): Input parameter
    - `site` (`callsite`): Input parameter
- `condvar_is_valid(condvar cond) -> b32`
  - What it does: Performs condvar is valid
  - Parameters:
    - `cond` (`condvar`): Input parameter
- `condvar_wait(condvar cond, mutex mtx) -> void`
  - What it does: Performs condvar wait
  - Parameters:
    - `cond` (`condvar`): Input parameter
    - `mtx` (`mutex`): Input parameter
- `condvar_wait_timeout(condvar cond, mutex mtx, u32 millis) -> b32`
  - What it does: Performs condvar wait timeout
  - Parameters:
    - `cond` (`condvar`): Input parameter
    - `mtx` (`mutex`): Input parameter
    - `millis` (`u32`): Input parameter
- `condvar_signal(condvar cond) -> void`
  - What it does: Performs condvar signal
  - Parameters:
    - `cond` (`condvar`): Input parameter
- `condvar_broadcast(condvar cond) -> void`
  - What it does: Performs condvar broadcast
  - Parameters:
    - `cond` (`condvar`): Input parameter

### `threads/mutex.h`

- Types: **1**
- Functions: **7**

#### Types

- `mutex` (alias)
  - What it represents: Type alias used for `mutex`
  - Members: none (alias/function type)

#### Functions

- `_mutex_create(callsite site) -> mutex`
  - What it does: Creates  mutex create resources or state
  - Parameters:
    - `site` (`callsite`): Input parameter
- `_mutex_destroy(mutex mtx, callsite site) -> b32`
  - What it does: Destroys  mutex destroy resources or state
  - Parameters:
    - `mtx` (`mutex`): Input parameter
    - `site` (`callsite`): Input parameter
- `mutex_is_valid(mutex mtx) -> b32`
  - What it does: Performs mutex is valid
  - Parameters:
    - `mtx` (`mutex`): Input parameter
- `mutex_lock(mutex mtx) -> void`
  - What it does: Performs mutex lock
  - Parameters:
    - `mtx` (`mutex`): Input parameter
- `mutex_trylock(mutex mtx) -> b32`
  - What it does: Performs mutex trylock
  - Parameters:
    - `mtx` (`mutex`): Input parameter
- `mutex_timed_lock(mutex mtx, i32 timeout_ms) -> b32`
  - What it does: Performs mutex timed lock
  - Parameters:
    - `mtx` (`mutex`): Input parameter
    - `timeout_ms` (`i32`): Input parameter
- `mutex_unlock(mutex mtx) -> void`
  - What it does: Performs mutex unlock
  - Parameters:
    - `mtx` (`mutex`): Input parameter

### `threads/rwlock.h`

- Types: **1**
- Functions: **11**

#### Types

- `rwlock` (alias)
  - What it represents: Type alias used for `rwlock`
  - Members: none (alias/function type)

#### Functions

- `_rwlock_create(callsite site) -> rwlock`
  - What it does: Creates  rwlock create resources or state
  - Parameters:
    - `site` (`callsite`): Input parameter
- `_rwlock_destroy(rwlock rw, callsite site) -> b32`
  - What it does: Destroys  rwlock destroy resources or state
  - Parameters:
    - `rw` (`rwlock`): Input parameter
    - `site` (`callsite`): Input parameter
- `rwlock_is_valid(rwlock rw) -> b32`
  - What it does: Performs rwlock is valid
  - Parameters:
    - `rw` (`rwlock`): Input parameter
- `rwlock_read_lock(rwlock rw) -> void`
  - What it does: Reads data for rwlock read lock
  - Parameters:
    - `rw` (`rwlock`): Input parameter
- `rwlock_read_unlock(rwlock rw) -> void`
  - What it does: Reads data for rwlock read unlock
  - Parameters:
    - `rw` (`rwlock`): Input parameter
- `rwlock_write_lock(rwlock rw) -> void`
  - What it does: Writes data for rwlock write lock
  - Parameters:
    - `rw` (`rwlock`): Input parameter
- `rwlock_write_unlock(rwlock rw) -> void`
  - What it does: Writes data for rwlock write unlock
  - Parameters:
    - `rw` (`rwlock`): Input parameter
- `rwlock_try_read_lock(rwlock rw) -> b32`
  - What it does: Reads data for rwlock try read lock
  - Parameters:
    - `rw` (`rwlock`): Input parameter
- `rwlock_try_write_lock(rwlock rw) -> b32`
  - What it does: Writes data for rwlock try write lock
  - Parameters:
    - `rw` (`rwlock`): Input parameter
- `rwlock_timed_read_lock(rwlock rw, i32 timeout_ms) -> b32`
  - What it does: Reads data for rwlock timed read lock
  - Parameters:
    - `rw` (`rwlock`): Input parameter
    - `timeout_ms` (`i32`): Input parameter
- `rwlock_timed_write_lock(rwlock rw, i32 timeout_ms) -> b32`
  - What it does: Writes data for rwlock timed write lock
  - Parameters:
    - `rw` (`rwlock`): Input parameter
    - `timeout_ms` (`i32`): Input parameter

### `threads/semaphore.h`

- Types: **1**
- Functions: **7**

#### Types

- `semaphore` (alias)
  - What it represents: Type alias used for `semaphore`
  - Members: none (alias/function type)

#### Functions

- `_semaphore_create(u32 initial_count, callsite site) -> semaphore`
  - What it does: Creates  semaphore create resources or state
  - Parameters:
    - `initial_count` (`u32`): Number of elements/items involved
    - `site` (`callsite`): Input parameter
- `_semaphore_destroy(semaphore sem, callsite site) -> b32`
  - What it does: Destroys  semaphore destroy resources or state
  - Parameters:
    - `sem` (`semaphore`): Input parameter
    - `site` (`callsite`): Input parameter
- `semaphore_is_valid(semaphore sem) -> b32`
  - What it does: Performs semaphore is valid
  - Parameters:
    - `sem` (`semaphore`): Input parameter
- `semaphore_acquire(semaphore sem) -> void`
  - What it does: Performs semaphore acquire
  - Parameters:
    - `sem` (`semaphore`): Input parameter
- `semaphore_try_acquire(semaphore sem) -> b32`
  - What it does: Performs semaphore try acquire
  - Parameters:
    - `sem` (`semaphore`): Input parameter
- `semaphore_acquire_timeout(semaphore sem, u32 millis) -> b32`
  - What it does: Performs semaphore acquire timeout
  - Parameters:
    - `sem` (`semaphore`): Input parameter
    - `millis` (`u32`): Input parameter
- `semaphore_release(semaphore sem) -> void`
  - What it does: Performs semaphore release
  - Parameters:
    - `sem` (`semaphore`): Input parameter

### `threads/spinlock.h`

- Types: **1**
- Functions: **6**

#### Types

- `spinlock` (alias)
  - What it represents: Type alias used for `spinlock`
  - Members: none (alias/function type)

#### Functions

- `_spinlock_create(callsite site) -> spinlock`
  - What it does: Creates  spinlock create resources or state
  - Parameters:
    - `site` (`callsite`): Input parameter
- `_spinlock_destroy(spinlock sl, callsite site) -> b32`
  - What it does: Destroys  spinlock destroy resources or state
  - Parameters:
    - `sl` (`spinlock`): Input parameter
    - `site` (`callsite`): Input parameter
- `spinlock_is_valid(spinlock sl) -> b32`
  - What it does: Performs spinlock is valid
  - Parameters:
    - `sl` (`spinlock`): Input parameter
- `spinlock_lock(spinlock sl) -> void`
  - What it does: Performs spinlock lock
  - Parameters:
    - `sl` (`spinlock`): Input parameter
- `spinlock_unlock(spinlock sl) -> void`
  - What it does: Performs spinlock unlock
  - Parameters:
    - `sl` (`spinlock`): Input parameter
- `spinlock_try_lock(spinlock sl) -> b32`
  - What it does: Performs spinlock try lock
  - Parameters:
    - `sl` (`spinlock`): Input parameter

### `threads/thread.h`

- Types: **2**
- Functions: **7**

#### Types

- `thread` (alias)
  - What it represents: Type alias used for `thread`
  - Members: none (alias/function type)
- `thread_func` (alias)
  - What it represents: Type alias used for `thread func`
  - Members: none (alias/function type)

#### Functions

- `_thread_create(thread_func entry, void* arg, ctx_setup setup, callsite site) -> thread`
  - What it does: Creates  thread create resources or state
  - Parameters:
    - `entry` (`thread_func`): Input parameter
    - `arg` (`void*`): Input parameter
    - `setup` (`ctx_setup`): Input parameter
    - `site` (`callsite`): Input parameter
- `_thread_create_named(thread_func entry, void* arg, cstr8 name, ctx_setup setup, callsite site) -> thread`
  - What it does: Creates  thread create named resources or state
  - Parameters:
    - `entry` (`thread_func`): Input parameter
    - `arg` (`void*`): Input parameter
    - `name` (`cstr8`): Input parameter
    - `setup` (`ctx_setup`): Input parameter
    - `site` (`callsite`): Input parameter
- `_thread_detach(thread thd, callsite site) -> b32`
  - What it does: Performs  thread detach
  - Parameters:
    - `thd` (`thread`): Input parameter
    - `site` (`callsite`): Input parameter
- `_thread_join(thread thd, i32* out_exit_code, callsite site) -> b32`
  - What it does: Performs  thread join
  - Parameters:
    - `thd` (`thread`): Input parameter
    - `out_exit_code` (`i32*`): Output value written by the function
    - `site` (`callsite`): Input parameter
- `thread_is_valid(thread thd) -> b32`
  - What it does: Performs thread is valid
  - Parameters:
    - `thd` (`thread`): Input parameter
- `thread_get_id(thread thd) -> u64`
  - What it does: Gets thread get id
  - Parameters:
    - `thd` (`thread`): Input parameter
- `thread_get_name(thread thd) -> cstr8`
  - What it does: Gets thread get name
  - Parameters:
    - `thd` (`thread`): Input parameter

### `threads/thread_current.h`

- Types: **1**
- Functions: **7**

#### Types

- `thread_priority` (enum)
  - What it represents: Represents a set of named constants for `thread priority`
  - Members:
    - `THREAD_PRIORITY_LOW`: `enum value`
    - `THREAD_PRIORITY_MEDIUM`: `enum value`
    - `THREAD_PRIORITY_HIGH`: `enum value`
    - `THREAD_PRIORITY_CRITICAL`: `enum value`

#### Functions

- `thread_id() -> u64`
  - What it does: Performs thread id
  - Parameters: none
- `thread_get_priority() -> thread_priority`
  - What it does: Gets thread get priority
  - Parameters: none
- `thread_set_priority(thread_priority priority) -> b32`
  - What it does: Sets thread set priority
  - Parameters:
    - `priority` (`thread_priority`): Input parameter
- `thread_sleep(u32 millis) -> void`
  - What it does: Performs thread sleep
  - Parameters:
    - `millis` (`u32`): Input parameter
- `thread_yield() -> void`
  - What it does: Performs thread yield
  - Parameters: none
- `thread_sleep_ns(u64 nanos) -> void`
  - What it does: Performs thread sleep ns
  - Parameters:
    - `nanos` (`u64`): Input parameter
- `thread_sleep_precise(u64 nanos) -> void`
  - What it does: Performs thread sleep precise
  - Parameters:
    - `nanos` (`u64`): Input parameter

### `threads/thread_group.h`

- Types: **2**
- Functions: **8**

#### Types

- `thread_group_func` (alias)
  - What it represents: Type alias used for `thread group func`
  - Members: none (alias/function type)
- `thread_group` (alias)
  - What it represents: Type alias used for `thread group`
  - Members: none (alias/function type)

#### Functions

- `_thread_group_create(u32 count, thread_group_func entry, void* arg, ctx_setup setup, callsite site) -> thread_group`
  - What it does: Creates  thread group create resources or state
  - Parameters:
    - `count` (`u32`): Number of elements/items involved
    - `entry` (`thread_group_func`): Input parameter
    - `arg` (`void*`): Input parameter
    - `setup` (`ctx_setup`): Input parameter
    - `site` (`callsite`): Input parameter
- `_thread_group_create_named(u32 count, thread_group_func entry, void* arg, ctx_setup setup, cstr8 base_name, callsite site) -> thread_group`
  - What it does: Creates  thread group create named resources or state
  - Parameters:
    - `count` (`u32`): Number of elements/items involved
    - `entry` (`thread_group_func`): Input parameter
    - `arg` (`void*`): Input parameter
    - `setup` (`ctx_setup`): Input parameter
    - `base_name` (`cstr8`): Input parameter
    - `site` (`callsite`): Input parameter
- `_thread_group_destroy(thread_group group, callsite site) -> b32`
  - What it does: Destroys  thread group destroy resources or state
  - Parameters:
    - `group` (`thread_group`): Input parameter
    - `site` (`callsite`): Input parameter
- `thread_group_is_valid(thread_group group) -> b32`
  - What it does: Performs thread group is valid
  - Parameters:
    - `group` (`thread_group`): Input parameter
- `thread_group_get_count(thread_group group) -> u32`
  - What it does: Gets thread group get count
  - Parameters:
    - `group` (`thread_group`): Input parameter
- `thread_group_get(thread_group group, u32 idx) -> thread`
  - What it does: Performs thread group get
  - Parameters:
    - `group` (`thread_group`): Input parameter
    - `idx` (`u32`): Zero-based index used to select an item
- `thread_group_join_all(thread_group group, i32* out_exit_codes) -> b32`
  - What it does: Performs thread group join all
  - Parameters:
    - `group` (`thread_group`): Input parameter
    - `out_exit_codes` (`i32*`): Output value written by the function
- `thread_group_detach_all(thread_group group) -> b32`
  - What it does: Performs thread group detach all
  - Parameters:
    - `group` (`thread_group`): Input parameter

## utils

General utility APIs such as compression, timers, UUID, logging and IDs.

### `utils/cmdline.h`

- Types: **1**
- Functions: **10**

#### Types

- `cmdline` (struct)
  - What it represents: Represents `cmdline` data grouped in a struct
  - Members:
    - `count`: `sz`
    - `args`: `c8**`

#### Functions

- `cmdline_build(sz count, c8** args) -> cmdline`
  - What it does: Performs cmdline build
  - Parameters:
    - `count` (`sz`): Number of elements/items involved
    - `args` (`c8**`): Input parameter
- `cmdline_get_count(cmdline cmdl) -> sz`
  - What it does: Gets cmdline get count
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
- `cmdline_is_empty(cmdline cmdl) -> b32`
  - What it does: Performs cmdline is empty
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
- `cmdline_get_arg(cmdline cmdl, sz idx) -> cstr8`
  - What it does: Gets cmdline get arg
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
    - `idx` (`sz`): Zero-based index used to select an item
- `cmdline_get_program(cmdline cmdl) -> cstr8`
  - What it does: Gets cmdline get program
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
- `cmdline_find(cmdline cmdl, cstr8 arg, sz* out_idx) -> b32`
  - What it does: Performs cmdline find
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
    - `arg` (`cstr8`): Input parameter
    - `out_idx` (`sz*`): Output value written by the function
- `cmdline_has(cmdline cmdl, cstr8 arg) -> b32`
  - What it does: Performs cmdline has
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
    - `arg` (`cstr8`): Input parameter
- `cmdline_get_option(cmdline cmdl, cstr8 name) -> cstr8`
  - What it does: Gets cmdline get option
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
    - `name` (`cstr8`): Input parameter
- `cmdline_get_option_i64(cmdline cmdl, cstr8 name, i64* out) -> b32`
  - What it does: Gets cmdline get option i64
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
    - `name` (`cstr8`): Input parameter
    - `out` (`i64*`): Destination storage for output data
- `cmdline_get_option_f64(cmdline cmdl, cstr8 name, f64* out) -> b32`
  - What it does: Gets cmdline get option f64
  - Parameters:
    - `cmdl` (`cmdline`): Input parameter
    - `name` (`cstr8`): Input parameter
    - `out` (`f64*`): Destination storage for output data

### `utils/compress.h`

- Types: **1**
- Functions: **3**

#### Types

- `compress_error` (enum)
  - What it represents: Represents a set of named constants for `compress error`
  - Members:
    - `COMPRESS_ERROR_NONE`: `enum value`
    - `COMPRESS_ERROR_INVALID_ARGUMENT`: `enum value`
    - `COMPRESS_ERROR_ALLOCATION_FAILED`: `enum value`
    - `COMPRESS_ERROR_CORRUPT_DATA`: `enum value`

#### Functions

- `compress_error_to_cstr(compress_error error) -> cstr8`
  - What it does: Performs compress error to cstr
  - Parameters:
    - `error` (`compress_error`): Input parameter
- `compress_encode(buffer src, allocator alloc, buffer* out_compressed) -> compress_error`
  - What it does: Performs compress encode
  - Parameters:
    - `src` (`buffer`): Input data used by the operation
    - `alloc` (`allocator`): Allocator/context used for memory management
    - `out_compressed` (`buffer*`): Output value written by the function
- `compress_decode(buffer src, allocator alloc, buffer* out_decompressed) -> compress_error`
  - What it does: Performs compress decode
  - Parameters:
    - `src` (`buffer`): Input data used by the operation
    - `alloc` (`allocator`): Allocator/context used for memory management
    - `out_decompressed` (`buffer*`): Output value written by the function

### `utils/crc.h`

- Types: **0**
- Functions: **8**

#### Types

- None

#### Functions

- `crc32_init() -> u32`
  - What it does: Initializes crc32 init
  - Parameters: none
- `crc32_update(u32 crc, const void* data, sz size) -> u32`
  - What it does: Performs crc32 update
  - Parameters:
    - `crc` (`u32`): Input parameter
    - `data` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `crc32_finalize(u32 crc) -> u32`
  - What it does: Performs crc32 finalize
  - Parameters:
    - `crc` (`u32`): Input parameter
- `crc32(const void* data, sz size) -> u32`
  - What it does: Performs crc32
  - Parameters:
    - `data` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `crc64_init() -> u64`
  - What it does: Initializes crc64 init
  - Parameters: none
- `crc64_update(u64 crc, const void* data, sz size) -> u64`
  - What it does: Performs crc64 update
  - Parameters:
    - `crc` (`u64`): Input parameter
    - `data` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation
- `crc64_finalize(u64 crc) -> u64`
  - What it does: Performs crc64 finalize
  - Parameters:
    - `crc` (`u64`): Input parameter
- `crc64(const void* data, sz size) -> u64`
  - What it does: Performs crc64
  - Parameters:
    - `data` (`const void*`): Input data used by the operation
    - `size` (`sz`): Size or capacity value used by the operation

### `utils/digits.h`

- Types: **0**
- Functions: **20**

#### Types

- None

#### Functions

- `u8_digits(u8 value) -> sz`
  - What it does: Performs u8 digits
  - Parameters:
    - `value` (`u8`): Input parameter
- `u16_digits(u16 value) -> sz`
  - What it does: Performs u16 digits
  - Parameters:
    - `value` (`u16`): Input parameter
- `u32_digits(u32 value) -> sz`
  - What it does: Performs u32 digits
  - Parameters:
    - `value` (`u32`): Input parameter
- `u64_digits(u64 value) -> sz`
  - What it does: Performs u64 digits
  - Parameters:
    - `value` (`u64`): Input parameter
- `i8_digits(i8 value) -> sz`
  - What it does: Performs i8 digits
  - Parameters:
    - `value` (`i8`): Input parameter
- `i16_digits(i16 value) -> sz`
  - What it does: Performs i16 digits
  - Parameters:
    - `value` (`i16`): Input parameter
- `i32_digits(i32 value) -> sz`
  - What it does: Performs i32 digits
  - Parameters:
    - `value` (`i32`): Input parameter
- `i64_digits(i64 value) -> sz`
  - What it does: Performs i64 digits
  - Parameters:
    - `value` (`i64`): Input parameter
- `u8x_digits(u8x value) -> sz`
  - What it does: Performs u8x digits
  - Parameters:
    - `value` (`u8x`): Input parameter
- `u16x_digits(u16x value) -> sz`
  - What it does: Performs u16x digits
  - Parameters:
    - `value` (`u16x`): Input parameter
- `u32x_digits(u32x value) -> sz`
  - What it does: Performs u32x digits
  - Parameters:
    - `value` (`u32x`): Input parameter
- `u64x_digits(u64x value) -> sz`
  - What it does: Performs u64x digits
  - Parameters:
    - `value` (`u64x`): Input parameter
- `sz_digits(sz value) -> sz`
  - What it does: Performs sz digits
  - Parameters:
    - `value` (`sz`): Input parameter
- `up_digits(up value) -> sz`
  - What it does: Performs up digits
  - Parameters:
    - `value` (`up`): Input parameter
- `i8x_digits(i8x value) -> sz`
  - What it does: Performs i8x digits
  - Parameters:
    - `value` (`i8x`): Input parameter
- `i16x_digits(i16x value) -> sz`
  - What it does: Performs i16x digits
  - Parameters:
    - `value` (`i16x`): Input parameter
- `i32x_digits(i32x value) -> sz`
  - What it does: Performs i32x digits
  - Parameters:
    - `value` (`i32x`): Input parameter
- `i64x_digits(i64x value) -> sz`
  - What it does: Performs i64x digits
  - Parameters:
    - `value` (`i64x`): Input parameter
- `sp_digits(sp value) -> sz`
  - What it does: Performs sp digits
  - Parameters:
    - `value` (`sp`): Input parameter
- `dp_digits(dp value) -> sz`
  - What it does: Performs dp digits
  - Parameters:
    - `value` (`dp`): Input parameter

### `utils/endian.h`

- Types: **0**
- Functions: **14**

#### Types

- None

#### Functions

- `endian_is_little() -> b32`
  - What it does: Ends endian is little
  - Parameters: none
- `endian_is_big() -> b32`
  - What it does: Ends endian is big
  - Parameters: none
- `endian_le16_to_native(u16 value) -> u16`
  - What it does: Ends endian le16 to native
  - Parameters:
    - `value` (`u16`): Input parameter
- `endian_le32_to_native(u32 value) -> u32`
  - What it does: Ends endian le32 to native
  - Parameters:
    - `value` (`u32`): Input parameter
- `endian_le64_to_native(u64 value) -> u64`
  - What it does: Ends endian le64 to native
  - Parameters:
    - `value` (`u64`): Input parameter
- `endian_be16_to_native(u16 value) -> u16`
  - What it does: Ends endian be16 to native
  - Parameters:
    - `value` (`u16`): Input parameter
- `endian_be32_to_native(u32 value) -> u32`
  - What it does: Ends endian be32 to native
  - Parameters:
    - `value` (`u32`): Input parameter
- `endian_be64_to_native(u64 value) -> u64`
  - What it does: Ends endian be64 to native
  - Parameters:
    - `value` (`u64`): Input parameter
- `endian_native_to_le16(u16 value) -> u16`
  - What it does: Ends endian native to le16
  - Parameters:
    - `value` (`u16`): Input parameter
- `endian_native_to_le32(u32 value) -> u32`
  - What it does: Ends endian native to le32
  - Parameters:
    - `value` (`u32`): Input parameter
- `endian_native_to_le64(u64 value) -> u64`
  - What it does: Ends endian native to le64
  - Parameters:
    - `value` (`u64`): Input parameter
- `endian_native_to_be16(u16 value) -> u16`
  - What it does: Ends endian native to be16
  - Parameters:
    - `value` (`u16`): Input parameter
- `endian_native_to_be32(u32 value) -> u32`
  - What it does: Ends endian native to be32
  - Parameters:
    - `value` (`u32`): Input parameter
- `endian_native_to_be64(u64 value) -> u64`
  - What it does: Ends endian native to be64
  - Parameters:
    - `value` (`u64`): Input parameter

### `utils/huffman.h`

- Types: **2**
- Functions: **8**

#### Types

- `huffman_code` (struct)
  - What it represents: Represents `huffman code` data grouped in a struct
  - Members:
    - `bit_length`: `u16`
    - `reserved0`: `u16`
    - `reserved1`: `u32`
    - `bits`: `u64 [HUFFMAN_CODE_WORD_CAP]`
- `huffman_tree` (struct)
  - What it represents: Represents `huffman tree` data grouped in a struct
  - Members:
    - `symbol_count`: `sz`
    - `node_count`: `sz`
    - `root_idx`: `u16`
    - `root_symbol`: `u16`
    - `symbol_frequencies`: `u64 [HUFFMAN_SYMBOL_CAP]`
    - `codes`: `huffman_code [HUFFMAN_SYMBOL_CAP]`
    - `node_frequencies`: `u64 [HUFFMAN_NODE_CAP]`
    - `node_left`: `u16 [HUFFMAN_NODE_CAP]`
    - `node_right`: `u16 [HUFFMAN_NODE_CAP]`
    - `node_symbol`: `u16 [HUFFMAN_NODE_CAP]`
    - `node_is_leaf`: `b32 [HUFFMAN_NODE_CAP]`

#### Functions

- `huffman_tree_clear(huffman_tree* tree) -> void`
  - What it does: Performs huffman tree clear
  - Parameters:
    - `tree` (`huffman_tree*`): Input parameter
- `huffman_tree_build(huffman_tree* tree, const u64* frequencies, sz symbol_count) -> b32`
  - What it does: Performs huffman tree build
  - Parameters:
    - `tree` (`huffman_tree*`): Input parameter
    - `frequencies` (`const u64*`): Input parameter
    - `symbol_count` (`sz`): Number of elements/items involved
- `huffman_tree_build_for_bytes(huffman_tree* tree, buffer src) -> b32`
  - What it does: Performs huffman tree build for bytes
  - Parameters:
    - `tree` (`huffman_tree*`): Input parameter
    - `src` (`buffer`): Input data used by the operation
- `huffman_tree_has_symbol(const huffman_tree* tree, u8 symbol) -> b32`
  - What it does: Performs huffman tree has symbol
  - Parameters:
    - `tree` (`const huffman_tree*`): Input parameter
    - `symbol` (`u8`): Input parameter
- `huffman_tree_get_code(const huffman_tree* tree, u8 symbol) -> huffman_code`
  - What it does: Gets huffman tree get code
  - Parameters:
    - `tree` (`const huffman_tree*`): Input parameter
    - `symbol` (`u8`): Input parameter
- `huffman_code_get_bit(const huffman_code* code, sz bit_idx) -> b32`
  - What it does: Gets huffman code get bit
  - Parameters:
    - `code` (`const huffman_code*`): Input parameter
    - `bit_idx` (`sz`): Zero-based index used to select an item
- `huffman_tree_bit_length_for_bytes(const huffman_tree* tree, buffer src) -> u64`
  - What it does: Performs huffman tree bit length for bytes
  - Parameters:
    - `tree` (`const huffman_tree*`): Input parameter
    - `src` (`buffer`): Input data used by the operation
- `huffman_tree_decode_symbol(const huffman_tree* tree, buffer bitstream, sz bit_offset, u8* out_symbol, sz* out_next_bit_offset) -> b32`
  - What it does: Performs huffman tree decode symbol
  - Parameters:
    - `tree` (`const huffman_tree*`): Input parameter
    - `bitstream` (`buffer`): Input parameter
    - `bit_offset` (`sz`): Input parameter
    - `out_symbol` (`u8*`): Output value written by the function
    - `out_next_bit_offset` (`sz*`): Output value written by the function

### `utils/id.h`

- Types: **4**
- Functions: **44**

#### Types

- `id8` (struct)
  - What it represents: Represents `id8` data grouped in a struct
  - Members:
    - `value`: `u8`
- `id16` (struct)
  - What it represents: Represents `id16` data grouped in a struct
  - Members:
    - `value`: `u16`
- `id32` (struct)
  - What it represents: Represents `id32` data grouped in a struct
  - Members:
    - `value`: `u32`
- `id64` (struct)
  - What it represents: Represents `id64` data grouped in a struct
  - Members:
    - `value`: `u64`

#### Functions

- `id8_zero() -> id8`
  - What it does: Performs id8 zero
  - Parameters: none
- `id8_make(u8 value) -> id8`
  - What it does: Performs id8 make
  - Parameters:
    - `value` (`u8`): Input parameter
- `id8_get(id8 ident) -> u8`
  - What it does: Performs id8 get
  - Parameters:
    - `ident` (`id8`): Input parameter
- `id8_is_zero(id8 ident) -> b32`
  - What it does: Performs id8 is zero
  - Parameters:
    - `ident` (`id8`): Input parameter
- `id8_is_valid(id8 ident) -> b32`
  - What it does: Performs id8 is valid
  - Parameters:
    - `ident` (`id8`): Input parameter
- `id8_next(id8 ident) -> id8`
  - What it does: Performs id8 next
  - Parameters:
    - `ident` (`id8`): Input parameter
- `id8_cmp(id8 lhs, id8 rhs) -> i32`
  - What it does: Performs id8 cmp
  - Parameters:
    - `lhs` (`id8`): Input data used by the operation
    - `rhs` (`id8`): Input data used by the operation
- `id8_string_length(id8 ident) -> sz`
  - What it does: Performs id8 string length
  - Parameters:
    - `ident` (`id8`): Input parameter
- `id8_parse_cstr8(cstr8 src, id8* out) -> b32`
  - What it does: Performs id8 parse cstr8
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `out` (`id8*`): Destination storage for output data
- `id8_to_cstr8(id8 ident, c8* dst, sz cap) -> b32`
  - What it does: Performs id8 to cstr8
  - Parameters:
    - `ident` (`id8`): Input parameter
    - `dst` (`c8*`): Destination storage for output data
    - `cap` (`sz`): Input parameter
- `id8_to_str8(id8 ident, str8* dst) -> b32`
  - What it does: Performs id8 to str8
  - Parameters:
    - `ident` (`id8`): Input parameter
    - `dst` (`str8*`): Destination storage for output data
- `id16_zero() -> id16`
  - What it does: Performs id16 zero
  - Parameters: none
- `id16_make(u16 value) -> id16`
  - What it does: Performs id16 make
  - Parameters:
    - `value` (`u16`): Input parameter
- `id16_get(id16 ident) -> u16`
  - What it does: Performs id16 get
  - Parameters:
    - `ident` (`id16`): Input parameter
- `id16_is_zero(id16 ident) -> b32`
  - What it does: Performs id16 is zero
  - Parameters:
    - `ident` (`id16`): Input parameter
- `id16_is_valid(id16 ident) -> b32`
  - What it does: Performs id16 is valid
  - Parameters:
    - `ident` (`id16`): Input parameter
- `id16_next(id16 ident) -> id16`
  - What it does: Performs id16 next
  - Parameters:
    - `ident` (`id16`): Input parameter
- `id16_cmp(id16 lhs, id16 rhs) -> i32`
  - What it does: Performs id16 cmp
  - Parameters:
    - `lhs` (`id16`): Input data used by the operation
    - `rhs` (`id16`): Input data used by the operation
- `id16_string_length(id16 ident) -> sz`
  - What it does: Performs id16 string length
  - Parameters:
    - `ident` (`id16`): Input parameter
- `id16_parse_cstr8(cstr8 src, id16* out) -> b32`
  - What it does: Performs id16 parse cstr8
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `out` (`id16*`): Destination storage for output data
- `id16_to_cstr8(id16 ident, c8* dst, sz cap) -> b32`
  - What it does: Performs id16 to cstr8
  - Parameters:
    - `ident` (`id16`): Input parameter
    - `dst` (`c8*`): Destination storage for output data
    - `cap` (`sz`): Input parameter
- `id16_to_str8(id16 ident, str8* dst) -> b32`
  - What it does: Performs id16 to str8
  - Parameters:
    - `ident` (`id16`): Input parameter
    - `dst` (`str8*`): Destination storage for output data
- `id32_zero() -> id32`
  - What it does: Performs id32 zero
  - Parameters: none
- `id32_make(u32 value) -> id32`
  - What it does: Performs id32 make
  - Parameters:
    - `value` (`u32`): Input parameter
- `id32_get(id32 ident) -> u32`
  - What it does: Performs id32 get
  - Parameters:
    - `ident` (`id32`): Input parameter
- `id32_is_zero(id32 ident) -> b32`
  - What it does: Performs id32 is zero
  - Parameters:
    - `ident` (`id32`): Input parameter
- `id32_is_valid(id32 ident) -> b32`
  - What it does: Performs id32 is valid
  - Parameters:
    - `ident` (`id32`): Input parameter
- `id32_next(id32 ident) -> id32`
  - What it does: Performs id32 next
  - Parameters:
    - `ident` (`id32`): Input parameter
- `id32_cmp(id32 lhs, id32 rhs) -> i32`
  - What it does: Performs id32 cmp
  - Parameters:
    - `lhs` (`id32`): Input data used by the operation
    - `rhs` (`id32`): Input data used by the operation
- `id32_string_length(id32 ident) -> sz`
  - What it does: Performs id32 string length
  - Parameters:
    - `ident` (`id32`): Input parameter
- `id32_parse_cstr8(cstr8 src, id32* out) -> b32`
  - What it does: Performs id32 parse cstr8
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `out` (`id32*`): Destination storage for output data
- `id32_to_cstr8(id32 ident, c8* dst, sz cap) -> b32`
  - What it does: Performs id32 to cstr8
  - Parameters:
    - `ident` (`id32`): Input parameter
    - `dst` (`c8*`): Destination storage for output data
    - `cap` (`sz`): Input parameter
- `id32_to_str8(id32 ident, str8* dst) -> b32`
  - What it does: Performs id32 to str8
  - Parameters:
    - `ident` (`id32`): Input parameter
    - `dst` (`str8*`): Destination storage for output data
- `id64_zero() -> id64`
  - What it does: Performs id64 zero
  - Parameters: none
- `id64_make(u64 value) -> id64`
  - What it does: Performs id64 make
  - Parameters:
    - `value` (`u64`): Input parameter
- `id64_get(id64 ident) -> u64`
  - What it does: Performs id64 get
  - Parameters:
    - `ident` (`id64`): Input parameter
- `id64_is_zero(id64 ident) -> b32`
  - What it does: Performs id64 is zero
  - Parameters:
    - `ident` (`id64`): Input parameter
- `id64_is_valid(id64 ident) -> b32`
  - What it does: Performs id64 is valid
  - Parameters:
    - `ident` (`id64`): Input parameter
- `id64_next(id64 ident) -> id64`
  - What it does: Performs id64 next
  - Parameters:
    - `ident` (`id64`): Input parameter
- `id64_cmp(id64 lhs, id64 rhs) -> i32`
  - What it does: Performs id64 cmp
  - Parameters:
    - `lhs` (`id64`): Input data used by the operation
    - `rhs` (`id64`): Input data used by the operation
- `id64_string_length(id64 ident) -> sz`
  - What it does: Performs id64 string length
  - Parameters:
    - `ident` (`id64`): Input parameter
- `id64_parse_cstr8(cstr8 src, id64* out) -> b32`
  - What it does: Performs id64 parse cstr8
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `out` (`id64*`): Destination storage for output data
- `id64_to_cstr8(id64 ident, c8* dst, sz cap) -> b32`
  - What it does: Performs id64 to cstr8
  - Parameters:
    - `ident` (`id64`): Input parameter
    - `dst` (`c8*`): Destination storage for output data
    - `cap` (`sz`): Input parameter
- `id64_to_str8(id64 ident, str8* dst) -> b32`
  - What it does: Performs id64 to str8
  - Parameters:
    - `ident` (`id64`): Input parameter
    - `dst` (`str8*`): Destination storage for output data

### `utils/log_state.h`

- Types: **5**
- Functions: **18**

#### Types

- `log_state` (alias)
  - What it represents: Type alias used for `log state`
  - Members: none (alias/function type)
- `log_level` (enum)
  - What it represents: Represents a set of named constants for `log level`
  - Members:
    - `LOG_LEVEL_FATAL`: `enum value`
    - `LOG_LEVEL_ERROR`: `enum value`
    - `LOG_LEVEL_WARN`: `enum value`
    - `LOG_LEVEL_INFO`: `enum value`
    - `LOG_LEVEL_DEBUG`: `enum value`
    - `LOG_LEVEL_VERBOSE`: `enum value`
    - `LOG_LEVEL_TRACE`: `enum value`
    - `LOG_LEVEL_MAX`: `enum value`
- `log_msg` (struct)
  - What it represents: Represents `log msg` data grouped in a struct
  - Members:
    - `next`: `struct log_msg*`
    - `level`: `log_level`
    - `site`: `callsite`
    - `text`: `cstr8`
- `log_frame` (struct)
  - What it represents: Represents `log frame` data grouped in a struct
  - Members:
    - `next`: `struct log_frame*`
    - `msgs_head`: `log_msg*`
    - `msgs_tail`: `log_msg*`
    - `msg_count`: `sz`
- `log_state` (struct)
  - What it represents: Represents `log state` data grouped in a struct
  - Members:
    - `is_init`: `b32`
    - `mutex_handle`: `mutex`
    - `level`: `log_level`
    - `root_frame_storage`: `log_frame`
    - `root_frame`: `log_frame*`
    - `active_frame`: `log_frame*`
    - `arena_alloc`: `arena`

#### Functions

- `log_level_to_str(log_level level) -> cstr8`
  - What it does: Performs log level to str
  - Parameters:
    - `level` (`log_level`): Configuration value controlling behavior
- `log_state_set_level(log_state* state, log_level level) -> void`
  - What it does: Sets log state set level
  - Parameters:
    - `state` (`log_state*`): Configuration value controlling behavior
    - `level` (`log_level`): Configuration value controlling behavior
- `log_msg_next(log_msg* msg) -> log_msg*`
  - What it does: Performs log msg next
  - Parameters:
    - `msg` (`log_msg*`): Input parameter
- `log_msg_level(log_msg* msg) -> log_level`
  - What it does: Performs log msg level
  - Parameters:
    - `msg` (`log_msg*`): Input parameter
- `log_msg_site(log_msg* msg) -> callsite`
  - What it does: Performs log msg site
  - Parameters:
    - `msg` (`log_msg*`): Input parameter
- `log_msg_text(log_msg* msg) -> cstr8`
  - What it does: Performs log msg text
  - Parameters:
    - `msg` (`log_msg*`): Input parameter
- `log_frame_has_msgs(log_frame* frame) -> b32`
  - What it does: Performs log frame has msgs
  - Parameters:
    - `frame` (`log_frame*`): Input parameter
- `log_frame_msg_count(log_frame* frame) -> sz`
  - What it does: Performs log frame msg count
  - Parameters:
    - `frame` (`log_frame*`): Input parameter
- `log_frame_first(log_frame* frame) -> log_msg*`
  - What it does: Performs log frame first
  - Parameters:
    - `frame` (`log_frame*`): Input parameter
- `log_frame_last(log_frame* frame) -> log_msg*`
  - What it does: Performs log frame last
  - Parameters:
    - `frame` (`log_frame*`): Input parameter
- `_log_state_init(log_state* state, mutex mutex_handle, allocator alloc, callsite site) -> b32`
  - What it does: Initializes  log state init
  - Parameters:
    - `state` (`log_state*`): Configuration value controlling behavior
    - `mutex_handle` (`mutex`): Input parameter
    - `alloc` (`allocator`): Allocator/context used for memory management
    - `site` (`callsite`): Input parameter
- `_log_state_quit(log_state* state, callsite site) -> void`
  - What it does: Shuts down  log state quit
  - Parameters:
    - `state` (`log_state*`): Configuration value controlling behavior
    - `site` (`callsite`): Input parameter
- `log_state_is_init(log_state* state) -> b32`
  - What it does: Initializes log state is init
  - Parameters:
    - `state` (`log_state*`): Configuration value controlling behavior
- `log_state_clear(log_state* state) -> b32`
  - What it does: Performs log state clear
  - Parameters:
    - `state` (`log_state*`): Configuration value controlling behavior
- `log_state_sync(log_state* dst, log_state* src) -> void`
  - What it does: Performs log state sync
  - Parameters:
    - `dst` (`log_state*`): Destination storage for output data
    - `src` (`log_state*`): Input data used by the operation
- `log_state_begin_frame(log_state* state) -> void`
  - What it does: Begins log state begin frame
  - Parameters:
    - `state` (`log_state*`): Configuration value controlling behavior
- `log_state_end_frame(log_state* state, u32 severity_mask) -> log_frame*`
  - What it does: Ends log state end frame
  - Parameters:
    - `state` (`log_state*`): Configuration value controlling behavior
    - `severity_mask` (`u32`): Input parameter
- `_log(log_state* state, log_level level, callsite site, const char* msg, ...) -> void`
  - What it does: Performs  log
  - Parameters:
    - `state` (`log_state*`): Configuration value controlling behavior
    - `level` (`log_level`): Configuration value controlling behavior
    - `site` (`callsite`): Input parameter
    - `msg` (`const char*`): Input parameter
    - `...` (`variadic`): Additional variadic arguments

### `utils/random_series.h`

- Types: **1**
- Functions: **29**

#### Types

- `random_series` (struct)
  - What it represents: Represents `random series` data grouped in a struct
  - Members:
    - `current`: `u32`
    - `seed`: `u32`

#### Functions

- `random_series_seed(random_series* series, u32 seed) -> void`
  - What it does: Performs random series seed
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `seed` (`u32`): Input parameter
- `random_series_reset(random_series* series) -> void`
  - What it does: Performs random series reset
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_chance_f64(random_series* series, f64 chance) -> b32`
  - What it does: Performs random series chance f64
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `chance` (`f64`): Input parameter
- `random_series_chance_f32(random_series* series, f32 chance) -> b32`
  - What it does: Performs random series chance f32
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `chance` (`f32`): Input parameter
- `random_series_chance(random_series* series, u32x chance) -> b32`
  - What it does: Performs random series chance
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `chance` (`u32x`): Input parameter
- `random_series_u8(random_series* series) -> u8`
  - What it does: Performs random series u8
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_u16(random_series* series) -> u16`
  - What it does: Performs random series u16
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_u32(random_series* series) -> u32`
  - What it does: Performs random series u32
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_u64(random_series* series) -> u64`
  - What it does: Performs random series u64
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_i8(random_series* series) -> i8`
  - What it does: Performs random series i8
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_i16(random_series* series) -> i16`
  - What it does: Performs random series i16
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_i32(random_series* series) -> i32`
  - What it does: Performs random series i32
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_i64(random_series* series) -> i64`
  - What it does: Performs random series i64
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_f32(random_series* series) -> f32`
  - What it does: Performs random series f32
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_f64(random_series* series) -> f64`
  - What it does: Performs random series f64
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_rng_u8(random_series* series, u8 min_value, u8 max_value) -> u8`
  - What it does: Performs random series rng u8
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `min_value` (`u8`): Input parameter
    - `max_value` (`u8`): Input parameter
- `random_series_rng_u16(random_series* series, u16 min_value, u16 max_value) -> u16`
  - What it does: Performs random series rng u16
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `min_value` (`u16`): Input parameter
    - `max_value` (`u16`): Input parameter
- `random_series_rng_u32(random_series* series, u32 min_value, u32 max_value) -> u32`
  - What it does: Performs random series rng u32
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `min_value` (`u32`): Input parameter
    - `max_value` (`u32`): Input parameter
- `random_series_rng_u64(random_series* series, u64 min_value, u64 max_value) -> u64`
  - What it does: Performs random series rng u64
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `min_value` (`u64`): Input parameter
    - `max_value` (`u64`): Input parameter
- `random_series_rng_i8(random_series* series, i8 min_value, i8 max_value) -> i8`
  - What it does: Performs random series rng i8
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `min_value` (`i8`): Input parameter
    - `max_value` (`i8`): Input parameter
- `random_series_rng_i16(random_series* series, i16 min_value, i16 max_value) -> i16`
  - What it does: Performs random series rng i16
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `min_value` (`i16`): Input parameter
    - `max_value` (`i16`): Input parameter
- `random_series_rng_i32(random_series* series, i32 min_value, i32 max_value) -> i32`
  - What it does: Performs random series rng i32
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `min_value` (`i32`): Input parameter
    - `max_value` (`i32`): Input parameter
- `random_series_rng_i64(random_series* series, i64 min_value, i64 max_value) -> i64`
  - What it does: Performs random series rng i64
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `min_value` (`i64`): Input parameter
    - `max_value` (`i64`): Input parameter
- `random_series_rng_f32(random_series* series, f32 min_value, f32 max_value) -> f32`
  - What it does: Performs random series rng f32
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `min_value` (`f32`): Input parameter
    - `max_value` (`f32`): Input parameter
- `random_series_rng_f64(random_series* series, f64 min_value, f64 max_value) -> f64`
  - What it does: Performs random series rng f64
  - Parameters:
    - `series` (`random_series*`): Input parameter
    - `min_value` (`f64`): Input parameter
    - `max_value` (`f64`): Input parameter
- `random_series_u_f32(random_series* series) -> f32`
  - What it does: Performs random series u f32
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_u_f64(random_series* series) -> f64`
  - What it does: Performs random series u f64
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_b_f32(random_series* series) -> f32`
  - What it does: Performs random series b f32
  - Parameters:
    - `series` (`random_series*`): Input parameter
- `random_series_b_f64(random_series* series) -> f64`
  - What it does: Performs random series b f64
  - Parameters:
    - `series` (`random_series*`): Input parameter

### `utils/stacktrace.h`

- Types: **1**
- Functions: **1**

#### Types

- `stacktrace_frame` (struct)
  - What it represents: Represents `stacktrace frame` data grouped in a struct
  - Members:
    - `address`: `up`
    - `symbol`: `str8_medium`

#### Functions

- `stacktrace_capture(stacktrace_frame* out_frames, sz out_capacity, sz skip_frames) -> sz`
  - What it does: Performs stacktrace capture
  - Parameters:
    - `out_frames` (`stacktrace_frame*`): Output value written by the function
    - `out_capacity` (`sz`): Output value written by the function
    - `skip_frames` (`sz`): Input parameter

### `utils/timer.h`

- Types: **0**
- Functions: **5**

#### Types

- None

#### Functions

- `timer_clear(f32* timer) -> void`
  - What it does: Performs timer clear
  - Parameters:
    - `timer` (`f32*`): Input parameter
- `timer_bump(f32* timer) -> void`
  - What it does: Performs timer bump
  - Parameters:
    - `timer` (`f32*`): Input parameter
- `timer_increment(f32* timer, f32 dt, f32 scale) -> void`
  - What it does: Performs timer increment
  - Parameters:
    - `timer` (`f32*`): Input parameter
    - `dt` (`f32`): Input parameter
    - `scale` (`f32`): Input parameter
- `timer_consume(f32* timer, f32 rate) -> b32`
  - What it does: Performs timer consume
  - Parameters:
    - `timer` (`f32*`): Input parameter
    - `rate` (`f32`): Input parameter
- `timer_consume_once(f32* timer, f32 rate) -> b32`
  - What it does: Performs timer consume once
  - Parameters:
    - `timer` (`f32*`): Input parameter
    - `rate` (`f32`): Input parameter

### `utils/timestamp.h`

- Types: **1**
- Functions: **19**

#### Types

- `timestamp` (struct)
  - What it represents: Represents `timestamp` data grouped in a struct
  - Members:
    - `microseconds`: `i64`

#### Functions

- `timestamp_zero() -> timestamp`
  - What it does: Performs timestamp zero
  - Parameters: none
- `timestamp_now() -> timestamp`
  - What it does: Performs timestamp now
  - Parameters: none
- `timestamp_from_microseconds(i64 microseconds) -> timestamp`
  - What it does: Performs timestamp from microseconds
  - Parameters:
    - `microseconds` (`i64`): Input parameter
- `timestamp_from_milliseconds(i64 milliseconds) -> timestamp`
  - What it does: Performs timestamp from milliseconds
  - Parameters:
    - `milliseconds` (`i64`): Input parameter
- `timestamp_from_seconds(f64 seconds) -> timestamp`
  - What it does: Performs timestamp from seconds
  - Parameters:
    - `seconds` (`f64`): Input parameter
- `timestamp_from_minutes(f64 minutes) -> timestamp`
  - What it does: Performs timestamp from minutes
  - Parameters:
    - `minutes` (`f64`): Input parameter
- `timestamp_from_hours(f64 hours) -> timestamp`
  - What it does: Performs timestamp from hours
  - Parameters:
    - `hours` (`f64`): Input parameter
- `timestamp_as_microseconds(timestamp value) -> i64`
  - What it does: Performs timestamp as microseconds
  - Parameters:
    - `value` (`timestamp`): Input parameter
- `timestamp_as_milliseconds(timestamp value) -> f64`
  - What it does: Performs timestamp as milliseconds
  - Parameters:
    - `value` (`timestamp`): Input parameter
- `timestamp_as_seconds(timestamp value) -> f64`
  - What it does: Performs timestamp as seconds
  - Parameters:
    - `value` (`timestamp`): Input parameter
- `timestamp_as_minutes(timestamp value) -> f64`
  - What it does: Performs timestamp as minutes
  - Parameters:
    - `value` (`timestamp`): Input parameter
- `timestamp_as_hours(timestamp value) -> f64`
  - What it does: Performs timestamp as hours
  - Parameters:
    - `value` (`timestamp`): Input parameter
- `timestamp_is_zero(timestamp value) -> b32`
  - What it does: Performs timestamp is zero
  - Parameters:
    - `value` (`timestamp`): Input parameter
- `timestamp_add(timestamp lhs, timestamp rhs) -> timestamp`
  - What it does: Adds timestamp add
  - Parameters:
    - `lhs` (`timestamp`): Input data used by the operation
    - `rhs` (`timestamp`): Input data used by the operation
- `timestamp_sub(timestamp lhs, timestamp rhs) -> timestamp`
  - What it does: Performs timestamp sub
  - Parameters:
    - `lhs` (`timestamp`): Input data used by the operation
    - `rhs` (`timestamp`): Input data used by the operation
- `timestamp_scale(timestamp value, f64 factor) -> timestamp`
  - What it does: Performs timestamp scale
  - Parameters:
    - `value` (`timestamp`): Input parameter
    - `factor` (`f64`): Input parameter
- `timestamp_abs(timestamp value) -> timestamp`
  - What it does: Performs timestamp abs
  - Parameters:
    - `value` (`timestamp`): Input parameter
- `timestamp_clamp(timestamp value, timestamp min_value, timestamp max_value) -> timestamp`
  - What it does: Performs timestamp clamp
  - Parameters:
    - `value` (`timestamp`): Input parameter
    - `min_value` (`timestamp`): Input parameter
    - `max_value` (`timestamp`): Input parameter
- `timestamp_cmp(timestamp lhs, timestamp rhs) -> i32`
  - What it does: Performs timestamp cmp
  - Parameters:
    - `lhs` (`timestamp`): Input data used by the operation
    - `rhs` (`timestamp`): Input data used by the operation

### `utils/uuid.h`

- Types: **1**
- Functions: **20**

#### Types

- `uuid` (struct)
  - What it represents: Represents `uuid` data grouped in a struct
  - Members:
    - `bytes`: `u8 [16]`

#### Functions

- `uuid_zero() -> uuid`
  - What it does: Performs uuid zero
  - Parameters: none
- `uuid_from_bytes(const u8* bytes) -> uuid`
  - What it does: Performs uuid from bytes
  - Parameters:
    - `bytes` (`const u8*`): Input parameter
- `uuid_from_u64(u64 upper, u64 lower) -> uuid`
  - What it does: Performs uuid from u64
  - Parameters:
    - `upper` (`u64`): Input parameter
    - `lower` (`u64`): Input parameter
- `uuid_get_bytes(uuid value, u8* dst) -> void`
  - What it does: Gets uuid get bytes
  - Parameters:
    - `value` (`uuid`): Input parameter
    - `dst` (`u8*`): Destination storage for output data
- `uuid_get_upper(uuid value) -> u64`
  - What it does: Gets uuid get upper
  - Parameters:
    - `value` (`uuid`): Input parameter
- `uuid_get_lower(uuid value) -> u64`
  - What it does: Gets uuid get lower
  - Parameters:
    - `value` (`uuid`): Input parameter
- `uuid_is_zero(uuid value) -> b32`
  - What it does: Performs uuid is zero
  - Parameters:
    - `value` (`uuid`): Input parameter
- `uuid_equal(uuid lhs, uuid rhs) -> b32`
  - What it does: Performs uuid equal
  - Parameters:
    - `lhs` (`uuid`): Input data used by the operation
    - `rhs` (`uuid`): Input data used by the operation
- `uuid_cmp(uuid lhs, uuid rhs) -> i32`
  - What it does: Performs uuid cmp
  - Parameters:
    - `lhs` (`uuid`): Input data used by the operation
    - `rhs` (`uuid`): Input data used by the operation
- `uuid_get_version(uuid value) -> u8`
  - What it does: Gets uuid get version
  - Parameters:
    - `value` (`uuid`): Input parameter
- `uuid_get_variant(uuid value) -> u8`
  - What it does: Gets uuid get variant
  - Parameters:
    - `value` (`uuid`): Input parameter
- `uuid_string_length() -> sz`
  - What it does: Performs uuid string length
  - Parameters: none
- `uuid_parse_cstr8(cstr8 src, uuid* out) -> b32`
  - What it does: Performs uuid parse cstr8
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `out` (`uuid*`): Destination storage for output data
- `uuid_to_cstr8(uuid value, c8* dst, sz cap) -> b32`
  - What it does: Performs uuid to cstr8
  - Parameters:
    - `value` (`uuid`): Input parameter
    - `dst` (`c8*`): Destination storage for output data
    - `cap` (`sz`): Input parameter
- `uuid_to_cstr16(uuid value, c16* dst, sz cap) -> b32`
  - What it does: Performs uuid to cstr16
  - Parameters:
    - `value` (`uuid`): Input parameter
    - `dst` (`c16*`): Destination storage for output data
    - `cap` (`sz`): Input parameter
- `uuid_to_cstr32(uuid value, c32* dst, sz cap) -> b32`
  - What it does: Performs uuid to cstr32
  - Parameters:
    - `value` (`uuid`): Input parameter
    - `dst` (`c32*`): Destination storage for output data
    - `cap` (`sz`): Input parameter
- `uuid_to_str8(uuid value, str8* dst) -> b32`
  - What it does: Performs uuid to str8
  - Parameters:
    - `value` (`uuid`): Input parameter
    - `dst` (`str8*`): Destination storage for output data
- `uuid_to_str16(uuid value, str16* dst) -> b32`
  - What it does: Performs uuid to str16
  - Parameters:
    - `value` (`uuid`): Input parameter
    - `dst` (`str16*`): Destination storage for output data
- `uuid_to_str32(uuid value, str32* dst) -> b32`
  - What it does: Performs uuid to str32
  - Parameters:
    - `value` (`uuid`): Input parameter
    - `dst` (`str32*`): Destination storage for output data
- `uuid_generate_v4(random_series* series) -> uuid`
  - What it does: Performs uuid generate v4
  - Parameters:
    - `series` (`random_series*`): Input parameter

### `utils/version.h`

- Types: **1**
- Functions: **17**

#### Types

- `parts` (union)
  - What it represents: Represents `parts` data in alternative memory layouts
  - Members:
    - `packed`: `u32`
    - `major`: `struct { u8`
    - `minor`: `u8`
    - `patch`: `u16`

#### Functions

- `version_zero() -> version`
  - What it does: Performs version zero
  - Parameters: none
- `version_from_packed(u32 packed) -> version`
  - What it does: Performs version from packed
  - Parameters:
    - `packed` (`u32`): Input parameter
- `version_make(u8 major, u8 minor, u16 patch) -> version`
  - What it does: Performs version make
  - Parameters:
    - `major` (`u8`): Input parameter
    - `minor` (`u8`): Input parameter
    - `patch` (`u16`): Input parameter
- `version_get_packed(version ver) -> u32`
  - What it does: Gets version get packed
  - Parameters:
    - `ver` (`version`): Input parameter
- `version_get_major(version ver) -> u8`
  - What it does: Gets version get major
  - Parameters:
    - `ver` (`version`): Input parameter
- `version_get_minor(version ver) -> u8`
  - What it does: Gets version get minor
  - Parameters:
    - `ver` (`version`): Input parameter
- `version_get_patch(version ver) -> u16`
  - What it does: Gets version get patch
  - Parameters:
    - `ver` (`version`): Input parameter
- `version_is_zero(version ver) -> b32`
  - What it does: Performs version is zero
  - Parameters:
    - `ver` (`version`): Input parameter
- `version_string_length(version ver) -> sz`
  - What it does: Performs version string length
  - Parameters:
    - `ver` (`version`): Input parameter
- `version_cmp(version lhs, version rhs) -> i32`
  - What it does: Performs version cmp
  - Parameters:
    - `lhs` (`version`): Input data used by the operation
    - `rhs` (`version`): Input data used by the operation
- `version_to_cstr8(version ver, c8* dst, sz cap) -> b32`
  - What it does: Performs version to cstr8
  - Parameters:
    - `ver` (`version`): Input parameter
    - `dst` (`c8*`): Destination storage for output data
    - `cap` (`sz`): Input parameter
- `version_to_cstr16(version ver, c16* dst, sz cap) -> b32`
  - What it does: Performs version to cstr16
  - Parameters:
    - `ver` (`version`): Input parameter
    - `dst` (`c16*`): Destination storage for output data
    - `cap` (`sz`): Input parameter
- `version_to_cstr32(version ver, c32* dst, sz cap) -> b32`
  - What it does: Performs version to cstr32
  - Parameters:
    - `ver` (`version`): Input parameter
    - `dst` (`c32*`): Destination storage for output data
    - `cap` (`sz`): Input parameter
- `version_to_str8(version ver, str8* dst) -> b32`
  - What it does: Performs version to str8
  - Parameters:
    - `ver` (`version`): Input parameter
    - `dst` (`str8*`): Destination storage for output data
- `version_to_str16(version ver, str16* dst) -> b32`
  - What it does: Performs version to str16
  - Parameters:
    - `ver` (`version`): Input parameter
    - `dst` (`str16*`): Destination storage for output data
- `version_to_str32(version ver, str32* dst) -> b32`
  - What it does: Performs version to str32
  - Parameters:
    - `ver` (`version`): Input parameter
    - `dst` (`str32*`): Destination storage for output data
- `version_parse_cstr8(cstr8 src, version* out_ver) -> b32`
  - What it does: Performs version parse cstr8
  - Parameters:
    - `src` (`cstr8`): Input data used by the operation
    - `out_ver` (`version*`): Output value written by the function
