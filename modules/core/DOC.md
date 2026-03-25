# Core Module Quick Guide

`core` is the foundational module of based. Use it for platform abstractions, memory/context systems, containers, I/O and filesystem utilities, input/event flow, threading, and shared diagnostics/logging primitives.

The canonical API reference remains `./api_index.txt`. This guide is organized by API area and then by header so you can quickly locate public types, functions, and macros.

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

Compiler/platform layer, primitive types, assertions, crash routing, intrinsics, and foundational macros.

### `basic/assert.h`

- Types: **1**
- Functions: **3**
- Macros: **5**

#### Types

| Type | Declaration |
| --- | --- |
| `assert_mode` | `typedef enum assert_mode { ... } assert_mode;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_assert` | `func void _assert(b32 condition, const char* msg, callsite site);` |
| `assert_set_mode` | `func void assert_set_mode(assert_mode mode);` |
| `assert_get_mode` | `func assert_mode assert_get_mode(void);` |

#### Macros

| Macro | Form |
| --- | --- |
| `assert` | `assert(condition)` |
| `assert_msg` | `assert_msg(condition, msg)` |
| `invalid_code_path` | `invalid_code_path` |
| `ASSERT_STACKTRACE_DEPTH` | `ASSERT_STACKTRACE_DEPTH` |
| `ASSERT_MODE_DEFAULT` | `ASSERT_MODE_DEFAULT` |

### `basic/codespace.h`

- Types: **2**
- Functions: **0**
- Macros: **2**

#### Types

| Type | Declaration |
| --- | --- |
| `callsite` | `typedef struct callsite { ... } callsite;` |
| `src_loc` | `typedef struct src_loc { ... } src_loc;` |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `CALLSITE_HERE` | `CALLSITE_HERE` |
| `SRC_LOC_HERE` | `SRC_LOC_HERE` |

### `basic/crash.h`

- Types: **0**
- Functions: **3**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `crash_install` | `func b32 crash_install(void);` |
| `crash_uninstall` | `func void crash_uninstall(void);` |
| `crash_is_installed` | `func b32 crash_is_installed(void);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `basic/entry.h`

- Types: **7**
- Functions: **11**
- Macros: **5**

#### Types

| Type | Declaration |
| --- | --- |
| `app_result` | `typedef enum app_result { ... } app_result;` |
| `entry_app_init_fn` | `typedef app_result entry_app_init_fn(cmdline cmdl, void** state);` |
| `entry_app_update_fn` | `typedef app_result entry_app_update_fn(void* state);` |
| `entry_app_quit_fn` | `typedef void entry_app_quit_fn(void* state, app_result result);` |
| `entry_app_callbacks` | `typedef struct entry_app_callbacks { ... } entry_app_callbacks;` |
| `entry_run_fn` | `typedef b32 entry_run_fn(cmdline cmdl);` |
| `entry_run_callbacks` | `typedef struct entry_run_callbacks { ... } entry_run_callbacks;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `entry_init` | `func b32 entry_init(cmdline cmdline);` |
| `entry_get_cmdline` | `func cmdline entry_get_cmdline(void);` |
| `entry_get_allocator` | `func allocator entry_get_allocator(void);` |
| `entry_get_ctx_setup` | `func ctx_setup entry_get_ctx_setup(void);` |
| `entry_quit` | `func void entry_quit(void);` |
| `app_init` | `func app_result app_init(cmdline cmdl, void** state);` |
| `app_update` | `func app_result app_update(void* state);` |
| `app_quit` | `func void app_quit(void* state, app_result result);` |
| `run` | `func b32 run(cmdline cmdl);` |
| `mod_init` | `func dll_export b32 mod_init(void);` |
| `mod_quit` | `func dll_export void mod_quit(void);` |

#### Macros

| Macro | Form |
| --- | --- |
| `ENTRY_GET_GLOBAL_ALLOCATOR` | `ENTRY_GET_GLOBAL_ALLOCATOR` |
| `ENTRY_GET_GLOBAL_CTX_SETUP` | `ENTRY_GET_GLOBAL_CTX_SETUP()` |
| `ENTRY_FUNCTION_NAME` | `ENTRY_FUNCTION_NAME` |
| `ENTRY_CALLBACKS_TYPE` | `ENTRY_CALLBACKS_TYPE` |
| `ENTRY_CALLBACKS_INIT` | `ENTRY_CALLBACKS_INIT()` |

### `basic/entry_impl.h`

- Types: **3**
- Functions: **0**
- Macros: **3**

#### Types

| Type | Declaration |
| --- | --- |
| `HINSTANCE` | `typedef struct HINSTANCE__* HINSTANCE;` |
| `LPSTR` | `typedef char* LPSTR;` |
| `PWSTR` | `typedef wchar_t* PWSTR;` |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `ENTRY_IMPL` | `ENTRY_IMPL` |
| `ENTRY_BOOT` | `ENTRY_BOOT(argc, argv)` |
| `WINAPI` | `WINAPI` |

### `basic/env_defines.h`

- Types: **0**
- Functions: **0**
- Macros: **9**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `PLATFORM_WINDOWS` | `PLATFORM_WINDOWS` |
| `PLATFORM_LINUX` | `PLATFORM_LINUX` |
| `PLATFORM_MACOS` | `PLATFORM_MACOS` |
| `PLATFORM_UNIX` | `PLATFORM_UNIX` |
| `PLATFORM_DESKTOP` | `PLATFORM_DESKTOP` |
| `ARCH_X86_64` | `ARCH_X86_64` |
| `ARCH_ARM64` | `ARCH_ARM64` |
| `BUILD_RELEASE` | `BUILD_RELEASE` |
| `BUILD_DEBUG` | `BUILD_DEBUG` |

### `basic/intrinsics.h`

- Types: **0**
- Functions: **15**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `popcount_u32` | `func i32 popcount_u32(u32 val);` |
| `popcount_u64` | `func i32 popcount_u64(u64 val);` |
| `ctz_u32` | `func i32 ctz_u32(u32 val);` |
| `ctz_u64` | `func i32 ctz_u64(u64 val);` |
| `clz_u32` | `func i32 clz_u32(u32 val);` |
| `clz_u64` | `func i32 clz_u64(u64 val);` |
| `bsr_u32` | `func i32 bsr_u32(u32 val);` |
| `bsr_u64` | `func i32 bsr_u64(u64 val);` |
| `bswap_u16` | `func u16 bswap_u16(u16 val);` |
| `bswap_u32` | `func u32 bswap_u32(u32 val);` |
| `bswap_u64` | `func u64 bswap_u64(u64 val);` |
| `rotl_u32` | `func u32 rotl_u32(u32 val, i32 cnt);` |
| `rotl_u64` | `func u64 rotl_u64(u64 val, i32 cnt);` |
| `rotr_u32` | `func u32 rotr_u32(u32 val, i32 cnt);` |
| `rotr_u64` | `func u64 rotr_u64(u64 val, i32 cnt);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `basic/keyword_defines.h`

- Types: **0**
- Functions: **0**
- Macros: **18**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `thread_local` | `thread_local` |
| `dll_export` | `dll_export` |
| `dll_import` | `dll_import` |
| `static_assert` | `static_assert(expr)` |
| `no_return` | `no_return` |
| `force_inline` | `force_inline` |
| `no_inline` | `no_inline` |
| `align_as` | `align_as(x)` |
| `align_of` | `align_of(x)` |
| `size_of` | `size_of(x)` |
| `type_of` | `type_of(x)` |
| `likely` | `likely(x)` |
| `unlikely` | `unlikely(x)` |
| `c_begin` | `c_begin` |
| `c_end` | `c_end` |
| `local_persist` | `local_persist` |
| `global_var` | `global_var` |
| `func` | `func` |

### `basic/primitive_types.h`

- Types: **33**
- Functions: **0**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `i8` | `typedef int8_t i8;` |
| `u8` | `typedef uint8_t u8;` |
| `i16` | `typedef int16_t i16;` |
| `u16` | `typedef uint16_t u16;` |
| `i32` | `typedef int32_t i32;` |
| `u32` | `typedef uint32_t u32;` |
| `i64` | `typedef int64_t i64;` |
| `u64` | `typedef uint64_t u64;` |
| `f32` | `typedef float f32;` |
| `f64` | `typedef double f64;` |
| `b8` | `typedef uint8_t b8;` |
| `b16` | `typedef uint16_t b16;` |
| `b32` | `typedef uint32_t b32;` |
| `b64` | `typedef uint64_t b64;` |
| `c8` | `typedef char c8;` |
| `c16` | `typedef uint16_t c16;` |
| `c32` | `typedef uint32_t c32;` |
| `i8x` | `typedef const c8* cstr8; // Null terminated UTF-8 C String typedef const c16* cstr16; // Null terminated UTF-16 C String typedef const c32* cstr32; // Null terminated UTF-32 C String typedef int_fast8_t i8x;` |
| `u8x` | `typedef uint_fast8_t u8x;` |
| `i16x` | `typedef int_fast16_t i16x;` |
| `u16x` | `typedef uint_fast16_t u16x;` |
| `i32x` | `typedef int_fast32_t i32x;` |
| `u32x` | `typedef uint_fast32_t u32x;` |
| `i64x` | `typedef int_fast64_t i64x;` |
| `u64x` | `typedef uint_fast64_t u64x;` |
| `b8x` | `typedef uint_fast8_t b8x;` |
| `b16x` | `typedef uint_fast16_t b16x;` |
| `b32x` | `typedef uint_fast32_t b32x;` |
| `b64x` | `typedef uint_fast64_t b64x;` |
| `sz` | `typedef size_t sz;` |
| `up` | `typedef uintptr_t up;` |
| `sp` | `typedef intptr_t sp;` |
| `dp` | `typedef ptrdiff_t dp;` |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `basic/profiler.h`

- Types: **0**
- Functions: **0**
- Macros: **6**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `TRACY_ENABLE` | `TRACY_ENABLE` |
| `TRACY_CALLSTACK` | `TRACY_CALLSTACK` |
| `profile_func_begin` | `profile_func_begin` |
| `profile_func_end` | `profile_func_end` |
| `TracyCAlloc` | `TracyCAlloc(ptr, size)` |
| `TracyCFree` | `TracyCFree(ptr)` |

### `basic/safe.h`

- Types: **0**
- Functions: **0**
- Macros: **5**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `SAFE_LOOP_MAX_ITERATION_COUNT` | `SAFE_LOOP_MAX_ITERATION_COUNT` |
| `SAFE_WHILE_IMPL` | `SAFE_WHILE_IMPL(cond, id)` |
| `safe_while` | `safe_while(cond)` |
| `SAFE_FOR_IMPL` | `SAFE_FOR_IMPL(id, ...)` |
| `safe_for` | `safe_for(...)` |

### `basic/third_party.h`

- Types: **0**
- Functions: **0**
- Macros: **3**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `LM2_ENABLE_UNPREFIXED_NAMES` | `LM2_ENABLE_UNPREFIXED_NAMES` |
| `LM2_CUSTOM_ASSERT` | `LM2_CUSTOM_ASSERT` |
| `LM2_NO_GENERICS` | `LM2_NO_GENERICS` |

### `basic/utility_defines.h`

- Types: **0**
- Functions: **0**
- Macros: **34**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `strfy` | `strfy(x)` |
| `strfy_exp` | `strfy_exp(x)` |
| `cat` | `cat(x, y)` |
| `cat_exp` | `cat_exp(x, y)` |
| `expr` | `expr(x)` |
| `stmt` | `stmt(x)` |
| `count_of` | `count_of(x)` |
| `size_of_each` | `size_of_each(x)` |
| `multiline_literal` | `multiline_literal(...)` |
| `bit` | `bit(x)` |
| `bit64` | `bit64(x)` |
| `bit_is_set` | `bit_is_set(bits, b)` |
| `bit_set` | `bit_set(bits, b)` |
| `bit_unset` | `bit_unset(bits, b)` |
| `bit_toggle` | `bit_toggle(bits, b)` |
| `field_of` | `field_of(type, field)` |
| `size_of_field` | `size_of_field(type, field)` |
| `offset_of` | `offset_of(type, field)` |
| `container_of` | `container_of(ptr, type, field)` |
| `in_range` | `in_range(x, lo, hi)` |
| `in_bounds` | `in_bounds(arr, i)` |
| `swap_value` | `swap_value(a, b)` |
| `swap_ptr` | `swap_ptr(a_ptr, b_ptr)` |
| `is_pow2` | `is_pow2(x)` |
| `align_up` | `align_up(x, n)` |
| `align_down` | `align_down(x, n)` |
| `kb` | `kb(b)` |
| `mb` | `mb(b)` |
| `gb` | `gb(b)` |
| `tb` | `tb(b)` |
| `th` | `th(x)` |
| `mil` | `mil(x)` |
| `bil` | `bil(x)` |
| `tril` | `tril(x)` |

## containers

Intrusive containers and sorting helpers for lightweight data structures.

### `containers/binary_tree.h`

- Types: **0**
- Functions: **0**
- Macros: **15**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `BINARY_TREE_IS_ROOT` | `BINARY_TREE_IS_ROOT(node)` |
| `BINARY_TREE_IS_LEAF` | `BINARY_TREE_IS_LEAF(node)` |
| `BINARY_TREE_FIRST_INORDER` | `BINARY_TREE_FIRST_INORDER(root, out)` |
| `BINARY_TREE_NEXT_INORDER` | `BINARY_TREE_NEXT_INORDER(root, node, out)` |
| `BINARY_TREE_FIRST_POSTORDER` | `BINARY_TREE_FIRST_POSTORDER(root, out)` |
| `BINARY_TREE_NEXT_POSTORDER` | `BINARY_TREE_NEXT_POSTORDER(root, node, out)` |
| `BINARY_TREE_NEXT_PREORDER` | `BINARY_TREE_NEXT_PREORDER(root, node, out)` |
| `BINARY_TREE_INSERT_LEFT` | `BINARY_TREE_INSERT_LEFT(parent, node)` |
| `BINARY_TREE_INSERT_RIGHT` | `BINARY_TREE_INSERT_RIGHT(parent, node)` |
| `BINARY_TREE_REMOVE` | `BINARY_TREE_REMOVE(root_ptr, node)` |
| `BINARY_TREE_ROTATE_LEFT` | `BINARY_TREE_ROTATE_LEFT(root_ptr, node)` |
| `BINARY_TREE_ROTATE_RIGHT` | `BINARY_TREE_ROTATE_RIGHT(root_ptr, node)` |
| `BINARY_TREE_FOREACH_PREORDER` | `BINARY_TREE_FOREACH_PREORDER(root, it)` |
| `BINARY_TREE_FOREACH_INORDER` | `BINARY_TREE_FOREACH_INORDER(root, it)` |
| `BINARY_TREE_FOREACH_POSTORDER` | `BINARY_TREE_FOREACH_POSTORDER(root, it)` |

### `containers/bitset.h`

- Types: **0**
- Functions: **0**
- Macros: **12**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `BITSET_WORD_COUNT` | `BITSET_WORD_COUNT(n)` |
| `BITSET_SET` | `BITSET_SET(arr, idx)` |
| `BITSET_CLEAR` | `BITSET_CLEAR(arr, idx)` |
| `BITSET_TOGGLE` | `BITSET_TOGGLE(arr, idx)` |
| `BITSET_TEST` | `BITSET_TEST(arr, idx)` |
| `BITSET_CLEAR_ALL` | `BITSET_CLEAR_ALL(arr, word_count)` |
| `BITSET_SET_ALL` | `BITSET_SET_ALL(arr, word_count)` |
| `BITSET_COUNT` | `BITSET_COUNT(arr, word_count, out)` |
| `BITSET_FIRST_SET` | `BITSET_FIRST_SET(arr, word_count, out)` |
| `BITSET_FIRST_CLEAR` | `BITSET_FIRST_CLEAR(arr, word_count, out)` |
| `BITSET_FIND_NEXT_SET` | `BITSET_FIND_NEXT_SET(arr, word_count, from_idx, out)` |
| `BITSET_FOREACH_SET` | `BITSET_FOREACH_SET(arr, word_count, idx)` |

### `containers/doubly_list.h`

- Types: **0**
- Functions: **0**
- Macros: **13**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `DOUBLY_LIST_EMPTY` | `DOUBLY_LIST_EMPTY(head, tail)` |
| `DOUBLY_LIST_COUNT` | `DOUBLY_LIST_COUNT(head, tail, count)` |
| `DOUBLY_LIST_HEAD` | `DOUBLY_LIST_HEAD(head, tail)` |
| `DOUBLY_LIST_TAIL` | `DOUBLY_LIST_TAIL(head, tail)` |
| `DOUBLY_LIST_PUSH_FRONT` | `DOUBLY_LIST_PUSH_FRONT(head, tail, node)` |
| `DOUBLY_LIST_PUSH_BACK` | `DOUBLY_LIST_PUSH_BACK(head, tail, node)` |
| `DOUBLY_LIST_POP_FRONT` | `DOUBLY_LIST_POP_FRONT(head, tail, node)` |
| `DOUBLY_LIST_POP_BACK` | `DOUBLY_LIST_POP_BACK(head, tail, node)` |
| `DOUBLY_LIST_REMOVE` | `DOUBLY_LIST_REMOVE(head, tail, node)` |
| `DOUBLY_LIST_INSERT_AFTER` | `DOUBLY_LIST_INSERT_AFTER(head, tail, after, node)` |
| `DOUBLY_LIST_INSERT_BEFORE` | `DOUBLY_LIST_INSERT_BEFORE(head, tail, before, node)` |
| `DOUBLY_LIST_FOREACH` | `DOUBLY_LIST_FOREACH(head, tail, it)` |
| `DOUBLY_LIST_FOREACH_REVERSE` | `DOUBLY_LIST_FOREACH_REVERSE(head, tail, it)` |

### `containers/hash_map.h`

- Types: **3**
- Functions: **13**
- Macros: **1**

#### Types

| Type | Declaration |
| --- | --- |
| `hash_map_slot` | `typedef struct hash_map_slot { ... } hash_map_slot;` |
| `hash_map` | `typedef struct hash_map { ... } hash_map;` |
| `hash_map_iter` | `typedef sz hash_map_iter;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `hash_map_create` | `func hash_map hash_map_create(sz cap, allocator alloc);` |
| `hash_map_destroy` | `func void hash_map_destroy(hash_map* map);` |
| `hash_map_clear` | `func void hash_map_clear(hash_map* map);` |
| `hash_map_count` | `func sz hash_map_count(hash_map const* map);` |
| `hash_map_capacity` | `func sz hash_map_capacity(hash_map const* map);` |
| `hash_map_load_factor` | `func f32 hash_map_load_factor(hash_map const* map);` |
| `hash_map_rehash` | `func b32 hash_map_rehash(hash_map* map, sz new_cap);` |
| `hash_map_reserve` | `func b32 hash_map_reserve(hash_map* map, sz min_cap);` |
| `hash_map_set` | `func b32 hash_map_set(hash_map* map, u64 key, void* value);` |
| `hash_map_get` | `func void* hash_map_get(hash_map* map, u64 key);` |
| `hash_map_has` | `func b32 hash_map_has(hash_map* map, u64 key);` |
| `hash_map_remove` | `func b32 hash_map_remove(hash_map* map, u64 key);` |
| `hash_map_next` | `func hash_map_slot* hash_map_next(hash_map* map, hash_map_iter* iter);` |

#### Macros

| Macro | Form |
| --- | --- |
| `HASH_MAP_FOREACH` | `HASH_MAP_FOREACH(map, it)` |

### `containers/ring_list.h`

- Types: **0**
- Functions: **0**
- Macros: **13**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `RING_LIST_EMPTY` | `RING_LIST_EMPTY(head)` |
| `RING_LIST_COUNT` | `RING_LIST_COUNT(head, count)` |
| `RING_LIST_HEAD` | `RING_LIST_HEAD(head)` |
| `RING_LIST_TAIL` | `RING_LIST_TAIL(head)` |
| `RING_LIST_PUSH_FRONT` | `RING_LIST_PUSH_FRONT(head, node)` |
| `RING_LIST_PUSH_BACK` | `RING_LIST_PUSH_BACK(head, node)` |
| `RING_LIST_POP_FRONT` | `RING_LIST_POP_FRONT(head, node)` |
| `RING_LIST_POP_BACK` | `RING_LIST_POP_BACK(head, node)` |
| `RING_LIST_REMOVE` | `RING_LIST_REMOVE(head, node)` |
| `RING_LIST_INSERT_AFTER` | `RING_LIST_INSERT_AFTER(head, after, node)` |
| `RING_LIST_INSERT_BEFORE` | `RING_LIST_INSERT_BEFORE(head, before, node)` |
| `RING_LIST_FOREACH` | `RING_LIST_FOREACH(head, it)` |
| `RING_LIST_FOREACH_REVERSE` | `RING_LIST_FOREACH_REVERSE(head, it)` |

### `containers/singly_list.h`

- Types: **0**
- Functions: **0**
- Macros: **8**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `SINGLY_LIST_EMPTY` | `SINGLY_LIST_EMPTY(head, tail)` |
| `SINGLY_LIST_COUNT` | `SINGLY_LIST_COUNT(head, tail, count)` |
| `SINGLY_LIST_HEAD` | `SINGLY_LIST_HEAD(head, tail)` |
| `SINGLY_LIST_TAIL` | `SINGLY_LIST_TAIL(head, tail)` |
| `SINGLY_LIST_PUSH_FRONT` | `SINGLY_LIST_PUSH_FRONT(head, tail, node)` |
| `SINGLY_LIST_PUSH_BACK` | `SINGLY_LIST_PUSH_BACK(head, tail, node)` |
| `SINGLY_LIST_POP_FRONT` | `SINGLY_LIST_POP_FRONT(head, tail, node)` |
| `SINGLY_LIST_FOREACH` | `SINGLY_LIST_FOREACH(head, tail, it)` |

### `containers/sort.h`

- Types: **1**
- Functions: **8**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `sort_compare_fn` | `typedef i32 sort_compare_fn(const void* lhs_ptr, const void* rhs_ptr, void* user_data);` |

#### Functions

| Function | Declaration |
| --- | --- |
| `sort_check` | `func b32 sort_check( const void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data);` |
| `sort_bubble` | `func sz sort_bubble( void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data);` |
| `sort_quick` | `func sz sort_quick( void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data, allocator allocator);` |
| `sort_merge` | `func sz sort_merge( void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data, allocator allocator);` |
| `sort_selection` | `func sz sort_selection( void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data);` |
| `sort_insertion` | `func sz sort_insertion( void* ptr, sz elem_count, sz elem_size, sort_compare_fn* compare, void* user_data);` |
| `sort_radix32` | `func sz sort_radix32(u32* ptr, sz elem_count);` |
| `sort_radix64` | `func sz sort_radix64(u64* ptr, sz elem_count);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `containers/stack_list.h`

- Types: **0**
- Functions: **0**
- Macros: **7**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `STACK_LIST_EMPTY` | `STACK_LIST_EMPTY(head)` |
| `STACK_LIST_COUNT` | `STACK_LIST_COUNT(head, count)` |
| `STACK_LIST_HEAD` | `STACK_LIST_HEAD(head)` |
| `STACK_LIST_NEXT` | `STACK_LIST_NEXT(node)` |
| `STACK_LIST_PUSH` | `STACK_LIST_PUSH(head, node)` |
| `STACK_LIST_POP` | `STACK_LIST_POP(head, node)` |
| `STACK_LIST_FOREACH` | `STACK_LIST_FOREACH(head, it)` |

### `containers/tree.h`

- Types: **0**
- Functions: **0**
- Macros: **11**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| None | - |

#### Macros

| Macro | Form |
| --- | --- |
| `TREE_IS_ROOT` | `TREE_IS_ROOT(node)` |
| `TREE_IS_LEAF` | `TREE_IS_LEAF(node)` |
| `TREE_NEXT_PREORDER` | `TREE_NEXT_PREORDER(root, node, out)` |
| `TREE_INSERT_CHILD_FRONT` | `TREE_INSERT_CHILD_FRONT(parent, node)` |
| `TREE_INSERT_CHILD_BACK` | `TREE_INSERT_CHILD_BACK(parent, node)` |
| `TREE_INSERT_BEFORE` | `TREE_INSERT_BEFORE(before, node)` |
| `TREE_INSERT_AFTER` | `TREE_INSERT_AFTER(after, node)` |
| `TREE_REMOVE` | `TREE_REMOVE(node)` |
| `TREE_FOREACH_CHILDREN` | `TREE_FOREACH_CHILDREN(parent, it)` |
| `TREE_FOREACH_CHILDREN_REVERSE` | `TREE_FOREACH_CHILDREN_REVERSE(parent, it)` |
| `TREE_FOREACH_PREORDER` | `TREE_FOREACH_PREORDER(root, it)` |

## context

Global/thread context setup, allocators, user data slots, and log-state access.

### `context/ctx.h`

- Types: **3**
- Functions: **15**
- Macros: **1**

#### Types

| Type | Declaration |
| --- | --- |
| `ctx_user_data_idx` | `typedef sz ctx_user_data_idx;` |
| `ctx_setup` | `typedef struct ctx_setup { ... } ctx_setup;` |
| `ctx` | `typedef struct ctx { ... } ctx;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `ctx_setup_is_valid` | `func b32 ctx_setup_is_valid(ctx_setup* setup);` |
| `ctx_setup_fill_defaults` | `func void ctx_setup_fill_defaults(ctx_setup* setup);` |
| `ctx_init` | `func b32 ctx_init(ctx* context, ctx_setup setup);` |
| `ctx_quit` | `func b32 ctx_quit(ctx* context);` |
| `ctx_is_init` | `func b32 ctx_is_init(ctx* context);` |
| `ctx_get_allocator` | `func allocator ctx_get_allocator(ctx* context);` |
| `ctx_get_setup` | `func ctx_setup ctx_get_setup(ctx* context);` |
| `ctx_get_log_state` | `func log_state* ctx_get_log_state(ctx* context);` |
| `ctx_get_perm_arena` | `func arena* ctx_get_perm_arena(ctx* context);` |
| `ctx_get_temp_arena` | `func arena* ctx_get_temp_arena(ctx* context);` |
| `ctx_get_perm_heap` | `func heap* ctx_get_perm_heap(ctx* context);` |
| `ctx_get_temp_heap` | `func heap* ctx_get_temp_heap(ctx* context);` |
| `ctx_get_user_data` | `func void* ctx_get_user_data(ctx* context, ctx_user_data_idx idx);` |
| `ctx_set_user_data` | `func b32 ctx_set_user_data(ctx* context, ctx_user_data_idx idx, void* user_data);` |
| `ctx_clear_temp` | `func void ctx_clear_temp(ctx* context);` |

#### Macros

| Macro | Form |
| --- | --- |
| `CTX_USER_DATA_COUNT` | `CTX_USER_DATA_COUNT` |

### `context/global_ctx.h`

- Types: **0**
- Functions: **23**
- Macros: **7**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `global_ctx_init` | `func b32 global_ctx_init(ctx_setup setup);` |
| `global_ctx_quit` | `func b32 global_ctx_quit(void);` |
| `global_ctx_is_init` | `func b32 global_ctx_is_init(void);` |
| `global_ctx_get` | `func ctx* global_ctx_get(void);` |
| `global_ctx_lock` | `func void global_ctx_lock(void);` |
| `global_ctx_unlock` | `func void global_ctx_unlock(void);` |
| `global_get_allocator` | `func allocator global_get_allocator(void);` |
| `global_get_setup` | `func ctx_setup global_get_setup(void);` |
| `global_get_main_allocator` | `func allocator global_get_main_allocator(void);` |
| `global_get_log_state` | `func log_state* global_get_log_state(void);` |
| `global_get_perm_arena` | `func arena* global_get_perm_arena(void);` |
| `global_get_temp_arena` | `func arena* global_get_temp_arena(void);` |
| `global_get_perm_heap` | `func heap* global_get_perm_heap(void);` |
| `global_get_temp_heap` | `func heap* global_get_temp_heap(void);` |
| `global_has_user_data_access` | `func b32 global_has_user_data_access(ctx_user_data_idx idx);` |
| `global_get_user_data` | `func void* global_get_user_data(ctx_user_data_idx idx);` |
| `global_set_user_data` | `func b32 global_set_user_data(ctx_user_data_idx idx, void* user_data);` |
| `global_set_user_data_access` | `func b32 global_set_user_data_access(ctx_user_data_idx idx, b8 has_access);` |
| `global_set_user_data_access_all` | `func void global_set_user_data_access_all(b8 has_access);` |
| `global_clear_temp` | `func void global_clear_temp(void);` |
| `global_log_set_level` | `func void global_log_set_level(log_level level);` |
| `global_log_begin_frame` | `func void global_log_begin_frame(void);` |
| `global_log_end_frame` | `func log_frame* global_log_end_frame(u32 severity_mask);` |

#### Macros

| Macro | Form |
| --- | --- |
| `global_log_fatal` | `global_log_fatal(...)` |
| `global_log_error` | `global_log_error(...)` |
| `global_log_warn` | `global_log_warn(...)` |
| `global_log_info` | `global_log_info(...)` |
| `global_log_debug` | `global_log_debug(...)` |
| `global_log_verbose` | `global_log_verbose(...)` |
| `global_log_trace` | `global_log_trace(...)` |

### `context/thread_ctx.h`

- Types: **0**
- Functions: **18**
- Macros: **8**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `thread_ctx_get` | `func ctx* thread_ctx_get(void);` |
| `thread_ctx_is_init` | `func b32 thread_ctx_is_init(void);` |
| `thread_ctx_init` | `func b32 thread_ctx_init(ctx_setup setup);` |
| `thread_ctx_quit` | `func b32 thread_ctx_quit(void);` |
| `thread_get_allocator` | `func allocator thread_get_allocator(void);` |
| `thread_get_setup` | `func ctx_setup thread_get_setup(void);` |
| `thread_get_log_state` | `func log_state* thread_get_log_state(void);` |
| `thread_get_perm_arena` | `func arena* thread_get_perm_arena(void);` |
| `thread_get_temp_arena` | `func arena* thread_get_temp_arena(void);` |
| `thread_get_perm_heap` | `func heap* thread_get_perm_heap(void);` |
| `thread_get_temp_heap` | `func heap* thread_get_temp_heap(void);` |
| `thread_get_user_data` | `func void* thread_get_user_data(ctx_user_data_idx idx);` |
| `thread_set_user_data` | `func b32 thread_set_user_data(ctx_user_data_idx idx, void* user_data);` |
| `thread_clear_temp` | `func void thread_clear_temp(void);` |
| `thread_log_set_level` | `func void thread_log_set_level(log_level level);` |
| `thread_log_sync` | `func b32 thread_log_sync(void);` |
| `thread_log_begin_frame` | `func void thread_log_begin_frame(void);` |
| `thread_log_end_frame` | `func log_frame* thread_log_end_frame(u32 severity_mask);` |

#### Macros

| Macro | Form |
| --- | --- |
| `THREAD_CTX_USER_DATA_COUNT` | `THREAD_CTX_USER_DATA_COUNT` |
| `thread_log_fatal` | `thread_log_fatal(...)` |
| `thread_log_error` | `thread_log_error(...)` |
| `thread_log_warn` | `thread_log_warn(...)` |
| `thread_log_info` | `thread_log_info(...)` |
| `thread_log_debug` | `thread_log_debug(...)` |
| `thread_log_verbose` | `thread_log_verbose(...)` |
| `thread_log_trace` | `thread_log_trace(...)` |

## filesystem

Paths, files, streams, archives, modules, and filesystem monitoring.

### `filesystem/archive.h`

- Types: **4**
- Functions: **14**
- Macros: **2**

#### Types

| Type | Declaration |
| --- | --- |
| `archive_entry` | `typedef struct archive_entry { ... } archive_entry;` |
| `archive` | `typedef struct archive { ... } archive;` |
| `archive_entry_info` | `typedef struct archive_entry_info { ... } archive_entry_info;` |
| `archive_iterate_callback` | `typedef b32 archive_iterate_callback(const archive_entry_info* info, void* user_data);` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_archive_create` | `func archive _archive_create(callsite site);` |
| `_archive_destroy` | `func void _archive_destroy(archive* arc, callsite site);` |
| `archive_clear` | `func void archive_clear(archive* arc);` |
| `archive_count` | `func sz archive_count(const archive* arc);` |
| `archive_exists` | `func b32 archive_exists(const archive* arc, const path* src);` |
| `archive_load_file` | `func b32 archive_load_file(archive* arc, const path* src);` |
| `archive_save_file` | `func b32 archive_save_file(const archive* arc, const path* dst);` |
| `archive_write_all` | `func b32 archive_write_all(archive* arc, const path* src, buffer data);` |
| `archive_remove` | `func b32 archive_remove(archive* arc, const path* src);` |
| `archive_read_all` | `func b32 archive_read_all(const archive* arc, const path* src, allocator* alloc, buffer* out_data);` |
| `archive_add_file` | `func b32 archive_add_file(archive* arc, const path* archive_path, const path* disk_path);` |
| `archive_iterate` | `func b32 archive_iterate(const archive* arc, archive_iterate_callback* callback, void* user_data);` |
| `archive_get_entry_info` | `func b32 archive_get_entry_info(const archive* arc, const path* src, archive_entry_info* out_info);` |
| `archive_get_entry_data` | `func b32 archive_get_entry_data(const archive* arc, const path* src, buffer* out_data);` |

#### Macros

| Macro | Form |
| --- | --- |
| `archive_create` | `archive_create()` |
| `archive_destroy` | `archive_destroy(arc)` |

### `filesystem/directory.h`

- Types: **3**
- Functions: **12**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `dir_entry` | `typedef struct dir_entry { ... } dir_entry;` |
| `dir_system_path` | `typedef enum dir_system_path { ... } dir_system_path;` |
| `dir_iterate_callback` | `typedef b32 dir_iterate_callback(const dir_entry* entry, void* user_data);` |

#### Functions

| Function | Declaration |
| --- | --- |
| `dir_get_base` | `func path dir_get_base(void);` |
| `dir_get_pref` | `func path dir_get_pref(cstr8 org_name, cstr8 app_name);` |
| `dir_get_system` | `func path dir_get_system(dir_system_path location);` |
| `dir_create` | `func b32 dir_create(const path* src);` |
| `dir_remove` | `func b32 dir_remove(const path* src);` |
| `dir_rename` | `func b32 dir_rename(const path* old_src, const path* new_src);` |
| `dir_cpy_recursive` | `func b32 dir_cpy_recursive(const path* src, const path* dst, b32 overwrite_existing);` |
| `dir_exists` | `func b32 dir_exists(const path* src);` |
| `dir_create_recursive` | `func b32 dir_create_recursive(const path* src);` |
| `dir_remove_recursive` | `func b32 dir_remove_recursive(const path* src);` |
| `dir_iterate` | `func b32 dir_iterate(const path* src, dir_iterate_callback* callback, void* user_data);` |
| `dir_iterate_recursive` | `func b32 dir_iterate_recursive( const path* src, dir_iterate_callback* callback, void* user_data);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `filesystem/file.h`

- Types: **0**
- Functions: **10**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `file_create` | `func b32 file_create(const path* src);` |
| `file_delete` | `func b32 file_delete(const path* src);` |
| `file_rename` | `func b32 file_rename(const path* old_src, const path* new_src);` |
| `file_cpy` | `func b32 file_cpy(const path* src, const path* dst, b32 overwrite_existing);` |
| `file_exists` | `func b32 file_exists(const path* src);` |
| `file_get_size` | `func b32 file_get_size(const path* src, sz* out_size);` |
| `file_read_all` | `func b32 file_read_all(const path* src, allocator* alloc, buffer* out_data);` |
| `file_write_all` | `func b32 file_write_all(const path* src, buffer data);` |
| `file_append_all` | `func b32 file_append_all(const path* src, buffer data);` |
| `file_write_all_atomic` | `func b32 file_write_all_atomic(const path* src, buffer data);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `filesystem/filemap.h`

- Types: **3**
- Functions: **7**
- Macros: **2**

#### Types

| Type | Declaration |
| --- | --- |
| `filemap_access` | `typedef enum filemap_access { ... } filemap_access;` |
| `filemap_error` | `typedef enum filemap_error { ... } filemap_error;` |
| `filemap` | `typedef struct filemap { ... } filemap;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_filemap_open` | `func filemap _filemap_open(const path* src, filemap_access access, callsite site);` |
| `filemap_is_open` | `func b32 filemap_is_open(const filemap* map);` |
| `filemap_flush` | `func b32 filemap_flush(filemap* map);` |
| `_filemap_close` | `func void _filemap_close(filemap* map, callsite site);` |
| `filemap_is_writable` | `func b32 filemap_is_writable(const filemap* map);` |
| `filemap_mark_dirty` | `func void filemap_mark_dirty(filemap* map);` |
| `filemap_get_last_error` | `func filemap_error filemap_get_last_error(void);` |

#### Macros

| Macro | Form |
| --- | --- |
| `filemap_open` | `filemap_open(src, access)` |
| `filemap_close` | `filemap_close(map)` |

### `filesystem/filestream.h`

- Types: **5**
- Functions: **14**
- Macros: **3**

#### Types

| Type | Declaration |
| --- | --- |
| `archive` | `typedef struct archive archive;` |
| `filestream_kind` | `typedef enum filestream_kind { ... } filestream_kind;` |
| `filestream_seek_basis` | `typedef enum filestream_seek_basis { ... } filestream_seek_basis;` |
| `filestream_error` | `typedef enum filestream_error { ... } filestream_error;` |
| `filestream` | `typedef struct filestream { ... } filestream;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_filestream_open` | `func filestream _filestream_open(const path* src, u32 mode_flags, callsite site);` |
| `_filestream_open_archive` | `func filestream _filestream_open_archive(archive* arc, const path* src, u32 mode_flags, callsite site);` |
| `_filestream_close` | `func void _filestream_close(filestream* stm, callsite site);` |
| `filestream_is_open` | `func b32 filestream_is_open(const filestream* stm);` |
| `filestream_flush` | `func b32 filestream_flush(filestream* stm);` |
| `filestream_read` | `func sz filestream_read(filestream* stm, void* dst, sz size);` |
| `filestream_read_exact` | `func b32 filestream_read_exact(filestream* stm, void* dst, sz size);` |
| `filestream_write` | `func sz filestream_write(filestream* stm, const void* src, sz size);` |
| `filestream_write_exact` | `func b32 filestream_write_exact(filestream* stm, const void* src, sz size);` |
| `filestream_seek` | `func b32 filestream_seek(filestream* stm, i64 offset, filestream_seek_basis basis);` |
| `filestream_tell` | `func sz filestream_tell(const filestream* stm);` |
| `filestream_size` | `func sz filestream_size(const filestream* stm);` |
| `filestream_eof` | `func b32 filestream_eof(const filestream* stm);` |
| `filestream_get_error` | `func filestream_error filestream_get_error(const filestream* stm);` |

#### Macros

| Macro | Form |
| --- | --- |
| `filestream_open` | `filestream_open(src, mode_flags)` |
| `filestream_open_archive` | `filestream_open_archive(arc, src, mode_flags)` |
| `filestream_close` | `filestream_close(stm)` |

### `filesystem/module.h`

- Types: **3**
- Functions: **5**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `mod_init_func` | `typedef b32 mod_init_func(void);` |
| `mod_quit_func` | `typedef void mod_quit_func(void);` |
| `mod` | `typedef struct mod { ... } mod;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `mod_open` | `func mod mod_open(const path* src);` |
| `mod_is_open` | `func b32 mod_is_open(const mod* mod_ptr);` |
| `mod_get_func` | `func void* mod_get_func(const mod* mod_ptr, cstr8 name);` |
| `mod_get_extension` | `func cstr8 mod_get_extension(void);` |
| `mod_close` | `func void mod_close(mod* mod_ptr);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `filesystem/path.h`

- Types: **1**
- Functions: **29**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `path` | `typedef struct path { ... } path;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `path_from_cstr` | `func path path_from_cstr(cstr8 src);` |
| `path_from_str8` | `func path path_from_str8(str8 src);` |
| `path_join` | `func path path_join(const path* lhs, const path* rhs);` |
| `path_join_cstr` | `func path path_join_cstr(const path* lhs, cstr8 rhs);` |
| `path_append` | `func sz path_append(path* dst, const path* src);` |
| `path_append_cstr` | `func sz path_append_cstr(path* dst, cstr8 src);` |
| `path_norm` | `func void path_norm(path* src);` |
| `path_norm_trimmed_cpy` | `func path path_norm_trimmed_cpy(const path* src);` |
| `path_cmd_normd` | `func b32 path_cmd_normd(const path* lhs, const path* rhs);` |
| `path_ends_with` | `func b32 path_ends_with(const path* src, cstr8 suffix);` |
| `path_is_absolute` | `func b32 path_is_absolute(const path* src);` |
| `path_is_relative` | `func b32 path_is_relative(const path* src);` |
| `path_remove_extension` | `func void path_remove_extension(path* src);` |
| `path_remove_name` | `func void path_remove_name(path* src);` |
| `path_remove_directory` | `func void path_remove_directory(path* src);` |
| `path_remove_trailing_slash` | `func void path_remove_trailing_slash(path* src);` |
| `path_get_extension` | `func path path_get_extension(const path* src);` |
| `path_get_name` | `func path path_get_name(const path* src);` |
| `path_get_basename` | `func path path_get_basename(const path* src);` |
| `path_get_directory` | `func path path_get_directory(const path* src);` |
| `path_get_common` | `func path path_get_common(const path* src_list, sz path_count);` |
| `path_get_current` | `func path path_get_current(void);` |
| `path_set_current` | `func b32 path_set_current(const path* src);` |
| `path_resolve` | `func path path_resolve(const path* src);` |
| `path_make_relative` | `func path path_make_relative(const path* src, const path* root);` |
| `path_exists` | `func b32 path_exists(const path* src);` |
| `path_remove` | `func b32 path_remove(const path* src);` |
| `path_rename` | `func b32 path_rename(const path* old_src, const path* new_src);` |
| `path_get_last_write_time` | `func timestamp path_get_last_write_time(const path* src);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `filesystem/pathinfo.h`

- Types: **2**
- Functions: **1**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `pathinfo_type` | `typedef enum pathinfo_type { ... } pathinfo_type;` |
| `pathinfo` | `typedef struct pathinfo { ... } pathinfo;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `pathinfo_get` | `func b32 pathinfo_get(const path* src, pathinfo* out_info);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `filesystem/pathwatch.h`

- Types: **5**
- Functions: **13**
- Macros: **4**

#### Types

| Type | Declaration |
| --- | --- |
| `pathwatch_id` | `typedef i64 pathwatch_id;` |
| `pathwatch_watch_id` | `typedef i64 pathwatch_watch_id;` |
| `pathwatch_action` | `typedef enum pathwatch_action { ... } pathwatch_action;` |
| `pathwatch_event` | `typedef struct pathwatch_event { ... } pathwatch_event;` |
| `pathwatch` | `typedef struct pathwatch { ... } pathwatch;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_pathwatch_create` | `func pathwatch _pathwatch_create(b32 use_generic_mode, callsite site);` |
| `_pathwatch_destroy` | `func void _pathwatch_destroy(pathwatch* watcher, callsite site);` |
| `pathwatch_start` | `func b32 pathwatch_start(pathwatch* watcher);` |
| `pathwatch_stop` | `func b32 pathwatch_stop(pathwatch* watcher);` |
| `pathwatch_pause` | `func b32 pathwatch_pause(pathwatch* watcher);` |
| `pathwatch_resume` | `func b32 pathwatch_resume(pathwatch* watcher);` |
| `pathwatch_add` | `func pathwatch_watch_id pathwatch_add(pathwatch* watcher, const path* src, b32 recursive);` |
| `pathwatch_remove` | `func b32 pathwatch_remove(pathwatch* watcher, pathwatch_watch_id watch_id);` |
| `pathwatch_remove_path` | `func b32 pathwatch_remove_path(pathwatch* watcher, const path* src);` |
| `pathwatch_get_path` | `func b32 pathwatch_get_path(pathwatch_watch_id watch_id, path* out_watch_path);` |
| `pathwatch_follow_symlinks` | `func b32 pathwatch_follow_symlinks(pathwatch* watcher, b32 enabled);` |
| `pathwatch_allow_out_of_scope_links` | `func b32 pathwatch_allow_out_of_scope_links(pathwatch* watcher, b32 enabled);` |
| `pathwatch_get_last_error` | `func cstr8 pathwatch_get_last_error(void);` |

#### Macros

| Macro | Form |
| --- | --- |
| `PATHWATCH_BINDING_CAP` | `PATHWATCH_BINDING_CAP` |
| `PATHWATCH_WATCH_BINDING_CAP` | `PATHWATCH_WATCH_BINDING_CAP` |
| `pathwatch_create` | `pathwatch_create(use_generic_mode)` |
| `pathwatch_destroy` | `pathwatch_destroy(watcher)` |

## input

Input devices, events, message payloads, and virtual-key helpers.

### `input/audio_device.h`

- Types: **2**
- Functions: **11**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `audio_device` | `typedef void* audio_device;` |
| `audio_device_type` | `typedef enum audio_device_type { ... } audio_device_type;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `audio_device_is_valid` | `func b32 audio_device_is_valid(audio_device src);` |
| `device_get_audio_device` | `func audio_device device_get_audio_device(device src);` |
| `audio_device_to_device` | `func device audio_device_to_device(audio_device src);` |
| `audio_device_type_is_valid` | `func b32 audio_device_type_is_valid(audio_device_type src);` |
| `audio_device_get_type_name` | `func cstr8 audio_device_get_type_name(audio_device_type audio_type);` |
| `audio_device_get_total_count` | `func sz audio_device_get_total_count(audio_device_type audio_type);` |
| `audio_device_get_from_idx` | `func audio_device audio_device_get_from_idx(audio_device_type audio_type, sz idx);` |
| `audio_device_get_primary` | `func audio_device audio_device_get_primary(audio_device_type audio_type);` |
| `audio_device_get_focused` | `func audio_device audio_device_get_focused(audio_device_type audio_type);` |
| `audio_device_get_type` | `func audio_device_type audio_device_get_type(audio_device aud_id);` |
| `audio_device_get_name` | `func cstr8 audio_device_get_name(audio_device aud_id);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `input/camera.h`

- Types: **2**
- Functions: **14**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `camera` | `typedef void* camera;` |
| `camera_pos` | `typedef enum camera_pos { ... } camera_pos;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `camera_is_valid` | `func b32 camera_is_valid(camera src);` |
| `device_get_camera` | `func camera device_get_camera(device src);` |
| `camera_to_device` | `func device camera_to_device(camera src);` |
| `camera_get_total_count` | `func sz camera_get_total_count(void);` |
| `camera_get_from_idx` | `func camera camera_get_from_idx(sz idx);` |
| `camera_get_primary` | `func camera camera_get_primary(void);` |
| `camera_get_focused` | `func camera camera_get_focused(void);` |
| `camera_get_name` | `func cstr8 camera_get_name(camera cam_id);` |
| `camera_get_pos` | `func camera_pos camera_get_pos(camera cam_id);` |
| `camera_open` | `func b32 camera_open(camera cam_id);` |
| `camera_close` | `func b32 camera_close(camera cam_id);` |
| `camera_start` | `func b32 camera_start(camera cam_id);` |
| `camera_stop` | `func b32 camera_stop(camera cam_id);` |
| `camera_read` | `func b32 camera_read(camera cam_id, buffer* out_frame);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `input/clipboard.h`

- Types: **0**
- Functions: **7**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `clipboard_has_text` | `func b32 clipboard_has_text(void);` |
| `clipboard_has_data` | `func b32 clipboard_has_data(cstr8 mime_type);` |
| `clipboard_set_text` | `func b32 clipboard_set_text(cstr8 src);` |
| `clipboard_set_data` | `func b32 clipboard_set_data(cstr8 mime_type, buffer src_data);` |
| `clipboard_get_text` | `func b32 clipboard_get_text(c8* out_text, sz out_capacity);` |
| `clipboard_get_data` | `func b32 clipboard_get_data(cstr8 mime_type, buffer out_data, sz* out_size);` |
| `clipboard_clear` | `func b32 clipboard_clear(void);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `input/devices.h`

- Types: **4**
- Functions: **10**
- Macros: **1**

#### Types

| Type | Declaration |
| --- | --- |
| `device` | `typedef void* device;` |
| `battery_state` | `typedef enum battery_state { ... } battery_state;` |
| `device_type` | `typedef enum device_type { ... } device_type;` |
| `device_info` | `typedef struct device_info { ... } device_info;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `device_is_valid` | `func b32 device_is_valid(device src);` |
| `devices_get_type` | `func device_type devices_get_type(device src);` |
| `device_get_handle` | `func void* device_get_handle(device src);` |
| `device_get_state` | `func u64 device_get_state(device src);` |
| `devices_get_type_name` | `func cstr8 devices_get_type_name(device_type type);` |
| `devices_get_count` | `func sz devices_get_count(device_type type);` |
| `devices_get_type_count` | `func u32 devices_get_type_count(device_type type);` |
| `devices_get_device` | `func device devices_get_device(device_type type, sz idx);` |
| `devices_is_connected` | `func b32 devices_is_connected(device dev_id);` |
| `devices_get_info` | `func b32 devices_get_info(device dev_id, device_info* out_info);` |

#### Macros

| Macro | Form |
| --- | --- |
| `DEVICES_HANDLE_CAP` | `DEVICES_HANDLE_CAP` |

### `input/gamepads.h`

- Types: **6**
- Functions: **17**
- Macros: **1**

#### Types

| Type | Declaration |
| --- | --- |
| `gamepad` | `typedef void* gamepad;` |
| `gamepad_button` | `typedef enum gamepad_button { ... } gamepad_button;` |
| `gamepad_axis` | `typedef enum gamepad_axis { ... } gamepad_axis;` |
| `gamepad_touchpad_idx` | `typedef i32 gamepad_touchpad_idx;` |
| `gamepad_finger_idx` | `typedef i32 gamepad_finger_idx;` |
| `gamepad_sensor_kind` | `typedef enum gamepad_sensor_kind { ... } gamepad_sensor_kind;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `gamepad_is_valid` | `func b32 gamepad_is_valid(gamepad src);` |
| `device_get_gamepad` | `func gamepad device_get_gamepad(device src);` |
| `gamepad_to_device` | `func device gamepad_to_device(gamepad src);` |
| `gamepad_get_total_count` | `func sz gamepad_get_total_count(void);` |
| `gamepad_get_from_idx` | `func gamepad gamepad_get_from_idx(sz idx);` |
| `gamepad_get_primary` | `func gamepad gamepad_get_primary(void);` |
| `gamepad_get_focused` | `func gamepad gamepad_get_focused(void);` |
| `gamepad_is_connected` | `func b32 gamepad_is_connected(gamepad src);` |
| `gamepad_get_name` | `func cstr8 gamepad_get_name(gamepad src);` |
| `gamepad_has_button` | `func b32 gamepad_has_button(gamepad src, gamepad_button button);` |
| `gamepad_get_button` | `func b32 gamepad_get_button(gamepad src, gamepad_button button);` |
| `gamepad_has_axis` | `func b32 gamepad_has_axis(gamepad src, gamepad_axis axis);` |
| `gamepad_get_axis` | `func i16 gamepad_get_axis(gamepad src, gamepad_axis axis);` |
| `gamepad_set_rumble` | `func b32 gamepad_set_rumble(gamepad src, u16 low_freq, u16 high_freq, u32 duration_ms);` |
| `gamepad_set_led` | `func b32 gamepad_set_led(gamepad src, u8 red, u8 green, u8 blue);` |
| `gamepad_set_axis_deadzone` | `func b32 gamepad_set_axis_deadzone(gamepad src, gamepad_axis axis, i16 deadzone);` |
| `gamepad_get_axis_deadzone` | `func i16 gamepad_get_axis_deadzone(gamepad src, gamepad_axis axis);` |

#### Macros

| Macro | Form |
| --- | --- |
| `GAMEPADS_MAX_COUNT` | `GAMEPADS_MAX_COUNT` |

### `input/joystick.h`

- Types: **2**
- Functions: **18**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `joystick` | `typedef void* joystick;` |
| `joystick_hat_state` | `typedef enum joystick_hat_state { ... } joystick_hat_state;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `joystick_is_valid` | `func b32 joystick_is_valid(joystick src);` |
| `device_get_joystick` | `func joystick device_get_joystick(device src);` |
| `joystick_to_device` | `func device joystick_to_device(joystick src);` |
| `joystick_get_total_count` | `func sz joystick_get_total_count(void);` |
| `joystick_get_from_idx` | `func joystick joystick_get_from_idx(sz idx);` |
| `joystick_get_primary` | `func joystick joystick_get_primary(void);` |
| `joystick_get_focused` | `func joystick joystick_get_focused(void);` |
| `joystick_get_name` | `func cstr8 joystick_get_name(joystick joy_id);` |
| `joystick_is_connected` | `func b32 joystick_is_connected(joystick joy_id);` |
| `joystick_get_axis_count` | `func sz joystick_get_axis_count(joystick joy_id);` |
| `joystick_get_ball_count` | `func sz joystick_get_ball_count(joystick joy_id);` |
| `joystick_get_hat_count` | `func sz joystick_get_hat_count(joystick joy_id);` |
| `joystick_get_button_count` | `func sz joystick_get_button_count(joystick joy_id);` |
| `joystick_get_axis` | `func i16 joystick_get_axis(joystick joy_id, sz axis_idx);` |
| `joystick_get_ball` | `func b32 joystick_get_ball(joystick joy_id, sz ball_idx, i32* out_xrel, i32* out_yrel);` |
| `joystick_get_hat` | `func joystick_hat_state joystick_get_hat(joystick joy_id, sz hat_idx);` |
| `joystick_get_button` | `func b32 joystick_get_button(joystick joy_id, sz button_idx);` |
| `joystick_get_battery` | `func battery_state joystick_get_battery(joystick joy_id, i32* out_percent);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `input/keyboard.h`

- Types: **1**
- Functions: **13**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `keyboard` | `typedef void* keyboard;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `keyboard_is_valid` | `func b32 keyboard_is_valid(keyboard src);` |
| `device_get_keyboard` | `func keyboard device_get_keyboard(device src);` |
| `keyboard_to_device` | `func device keyboard_to_device(keyboard src);` |
| `keyboard_is_available` | `func b32 keyboard_is_available(void);` |
| `keyboard_get_primary` | `func keyboard keyboard_get_primary(void);` |
| `keyboard_get_focused` | `func keyboard keyboard_get_focused(void);` |
| `keyboard_is_key_down` | `func b32 keyboard_is_key_down(keyboard src, vkey key);` |
| `keyboard_get_mods` | `func keymod keyboard_get_mods(keyboard src);` |
| `keyboard_has_mods` | `func b32 keyboard_has_mods(keyboard src, keymod required_mods);` |
| `keyboard_has_mods_exact` | `func b32 keyboard_has_mods_exact(keyboard src, keymod required_mods, keymod forbidden_mods);` |
| `keyboard_is_key_down_mod` | `func b32 keyboard_is_key_down_mod(keyboard src, vkey key, keymod required_mods, keymod forbidden_mods);` |
| `keyboard_get_key_name` | `func cstr8 keyboard_get_key_name(keyboard src, vkey key);` |
| `keyboard_get_key_display_name` | `func cstr8 keyboard_get_key_display_name(keyboard src, vkey key, keymod modifiers, b32 key_event);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `input/mouse.h`

- Types: **4**
- Functions: **8**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `mouse` | `typedef void* mouse;` |
| `mouse_button` | `typedef enum mouse_button { ... } mouse_button;` |
| `mouse_wheel_direction` | `typedef enum mouse_wheel_direction { ... } mouse_wheel_direction;` |
| `mouse_state` | `typedef u32 mouse_state;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `mouse_is_valid` | `func b32 mouse_is_valid(mouse src);` |
| `device_get_mouse` | `func mouse device_get_mouse(device src);` |
| `mouse_to_device` | `func device mouse_to_device(mouse src);` |
| `mouse_is_available` | `func b32 mouse_is_available(void);` |
| `mouse_get_primary` | `func mouse mouse_get_primary(void);` |
| `mouse_get_focused` | `func mouse mouse_get_focused(void);` |
| `mouse_get_state` | `func mouse_state mouse_get_state(mouse src);` |
| `mouse_is_button_down` | `func b32 mouse_is_button_down(mouse src, mouse_button button);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `input/msg.h`

- Types: **5**
- Functions: **5**
- Macros: **3**

#### Types

| Type | Declaration |
| --- | --- |
| `msg_category` | `typedef enum msg_category { ... } msg_category;` |
| `msg` | `typedef struct msg { ... } msg;` |
| `msg_handler_fn` | `typedef b32 (*msg_handler_fn)(msg* src, void* user_data);` |
| `msg_filter_fn` | `typedef b32 (*msg_filter_fn)(const msg* src, void* user_data);` |
| `msg_handler_desc` | `typedef struct msg_handler_desc { ... } msg_handler_desc;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `msg_poll` | `func b32 msg_poll(msg* out_msg);` |
| `_msg_post` | `func b32 _msg_post(const msg* src, callsite site);` |
| `msg_add_handler` | `func u64 msg_add_handler(const msg_handler_desc* desc);` |
| `msg_remove_handler` | `func b32 msg_remove_handler(u64 handler_id);` |
| `msg_set_filter` | `func void msg_set_filter(msg_filter_fn filter_fn, void* user_data);` |

#### Macros

| Macro | Form |
| --- | --- |
| `MSG_HANDLER_CAP` | `MSG_HANDLER_CAP` |
| `MSG_DATA_SIZE` | `MSG_DATA_SIZE` |
| `msg_post` | `msg_post(src)` |

### `input/msg_core.h`

- Types: **49**
- Functions: **84**
- Macros: **2**

#### Types

| Type | Declaration |
| --- | --- |
| `msg_core_type` | `typedef enum msg_core_type { ... } msg_core_type;` |
| `msg_core_object_event_kind` | `typedef enum msg_core_object_event_kind { ... } msg_core_object_event_kind;` |
| `msg_core_object_type` | `typedef enum msg_core_object_type { ... } msg_core_object_type;` |
| `msg_core_thread_ctx_event_kind` | `typedef enum msg_core_thread_ctx_event_kind { ... } msg_core_thread_ctx_event_kind;` |
| `msg_core_pathwatch_event_kind` | `typedef enum msg_core_pathwatch_event_kind { ... } msg_core_pathwatch_event_kind;` |
| `msg_core_global_ctx_event_kind` | `typedef enum msg_core_global_ctx_event_kind { ... } msg_core_global_ctx_event_kind;` |
| `msg_core_monitor_data` | `typedef struct msg_core_monitor_data { ... } msg_core_monitor_data;` |
| `msg_core_window_data` | `typedef struct msg_core_window_data { ... } msg_core_window_data;` |
| `msg_core_keyboard_device_data` | `typedef struct msg_core_keyboard_device_data { ... } msg_core_keyboard_device_data;` |
| `msg_core_keyboard_data` | `typedef struct msg_core_keyboard_data { ... } msg_core_keyboard_data;` |
| `msg_core_text_editing_data` | `typedef struct msg_core_text_editing_data { ... } msg_core_text_editing_data;` |
| `msg_core_text_editing_candidates_data` | `typedef struct msg_core_text_editing_candidates_data { ... } msg_core_text_editing_candidates_data;` |
| `msg_core_text_input_data` | `typedef struct msg_core_text_input_data { ... } msg_core_text_input_data;` |
| `msg_core_mouse_device_data` | `typedef struct msg_core_mouse_device_data { ... } msg_core_mouse_device_data;` |
| `msg_core_mouse_motion_data` | `typedef struct msg_core_mouse_motion_data { ... } msg_core_mouse_motion_data;` |
| `msg_core_mouse_button_data` | `typedef struct msg_core_mouse_button_data { ... } msg_core_mouse_button_data;` |
| `msg_core_mouse_wheel_data` | `typedef struct msg_core_mouse_wheel_data { ... } msg_core_mouse_wheel_data;` |
| `msg_core_joystick_device_data` | `typedef struct msg_core_joystick_device_data { ... } msg_core_joystick_device_data;` |
| `msg_core_joystick_axis_data` | `typedef struct msg_core_joystick_axis_data { ... } msg_core_joystick_axis_data;` |
| `msg_core_joystick_ball_data` | `typedef struct msg_core_joystick_ball_data { ... } msg_core_joystick_ball_data;` |
| `msg_core_joystick_hat_data` | `typedef struct msg_core_joystick_hat_data { ... } msg_core_joystick_hat_data;` |
| `msg_core_joystick_button_data` | `typedef struct msg_core_joystick_button_data { ... } msg_core_joystick_button_data;` |
| `msg_core_joystick_battery_data` | `typedef struct msg_core_joystick_battery_data { ... } msg_core_joystick_battery_data;` |
| `msg_core_gamepad_device_data` | `typedef struct msg_core_gamepad_device_data { ... } msg_core_gamepad_device_data;` |
| `msg_core_gamepad_axis_data` | `typedef struct msg_core_gamepad_axis_data { ... } msg_core_gamepad_axis_data;` |
| `msg_core_gamepad_button_data` | `typedef struct msg_core_gamepad_button_data { ... } msg_core_gamepad_button_data;` |
| `msg_core_gamepad_touchpad_data` | `typedef struct msg_core_gamepad_touchpad_data { ... } msg_core_gamepad_touchpad_data;` |
| `msg_core_gamepad_sensor_data` | `typedef struct msg_core_gamepad_sensor_data { ... } msg_core_gamepad_sensor_data;` |
| `msg_core_audio_device_data` | `typedef struct msg_core_audio_device_data { ... } msg_core_audio_device_data;` |
| `msg_core_camera_device_data` | `typedef struct msg_core_camera_device_data { ... } msg_core_camera_device_data;` |
| `msg_core_render_data` | `typedef struct msg_core_render_data { ... } msg_core_render_data;` |
| `msg_core_touch_device_data` | `typedef struct msg_core_touch_device_data { ... } msg_core_touch_device_data;` |
| `msg_core_tablet_device_data` | `typedef struct msg_core_tablet_device_data { ... } msg_core_tablet_device_data;` |
| `msg_core_touch_data` | `typedef struct msg_core_touch_data { ... } msg_core_touch_data;` |
| `msg_core_pen_proximity_data` | `typedef struct msg_core_pen_proximity_data { ... } msg_core_pen_proximity_data;` |
| `msg_core_pen_motion_data` | `typedef struct msg_core_pen_motion_data { ... } msg_core_pen_motion_data;` |
| `msg_core_pen_touch_data` | `typedef struct msg_core_pen_touch_data { ... } msg_core_pen_touch_data;` |
| `msg_core_pen_button_data` | `typedef struct msg_core_pen_button_data { ... } msg_core_pen_button_data;` |
| `msg_core_pen_axis_data` | `typedef struct msg_core_pen_axis_data { ... } msg_core_pen_axis_data;` |
| `msg_core_drop_data` | `typedef struct msg_core_drop_data { ... } msg_core_drop_data;` |
| `msg_core_clipboard_data` | `typedef struct msg_core_clipboard_data { ... } msg_core_clipboard_data;` |
| `msg_core_sensor_data` | `typedef struct msg_core_sensor_data { ... } msg_core_sensor_data;` |
| `msg_core_object_lifecycle_data` | `typedef struct msg_core_object_lifecycle_data { ... } msg_core_object_lifecycle_data;` |
| `msg_core_thread_ctx_data` | `typedef struct msg_core_thread_ctx_data { ... } msg_core_thread_ctx_data;` |
| `msg_core_pathwatch_data` | `typedef struct msg_core_pathwatch_data { ... } msg_core_pathwatch_data;` |
| `msg_core_log_data` | `typedef struct msg_core_log_data { ... } msg_core_log_data;` |
| `msg_core_global_ctx_data` | `typedef struct msg_core_global_ctx_data { ... } msg_core_global_ctx_data;` |
| `msg_core_assert_data` | `typedef struct msg_core_assert_data { ... } msg_core_assert_data;` |
| `msg_core_data` | `typedef union msg_core_data { ... } msg_core_data;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `msg_core_fill_monitor` | `func void msg_core_fill_monitor(msg* src, const msg_core_monitor_data* core_data);` |
| `msg_core_fill_window` | `func void msg_core_fill_window(msg* src, const msg_core_window_data* core_data);` |
| `msg_core_fill_keyboard_device` | `func void msg_core_fill_keyboard_device(msg* src, const msg_core_keyboard_device_data* core_data);` |
| `msg_core_fill_keyboard` | `func void msg_core_fill_keyboard(msg* src, const msg_core_keyboard_data* core_data);` |
| `msg_core_fill_text_editing` | `func void msg_core_fill_text_editing(msg* src, const msg_core_text_editing_data* core_data);` |
| `msg_core_fill_text_editing_candidates` | `func void msg_core_fill_text_editing_candidates(msg* src, const msg_core_text_editing_candidates_data* core_data);` |
| `msg_core_fill_text_input` | `func void msg_core_fill_text_input(msg* src, const msg_core_text_input_data* core_data);` |
| `msg_core_fill_mouse_device` | `func void msg_core_fill_mouse_device(msg* src, const msg_core_mouse_device_data* core_data);` |
| `msg_core_fill_mouse_motion` | `func void msg_core_fill_mouse_motion(msg* src, const msg_core_mouse_motion_data* core_data);` |
| `msg_core_fill_mouse_button` | `func void msg_core_fill_mouse_button(msg* src, const msg_core_mouse_button_data* core_data);` |
| `msg_core_fill_mouse_wheel` | `func void msg_core_fill_mouse_wheel(msg* src, const msg_core_mouse_wheel_data* core_data);` |
| `msg_core_fill_joystick_device` | `func void msg_core_fill_joystick_device(msg* src, const msg_core_joystick_device_data* core_data);` |
| `msg_core_fill_joystick_axis` | `func void msg_core_fill_joystick_axis(msg* src, const msg_core_joystick_axis_data* core_data);` |
| `msg_core_fill_joystick_ball` | `func void msg_core_fill_joystick_ball(msg* src, const msg_core_joystick_ball_data* core_data);` |
| `msg_core_fill_joystick_hat` | `func void msg_core_fill_joystick_hat(msg* src, const msg_core_joystick_hat_data* core_data);` |
| `msg_core_fill_joystick_button` | `func void msg_core_fill_joystick_button(msg* src, const msg_core_joystick_button_data* core_data);` |
| `msg_core_fill_joystick_battery` | `func void msg_core_fill_joystick_battery(msg* src, const msg_core_joystick_battery_data* core_data);` |
| `msg_core_fill_gamepad_device` | `func void msg_core_fill_gamepad_device(msg* src, const msg_core_gamepad_device_data* core_data);` |
| `msg_core_fill_gamepad_axis` | `func void msg_core_fill_gamepad_axis(msg* src, const msg_core_gamepad_axis_data* core_data);` |
| `msg_core_fill_gamepad_button` | `func void msg_core_fill_gamepad_button(msg* src, const msg_core_gamepad_button_data* core_data);` |
| `msg_core_fill_gamepad_touchpad` | `func void msg_core_fill_gamepad_touchpad(msg* src, const msg_core_gamepad_touchpad_data* core_data);` |
| `msg_core_fill_gamepad_sensor` | `func void msg_core_fill_gamepad_sensor(msg* src, const msg_core_gamepad_sensor_data* core_data);` |
| `msg_core_fill_audio_device` | `func void msg_core_fill_audio_device(msg* src, const msg_core_audio_device_data* core_data);` |
| `msg_core_fill_camera_device` | `func void msg_core_fill_camera_device(msg* src, const msg_core_camera_device_data* core_data);` |
| `msg_core_fill_render` | `func void msg_core_fill_render(msg* src, const msg_core_render_data* core_data);` |
| `msg_core_fill_touch_device` | `func void msg_core_fill_touch_device(msg* src, const msg_core_touch_device_data* core_data);` |
| `msg_core_fill_touch` | `func void msg_core_fill_touch(msg* src, const msg_core_touch_data* core_data);` |
| `msg_core_fill_tablet_device` | `func void msg_core_fill_tablet_device(msg* src, const msg_core_tablet_device_data* core_data);` |
| `msg_core_fill_pen_proximity` | `func void msg_core_fill_pen_proximity(msg* src, const msg_core_pen_proximity_data* core_data);` |
| `msg_core_fill_pen_motion` | `func void msg_core_fill_pen_motion(msg* src, const msg_core_pen_motion_data* core_data);` |
| `msg_core_fill_pen_touch` | `func void msg_core_fill_pen_touch(msg* src, const msg_core_pen_touch_data* core_data);` |
| `msg_core_fill_pen_button` | `func void msg_core_fill_pen_button(msg* src, const msg_core_pen_button_data* core_data);` |
| `msg_core_fill_pen_axis` | `func void msg_core_fill_pen_axis(msg* src, const msg_core_pen_axis_data* core_data);` |
| `msg_core_fill_drop` | `func void msg_core_fill_drop(msg* src, const msg_core_drop_data* core_data);` |
| `msg_core_fill_clipboard` | `func void msg_core_fill_clipboard(msg* src, const msg_core_clipboard_data* core_data);` |
| `msg_core_fill_sensor` | `func void msg_core_fill_sensor(msg* src, const msg_core_sensor_data* core_data);` |
| `msg_core_fill_object_lifecycle` | `func void msg_core_fill_object_lifecycle(msg* src, const msg_core_object_lifecycle_data* core_data);` |
| `msg_core_fill_thread_ctx` | `func void msg_core_fill_thread_ctx(msg* src, const msg_core_thread_ctx_data* core_data);` |
| `msg_core_fill_pathwatch` | `func void msg_core_fill_pathwatch(msg* src, const msg_core_pathwatch_data* core_data);` |
| `msg_core_fill_log` | `func void msg_core_fill_log(msg* src, const msg_core_log_data* core_data);` |
| `msg_core_fill_assert` | `func void msg_core_fill_assert(msg* src, const msg_core_assert_data* core_data);` |
| `msg_core_fill_global_ctx` | `func void msg_core_fill_global_ctx(msg* src, const msg_core_global_ctx_data* core_data);` |
| `msg_core_get_monitor` | `func msg_core_monitor_data* msg_core_get_monitor(msg* src);` |
| `msg_core_get_window` | `func msg_core_window_data* msg_core_get_window(msg* src);` |
| `msg_core_get_keyboard_device` | `func msg_core_keyboard_device_data* msg_core_get_keyboard_device(msg* src);` |
| `msg_core_get_keyboard` | `func msg_core_keyboard_data* msg_core_get_keyboard(msg* src);` |
| `msg_core_get_text_editing` | `func msg_core_text_editing_data* msg_core_get_text_editing(msg* src);` |
| `msg_core_get_text_editing_candidates` | `func msg_core_text_editing_candidates_data* msg_core_get_text_editing_candidates(msg* src);` |
| `msg_core_get_text_input` | `func msg_core_text_input_data* msg_core_get_text_input(msg* src);` |
| `msg_core_get_mouse_device` | `func msg_core_mouse_device_data* msg_core_get_mouse_device(msg* src);` |
| `msg_core_get_mouse_motion` | `func msg_core_mouse_motion_data* msg_core_get_mouse_motion(msg* src);` |
| `msg_core_get_mouse_button` | `func msg_core_mouse_button_data* msg_core_get_mouse_button(msg* src);` |
| `msg_core_get_mouse_wheel` | `func msg_core_mouse_wheel_data* msg_core_get_mouse_wheel(msg* src);` |
| `msg_core_get_joystick_device` | `func msg_core_joystick_device_data* msg_core_get_joystick_device(msg* src);` |
| `msg_core_get_joystick_axis` | `func msg_core_joystick_axis_data* msg_core_get_joystick_axis(msg* src);` |
| `msg_core_get_joystick_ball` | `func msg_core_joystick_ball_data* msg_core_get_joystick_ball(msg* src);` |
| `msg_core_get_joystick_hat` | `func msg_core_joystick_hat_data* msg_core_get_joystick_hat(msg* src);` |
| `msg_core_get_joystick_button` | `func msg_core_joystick_button_data* msg_core_get_joystick_button(msg* src);` |
| `msg_core_get_joystick_battery` | `func msg_core_joystick_battery_data* msg_core_get_joystick_battery(msg* src);` |
| `msg_core_get_gamepad_device` | `func msg_core_gamepad_device_data* msg_core_get_gamepad_device(msg* src);` |
| `msg_core_get_gamepad_axis` | `func msg_core_gamepad_axis_data* msg_core_get_gamepad_axis(msg* src);` |
| `msg_core_get_gamepad_button` | `func msg_core_gamepad_button_data* msg_core_get_gamepad_button(msg* src);` |
| `msg_core_get_gamepad_touchpad` | `func msg_core_gamepad_touchpad_data* msg_core_get_gamepad_touchpad(msg* src);` |
| `msg_core_get_gamepad_sensor` | `func msg_core_gamepad_sensor_data* msg_core_get_gamepad_sensor(msg* src);` |
| `msg_core_get_audio_device` | `func msg_core_audio_device_data* msg_core_get_audio_device(msg* src);` |
| `msg_core_get_camera_device` | `func msg_core_camera_device_data* msg_core_get_camera_device(msg* src);` |
| `msg_core_get_render` | `func msg_core_render_data* msg_core_get_render(msg* src);` |
| `msg_core_get_touch_device` | `func msg_core_touch_device_data* msg_core_get_touch_device(msg* src);` |
| `msg_core_get_touch` | `func msg_core_touch_data* msg_core_get_touch(msg* src);` |
| `msg_core_get_tablet_device` | `func msg_core_tablet_device_data* msg_core_get_tablet_device(msg* src);` |
| `msg_core_get_pen_proximity` | `func msg_core_pen_proximity_data* msg_core_get_pen_proximity(msg* src);` |
| `msg_core_get_pen_motion` | `func msg_core_pen_motion_data* msg_core_get_pen_motion(msg* src);` |
| `msg_core_get_pen_touch` | `func msg_core_pen_touch_data* msg_core_get_pen_touch(msg* src);` |
| `msg_core_get_pen_button` | `func msg_core_pen_button_data* msg_core_get_pen_button(msg* src);` |
| `msg_core_get_pen_axis` | `func msg_core_pen_axis_data* msg_core_get_pen_axis(msg* src);` |
| `msg_core_get_drop` | `func msg_core_drop_data* msg_core_get_drop(msg* src);` |
| `msg_core_get_clipboard` | `func msg_core_clipboard_data* msg_core_get_clipboard(msg* src);` |
| `msg_core_get_sensor` | `func msg_core_sensor_data* msg_core_get_sensor(msg* src);` |
| `msg_core_get_object_lifecycle` | `func msg_core_object_lifecycle_data* msg_core_get_object_lifecycle(msg* src);` |
| `msg_core_get_thread_ctx` | `func msg_core_thread_ctx_data* msg_core_get_thread_ctx(msg* src);` |
| `msg_core_get_pathwatch` | `func msg_core_pathwatch_data* msg_core_get_pathwatch(msg* src);` |
| `msg_core_get_log` | `func msg_core_log_data* msg_core_get_log(msg* src);` |
| `msg_core_get_assert` | `func msg_core_assert_data* msg_core_get_assert(msg* src);` |
| `msg_core_get_global_ctx` | `func msg_core_global_ctx_data* msg_core_get_global_ctx(msg* src);` |

#### Macros

| Macro | Form |
| --- | --- |
| `MSG_LOG_TEXT_CAP` | `MSG_LOG_TEXT_CAP` |
| `MSG_ASSERT_TEXT_CAP` | `MSG_ASSERT_TEXT_CAP` |

### `input/sensor.h`

- Types: **2**
- Functions: **14**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `sensor` | `typedef void* sensor;` |
| `sensor_kind` | `typedef enum sensor_kind { ... } sensor_kind;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `sensor_is_valid` | `func b32 sensor_is_valid(sensor src);` |
| `device_get_sensor` | `func sensor device_get_sensor(device src);` |
| `sensor_to_device` | `func device sensor_to_device(sensor src);` |
| `sensor_get_total_count` | `func sz sensor_get_total_count(void);` |
| `sensor_get_from_idx` | `func sensor sensor_get_from_idx(sz idx);` |
| `sensor_get_primary` | `func sensor sensor_get_primary(void);` |
| `sensor_get_focused` | `func sensor sensor_get_focused(void);` |
| `sensor_get_name` | `func cstr8 sensor_get_name(sensor sen_id);` |
| `sensor_get_kind` | `func sensor_kind sensor_get_kind(sensor sen_id);` |
| `sensor_open` | `func b32 sensor_open(sensor sen_id);` |
| `sensor_close` | `func b32 sensor_close(sensor sen_id);` |
| `sensor_start` | `func b32 sensor_start(sensor sen_id);` |
| `sensor_stop` | `func b32 sensor_stop(sensor sen_id);` |
| `sensor_read` | `func b32 sensor_read(sensor sen_id, buffer* out_samples);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `input/tablet.h`

- Types: **7**
- Functions: **10**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `tablet` | `typedef void* tablet;` |
| `pen_id` | `typedef u32 pen_id;` |
| `tablet_axis` | `typedef enum tablet_axis { ... } tablet_axis;` |
| `tablet_input_flag` | `typedef enum tablet_input_flag { ... } tablet_input_flag;` |
| `tablet_input_flags` | `typedef u32 tablet_input_flags;` |
| `tablet_button` | `typedef enum tablet_button { ... } tablet_button;` |
| `tablet_pen_state` | `typedef struct tablet_pen_state { ... } tablet_pen_state;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `tablet_is_valid` | `func b32 tablet_is_valid(tablet src);` |
| `device_get_tablet` | `func tablet device_get_tablet(device src);` |
| `tablet_to_device` | `func device tablet_to_device(tablet src);` |
| `tablet_is_available` | `func b32 tablet_is_available(void);` |
| `tablet_get_total_count` | `func sz tablet_get_total_count(void);` |
| `tablet_get_from_idx` | `func tablet tablet_get_from_idx(sz idx);` |
| `tablet_get_primary` | `func tablet tablet_get_primary(void);` |
| `tablet_get_focused` | `func tablet tablet_get_focused(void);` |
| `tablet_get_last_pen_state` | `func b32 tablet_get_last_pen_state(tablet src, tablet_pen_state* out_state);` |
| `tablet_read_hid_report` | `func b32 tablet_read_hid_report(tablet src, void* dst, sz capacity, sz* out_size, i32 timeout_ms);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `input/touch.h`

- Types: **4**
- Functions: **11**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `touch` | `typedef void* touch;` |
| `finger_id` | `typedef u64 finger_id;` |
| `touch_device_kind` | `typedef enum touch_device_kind { ... } touch_device_kind;` |
| `touch_finger_state` | `typedef struct touch_finger_state { ... } touch_finger_state;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `touch_is_valid` | `func b32 touch_is_valid(touch src);` |
| `device_get_touch` | `func touch device_get_touch(device src);` |
| `touch_to_device` | `func device touch_to_device(touch src);` |
| `touch_is_available` | `func b32 touch_is_available(void);` |
| `touch_get_total_count` | `func sz touch_get_total_count(void);` |
| `touch_get_from_idx` | `func touch touch_get_from_idx(sz idx);` |
| `touch_get_primary` | `func touch touch_get_primary(void);` |
| `touch_get_focused` | `func touch touch_get_focused(void);` |
| `touch_get_device_kind` | `func touch_device_kind touch_get_device_kind(touch src);` |
| `touch_get_finger_count` | `func sz touch_get_finger_count(touch src);` |
| `touch_get_finger` | `func b32 touch_get_finger(touch src, sz idx, touch_finger_state* out_finger);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `input/vkeys.h`

- Types: **2**
- Functions: **1**
- Macros: **1**

#### Types

| Type | Declaration |
| --- | --- |
| `vkey` | `typedef enum vkey { ... } vkey;` |
| `keymod` | `typedef enum keymod { ... } keymod;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `BASED_VKEY_LIST` | `func force_inline b32 vkey_is_valid(vkey key) { switch (key) { #define BASED_VKEY_CASE(name, value) case name: BASED_VKEY_LIST(BASED_VKEY_CASE) #undef BASED_VKEY_CASE return true;` |

#### Macros

| Macro | Form |
| --- | --- |
| `BASED_VKEY_LIST` | `BASED_VKEY_LIST(X)` |

## interface

Windowing, monitor, cursor, icon, dialog, DPI, and text-input APIs.

### `interface/cursor.h`

- Types: **1**
- Functions: **12**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `cursor_pos` | `typedef struct cursor_pos { ... } cursor_pos;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `cursor_set_icon` | `func b32 cursor_set_icon(icon icon_id);` |
| `cursor_reset_icon` | `func b32 cursor_reset_icon(void);` |
| `cursor_set_visible` | `func b32 cursor_set_visible(b32 visible);` |
| `cursor_is_visible` | `func b32 cursor_is_visible(void);` |
| `cursor_set_capture` | `func b32 cursor_set_capture(window opt_window, b32 enabled);` |
| `cursor_is_captured` | `func b32 cursor_is_captured(window opt_window);` |
| `cursor_set_relative_mode` | `func b32 cursor_set_relative_mode(window opt_window, b32 enabled);` |
| `cursor_is_relative_mode` | `func b32 cursor_is_relative_mode(window opt_window);` |
| `cursor_get_pos` | `func cursor_pos cursor_get_pos(void);` |
| `cursor_get_global_pos` | `func cursor_pos cursor_get_global_pos(void);` |
| `cursor_get_relative_pos` | `func cursor_pos cursor_get_relative_pos(void);` |
| `cursor_warp` | `func b32 cursor_warp(window opt_window, f32 xpos, f32 ypos);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `interface/dialog.h`

- Types: **9**
- Functions: **5**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `dialog_message_kind` | `typedef enum dialog_message_kind { ... } dialog_message_kind;` |
| `dialog_message_box_flag` | `typedef enum dialog_message_box_flag { ... } dialog_message_box_flag;` |
| `dialog_message_box_button_flag` | `typedef enum dialog_message_box_button_flag { ... } dialog_message_box_button_flag;` |
| `dialog_message_box_button` | `typedef struct dialog_message_box_button { ... } dialog_message_box_button;` |
| `dialog_message_box_color` | `typedef struct dialog_message_box_color { ... } dialog_message_box_color;` |
| `dialog_message_box_color_scheme` | `typedef struct dialog_message_box_color_scheme { ... } dialog_message_box_color_scheme;` |
| `dialog_message_box` | `typedef struct dialog_message_box { ... } dialog_message_box;` |
| `dialog_file_filter` | `typedef struct dialog_file_filter { ... } dialog_file_filter;` |
| `dialog_file_callback` | `typedef void dialog_file_callback( void* user_data, cstr8 const* file_list, i32 filter_idx, b32 is_cancelled, b32 has_error);` |

#### Functions

| Function | Declaration |
| --- | --- |
| `dialog_show_message` | `func b32 dialog_show_message( window owner, dialog_message_kind message_kind, cstr8 title, cstr8 message);` |
| `dialog_show_message_box` | `func b32 dialog_show_message_box(window owner, const dialog_message_box* message_box, i32* out_button_id);` |
| `dialog_open_file` | `func b32 dialog_open_file( window owner, const dialog_file_filter* filters, sz filter_count, cstr8 default_location, b32 allow_many, dialog_file_callback* callback, void* user_data);` |
| `dialog_save_file` | `func b32 dialog_save_file( window owner, const dialog_file_filter* filters, sz filter_count, cstr8 default_location, dialog_file_callback* callback, void* user_data);` |
| `dialog_open_folder` | `func b32 dialog_open_folder( window owner, cstr8 default_location, b32 allow_many, dialog_file_callback* callback, void* user_data);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `interface/dpi.h`

- Types: **1**
- Functions: **5**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `dpi_metrics` | `typedef struct dpi_metrics { ... } dpi_metrics;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `dpi_metrics_make_default` | `func dpi_metrics dpi_metrics_make_default(void);` |
| `dpi_get_monitor_metrics` | `func b32 dpi_get_monitor_metrics(monitor monitor_id, dpi_metrics* out_metrics);` |
| `dpi_get_window_metrics` | `func b32 dpi_get_window_metrics(window window_id, dpi_metrics* out_metrics);` |
| `dpi_scale_f32` | `func f32 dpi_scale_f32(f32 value, f32 scale);` |
| `dpi_scale_i32` | `func i32 dpi_scale_i32(i32 value, f32 scale);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `interface/icon.h`

- Types: **2**
- Functions: **9**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `icon` | `typedef void* icon;` |
| `icon_system` | `typedef enum icon_system { ... } icon_system;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `icon_id_is_valid` | `func b32 icon_id_is_valid(icon src);` |
| `icon_create_rgba` | `func icon icon_create_rgba(i32 width, i32 height, const u8* rgba_pixels, i32 hot_xpos, i32 hot_ypos);` |
| `icon_create_system` | `func icon icon_create_system(icon_system system_icon);` |
| `icon_destroy` | `func b32 icon_destroy(icon icon_id);` |
| `icon_get_size` | `func b32 icon_get_size(icon icon_id, i32* out_width, i32* out_height);` |
| `icon_get_hotspot` | `func b32 icon_get_hotspot(icon icon_id, i32* out_xpos, i32* out_ypos);` |
| `icon_get_system` | `func b32 icon_get_system(icon icon_id, icon_system* out_system_icon);` |
| `icon_is_rgba` | `func b32 icon_is_rgba(icon icon_id);` |
| `icon_is_system` | `func b32 icon_is_system(icon icon_id);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `interface/monitor.h`

- Types: **3**
- Functions: **15**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `monitor` | `typedef void* monitor;` |
| `monitor_mode` | `typedef struct monitor_mode { ... } monitor_mode;` |
| `monitor_orientation` | `typedef enum monitor_orientation { ... } monitor_orientation;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `monitor_id_is_valid` | `func b32 monitor_id_is_valid(monitor src);` |
| `monitor_from_device` | `func monitor monitor_from_device(device src);` |
| `monitor_get_total_count` | `func sz monitor_get_total_count(void);` |
| `monitor_get_from_idx` | `func monitor monitor_get_from_idx(sz idx);` |
| `monitor_get_primary_id` | `func monitor monitor_get_primary_id(void);` |
| `monitor_get_bounds` | `func r2_i32 monitor_get_bounds(monitor mon_id);` |
| `monitor_get_usable_bounds` | `func r2_i32 monitor_get_usable_bounds(monitor mon_id);` |
| `monitor_get_mode_count` | `func sz monitor_get_mode_count(monitor mon_id);` |
| `monitor_get_mode` | `func b32 monitor_get_mode(monitor mon_id, sz idx, monitor_mode* out_mode);` |
| `monitor_get_current_mode` | `func b32 monitor_get_current_mode(monitor mon_id, monitor_mode* out_mode);` |
| `monitor_get_desktop_mode` | `func b32 monitor_get_desktop_mode(monitor mon_id, monitor_mode* out_mode);` |
| `monitor_get_name` | `func cstr8 monitor_get_name(monitor mon_id);` |
| `monitor_get_refresh_rate` | `func f32 monitor_get_refresh_rate(monitor mon_id);` |
| `monitor_get_content_scale` | `func f32 monitor_get_content_scale(monitor mon_id);` |
| `monitor_get_orientation` | `func monitor_orientation monitor_get_orientation(monitor mon_id);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `interface/text_input.h`

- Types: **0**
- Functions: **4**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `text_input_start` | `func b32 text_input_start(window opt_window);` |
| `text_input_stop` | `func b32 text_input_stop(window opt_window);` |
| `text_input_is_active` | `func b32 text_input_is_active(window opt_window);` |
| `text_input_set_area` | `func b32 text_input_set_area(window opt_window, i32 xpos, i32 ypos, i32 width, i32 height);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `interface/window.h`

- Types: **3**
- Functions: **33**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `window` | `typedef void* window;` |
| `window_mode` | `typedef enum window_mode { ... } window_mode;` |
| `window_center_axis` | `typedef enum window_center_axis { ... } window_center_axis;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `window_id_is_valid` | `func b32 window_id_is_valid(window src);` |
| `window_get_total_count` | `func sz window_get_total_count(void);` |
| `window_get_from_idx` | `func window window_get_from_idx(sz idx);` |
| `window_is_valid` | `func b32 window_is_valid(window window_id);` |
| `window_create` | `func window window_create(cstr8 title, i32 width, i32 height, window_mode mode, u64 flags);` |
| `window_destroy` | `func b32 window_destroy(window window_id);` |
| `window_is_hidden` | `func b32 window_is_hidden(window window_id);` |
| `window_show` | `func b32 window_show(window window_id);` |
| `window_hide` | `func b32 window_hide(window window_id);` |
| `window_set_pos` | `func b32 window_set_pos(window window_id, i32 xpos, i32 ypos);` |
| `window_get_pos` | `func b32 window_get_pos(window window_id, i32* out_xpos, i32* out_ypos);` |
| `window_set_size` | `func b32 window_set_size(window window_id, i32 width, i32 height);` |
| `window_get_size` | `func b32 window_get_size(window window_id, i32* out_width, i32* out_height);` |
| `window_set_mode` | `func b32 window_set_mode(window window_id, window_mode mode);` |
| `window_get_mode` | `func window_mode window_get_mode(window window_id);` |
| `window_is_minimized` | `func b32 window_is_minimized(window window_id);` |
| `window_is_maximized` | `func b32 window_is_maximized(window window_id);` |
| `window_has_input_focus` | `func b32 window_has_input_focus(window window_id);` |
| `window_has_mouse_focus` | `func b32 window_has_mouse_focus(window window_id);` |
| `window_is_topmost` | `func b32 window_is_topmost(window window_id);` |
| `window_minimize` | `func b32 window_minimize(window window_id);` |
| `window_maximize` | `func b32 window_maximize(window window_id);` |
| `window_restore` | `func b32 window_restore(window window_id);` |
| `window_focus` | `func b32 window_focus(window window_id);` |
| `window_set_topmost` | `func b32 window_set_topmost(window window_id, b32 enabled);` |
| `window_get_title` | `func cstr8 window_get_title(window window_id);` |
| `window_set_title` | `func b32 window_set_title(window window_id, cstr8 title);` |
| `window_set_icon` | `func b32 window_set_icon(window window_id, icon icon_id);` |
| `window_get_monitor` | `func b32 window_get_monitor(window window_id, monitor* out_monitor_id);` |
| `window_get_creation_monitor` | `func b32 window_get_creation_monitor(window window_id, monitor* out_monitor_id);` |
| `window_center_in_monitor` | `func b32 window_center_in_monitor( window window_id, monitor monitor_id, window_center_axis axis_mask);` |
| `window_get_keyboard_focus` | `func window window_get_keyboard_focus();` |
| `window_get_cursor_focus` | `func window window_get_cursor_focus();` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

## memory

Allocators, arenas, heaps, pools, rings, scratch buffers, and memory helpers.

### `memory/alloc_tracker.h`

- Types: **5**
- Functions: **12**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `alloc_tracker_stats` | `typedef struct alloc_tracker_stats { ... } alloc_tracker_stats;` |
| `alloc_tracker_callback_alloc` | `typedef void alloc_tracker_callback_alloc(void* user_data, callsite site, void* ptr, sz size);` |
| `alloc_tracker_callback_free` | `typedef void alloc_tracker_callback_free(void* user_data, callsite site, void* ptr, sz size);` |
| `alloc_tracker_callback_realloc` | `typedef void alloc_tracker_callback_realloc(void* user_data, callsite site, void* old_ptr, void* new_ptr, sz old_size, sz new_size);` |
| `alloc_tracker` | `typedef struct alloc_tracker { ... } alloc_tracker;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `alloc_tracker_create` | `func alloc_tracker alloc_tracker_create(void);` |
| `alloc_tracker_get_user_data` | `func void* alloc_tracker_get_user_data(alloc_tracker* tracker);` |
| `alloc_tracker_set_user_data` | `func void alloc_tracker_set_user_data(alloc_tracker* tracker, void* user_data);` |
| `alloc_tracker_reset` | `func void alloc_tracker_reset(alloc_tracker* tracker);` |
| `alloc_tracker_get_stats` | `func alloc_tracker_stats alloc_tracker_get_stats(alloc_tracker* tracker);` |
| `alloc_tracker_on_alloc_call` | `func void alloc_tracker_on_alloc_call(alloc_tracker* tracker);` |
| `alloc_tracker_on_calloc_call` | `func void alloc_tracker_on_calloc_call(alloc_tracker* tracker);` |
| `alloc_tracker_on_realloc_call` | `func void alloc_tracker_on_realloc_call(alloc_tracker* tracker);` |
| `alloc_tracker_on_free_call` | `func void alloc_tracker_on_free_call(alloc_tracker* tracker);` |
| `alloc_tracker_on_alloc_success` | `func void alloc_tracker_on_alloc_success(alloc_tracker* tracker, callsite site, void* ptr, sz size);` |
| `alloc_tracker_on_free_success` | `func void alloc_tracker_on_free_success(alloc_tracker* tracker, callsite site, void* ptr, sz size);` |
| `alloc_tracker_on_realloc_success` | `func void alloc_tracker_on_realloc_success( alloc_tracker* tracker, callsite site, void* old_ptr, void* new_ptr, sz old_size, sz new_size);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `memory/allocator.h`

- Types: **4**
- Functions: **8**
- Macros: **4**

#### Types

| Type | Declaration |
| --- | --- |
| `allocator_callback_realloc` | `typedef void* allocator_callback_realloc(void* user_data, callsite site, void* ptr, sz new_size);` |
| `allocator_callback_alloc` | `typedef void* allocator_callback_alloc(void* user_data, callsite site, sz size);` |
| `allocator_callback_free` | `typedef void allocator_callback_free(void* user_data, callsite site, void* ptr);` |
| `allocator` | `typedef struct allocator { ... } allocator;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `allocator_get_user_data` | `func void* allocator_get_user_data(allocator* alloc);` |
| `allocator_set_user_data` | `func void allocator_set_user_data(allocator* alloc, void* user_data);` |
| `allocator_get_tracker` | `func alloc_tracker* allocator_get_tracker(allocator* alloc);` |
| `allocator_set_tracker` | `func void allocator_set_tracker(allocator* alloc, alloc_tracker* tracker);` |
| `_allocator_alloc` | `func void* _allocator_alloc(allocator alloc, sz size, callsite site);` |
| `_allocator_calloc` | `func void* _allocator_calloc(allocator alloc, sz count, sz size, callsite site);` |
| `_allocator_dealloc` | `func void _allocator_dealloc(allocator alloc, void* ptr, callsite site);` |
| `_allocator_realloc` | `func void* _allocator_realloc(allocator alloc, void* ptr, sz new_size, callsite site);` |

#### Macros

| Macro | Form |
| --- | --- |
| `allocator_alloc` | `allocator_alloc(alloc, size)` |
| `allocator_dealloc` | `allocator_dealloc(alloc, ptr)` |
| `allocator_calloc` | `allocator_calloc(alloc, count, size)` |
| `allocator_realloc` | `allocator_realloc(alloc, ptr, new_size)` |

### `memory/arena.h`

- Types: **2**
- Functions: **13**
- Macros: **8**

#### Types

| Type | Declaration |
| --- | --- |
| `arena_block` | `typedef struct arena_block { ... } arena_block;` |
| `arena` | `typedef struct arena { ... } arena;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_arena_create` | `func arena _arena_create( allocator parent_alloc, mutex opt_mutex, sz default_block_sz, callsite site);` |
| `_arena_create_mutexed` | `func arena _arena_create_mutexed(allocator parent_alloc, sz default_block_sz, callsite site);` |
| `_arena_destroy` | `func void _arena_destroy(arena* arn, callsite site);` |
| `arena_get_allocator` | `func allocator arena_get_allocator(arena* arn);` |
| `arena_add_block` | `func void arena_add_block(arena* arn, void* ptr, sz size);` |
| `arena_remove_block` | `func b32 arena_remove_block(arena* arn, void* ptr);` |
| `_arena_alloc` | `func void* _arena_alloc(arena* arn, sz size, sz align, callsite site);` |
| `_arena_realloc` | `func void* _arena_realloc( arena* arn, void* ptr, sz old_size, sz new_size, sz align, callsite site);` |
| `arena_clear` | `func void arena_clear(arena* arn);` |
| `arena_block_count` | `func sz arena_block_count(arena* arn);` |
| `arena_total_size` | `func sz arena_total_size(arena* arn);` |
| `arena_total_used` | `func sz arena_total_used(arena* arn);` |
| `arena_total_free` | `func sz arena_total_free(arena* arn);` |

#### Macros

| Macro | Form |
| --- | --- |
| `arena_create` | `arena_create(parent_alloc, opt_mutex, default_block_sz)` |
| `arena_create_mutexed` | `arena_create_mutexed(parent_alloc, default_block_sz)` |
| `arena_destroy` | `arena_destroy(arn)` |
| `arena_alloc` | `arena_alloc(arn, size, align)` |
| `arena_realloc` | `arena_realloc(arn, ptr, old_size, new_size, align)` |
| `arena_alloc_type` | `arena_alloc_type(arn, type)` |
| `arena_alloc_array` | `arena_alloc_array(arn, type, count)` |
| `arena_realloc_array` | `arena_realloc_array(arn, ptr, old_count, new_count, type)` |

### `memory/buffer.h`

- Types: **1**
- Functions: **15**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `buffer` | `typedef struct buffer { ... } buffer;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `buffer_from` | `func buffer buffer_from(void* ptr, sz size);` |
| `buffer_sub_from` | `func buffer buffer_sub_from(buffer buff, sz offset);` |
| `buffer_sub_from_sized` | `func buffer buffer_sub_from_sized(buffer buff, sz offset, sz size);` |
| `buffer_slice` | `func buffer buffer_slice(buffer buff, sz start, sz end);` |
| `buffer_split_offset` | `func buffer buffer_split_offset(buffer* buff, sz offset);` |
| `buffer_split_size` | `func buffer buffer_split_size(buffer* buff, sz size);` |
| `buffer_cmp` | `func b32 buffer_cmp(buffer a, buffer b);` |
| `buffer_cmp_common` | `func b32 buffer_cmp_common(buffer a, buffer b);` |
| `buffer_get_ptr` | `func void* buffer_get_ptr(buffer buff, sz offset);` |
| `buffer_get_data` | `func void* buffer_get_data(buffer buff, sz offset, sz read_size);` |
| `buffer_set8` | `func void buffer_set8(buffer buff, u8 value);` |
| `buffer_set16` | `func void buffer_set16(buffer buff, u16 value);` |
| `buffer_set32` | `func void buffer_set32(buffer buff, u32 value);` |
| `buffer_set64` | `func void buffer_set64(buffer buff, u64 value);` |
| `buffer_zero` | `func void buffer_zero(buffer buff);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `memory/heap.h`

- Types: **3**
- Functions: **13**
- Macros: **9**

#### Types

| Type | Declaration |
| --- | --- |
| `heap_chunk` | `typedef struct heap_chunk { ... } heap_chunk;` |
| `heap_block` | `typedef struct heap_block { ... } heap_block;` |
| `heap` | `typedef struct heap { ... } heap;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_heap_create` | `func heap _heap_create(allocator parent_alloc, mutex opt_mutex, sz default_block_sz, callsite site);` |
| `_heap_create_mutexed` | `func heap _heap_create_mutexed(allocator parent_alloc, sz default_block_sz, callsite site);` |
| `_heap_destroy` | `func void _heap_destroy(heap* hep, callsite site);` |
| `heap_get_allocator` | `func allocator heap_get_allocator(heap* hep);` |
| `heap_add_block` | `func void heap_add_block(heap* hep, void* ptr, sz size);` |
| `heap_remove_block` | `func b32 heap_remove_block(heap* hep, void* ptr);` |
| `_heap_alloc` | `func void* _heap_alloc(heap* hep, sz size, sz align, callsite site);` |
| `_heap_dealloc` | `func void _heap_dealloc(heap* hep, void* ptr, callsite site);` |
| `_heap_realloc` | `func void* _heap_realloc( heap* hep, void* ptr, sz old_size, sz new_size, sz align, callsite site);` |
| `heap_clear` | `func void heap_clear(heap* hep);` |
| `heap_block_count` | `func sz heap_block_count(heap* hep);` |
| `heap_total_size` | `func sz heap_total_size(heap* hep);` |
| `heap_total_free` | `func sz heap_total_free(heap* hep);` |

#### Macros

| Macro | Form |
| --- | --- |
| `heap_create` | `heap_create(parent_alloc, opt_mutex, default_block_sz)` |
| `heap_create_mutexed` | `heap_create_mutexed(parent_alloc, default_block_sz)` |
| `heap_destroy` | `heap_destroy(hep)` |
| `heap_alloc` | `heap_alloc(hp, size, align)` |
| `heap_dealloc` | `heap_dealloc(hp, ptr)` |
| `heap_realloc` | `heap_realloc(hp, ptr, old_size, new_size, align)` |
| `heap_alloc_type` | `heap_alloc_type(hp, type)` |
| `heap_alloc_array` | `heap_alloc_array(hp, type, count)` |
| `heap_realloc_array` | `heap_realloc_array(hp, ptr, old_count, new_count, type)` |

### `memory/memops.h`

- Types: **0**
- Functions: **12**
- Macros: **11**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `mem_set8` | `func void mem_set8(void* ptr, u8 value, sz size);` |
| `mem_set16` | `func void mem_set16(void* ptr, u16 value, sz count);` |
| `mem_set32` | `func void mem_set32(void* ptr, u32 value, sz count);` |
| `mem_set64` | `func void mem_set64(void* ptr, u64 value, sz count);` |
| `mem_zero` | `func void mem_zero(void* ptr, sz size);` |
| `mem_cpy` | `func void* mem_cpy(void* dst, const void* src, sz size);` |
| `mem_mv` | `func void* mem_mv(void* dst, const void* src, sz size);` |
| `mem_cmp` | `func b32 mem_cmp(const void* lhs, const void* rhs, sz size);` |
| `mem_align_forward` | `func void* mem_align_forward(void* ptr, sz align);` |
| `mem_align_backward` | `func void* mem_align_backward(void* ptr, sz align);` |
| `mem_align_forward_up` | `func up mem_align_forward_up(up ptr, sz align);` |
| `mem_align_backward_up` | `func up mem_align_backward_up(up ptr, sz align);` |

#### Macros

| Macro | Form |
| --- | --- |
| `mem_set8_type` | `mem_set8_type(ptr, value, type)` |
| `mem_set8_array` | `mem_set8_array(ptr, value, count, type)` |
| `mem_set16_type` | `mem_set16_type(ptr, value)` |
| `mem_set16_array` | `mem_set16_array(ptr, value, count)` |
| `mem_set32_type` | `mem_set32_type(ptr, value)` |
| `mem_set32_array` | `mem_set32_array(ptr, value, count)` |
| `mem_set64_type` | `mem_set64_type(ptr, value)` |
| `mem_set64_array` | `mem_set64_array(ptr, value, count)` |
| `mem_zero_value` | `mem_zero_value(ptr)` |
| `mem_zero_type` | `mem_zero_type(ptr, type)` |
| `mem_zero_array` | `mem_zero_array(ptr, type, count)` |

### `memory/pool.h`

- Types: **2**
- Functions: **12**
- Macros: **7**

#### Types

| Type | Declaration |
| --- | --- |
| `pool_block` | `typedef struct pool_block { ... } pool_block;` |
| `pool` | `typedef struct pool { ... } pool;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_pool_create` | `func pool _pool_create( allocator parent_alloc, mutex opt_mutex, sz default_block_sz, sz object_size, sz object_align, callsite site);` |
| `_pool_create_mutexed` | `func pool _pool_create_mutexed( allocator parent_alloc, sz default_block_sz, sz object_size, sz object_align, callsite site);` |
| `_pool_destroy` | `func void _pool_destroy(pool* pol, callsite site);` |
| `pool_get_allocator` | `func allocator pool_get_allocator(pool* pol);` |
| `pool_add_block` | `func void pool_add_block(pool* pol, void* ptr, sz size);` |
| `pool_remove_block` | `func b32 pool_remove_block(pool* pol, void* ptr);` |
| `_pool_alloc` | `func void* _pool_alloc(pool* pol, callsite site);` |
| `_pool_dealloc` | `func void _pool_dealloc(pool* pol, void* ptr, callsite site);` |
| `pool_clear` | `func void pool_clear(pool* pol);` |
| `pool_block_count` | `func sz pool_block_count(pool* pol);` |
| `pool_slot_size` | `func sz pool_slot_size(pool* pol);` |
| `pool_free_count` | `func sz pool_free_count(pool* pol);` |

#### Macros

| Macro | Form |
| --- | --- |
| `pool_create` | `pool_create(parent_alloc, opt_mutex, default_block_sz, object_size, object_align)` |
| `pool_create_mutexed` | `pool_create_mutexed(parent_alloc, default_block_sz, object_size, object_align)` |
| `pool_destroy` | `pool_destroy(pol)` |
| `pool_alloc` | `pool_alloc(pol)` |
| `pool_dealloc` | `pool_dealloc(pol, ptr)` |
| `pool_alloc_type` | `pool_alloc_type(pol, type)` |
| `pool_dealloc_type` | `pool_dealloc_type(pol, ptr)` |

### `memory/ring.h`

- Types: **1**
- Functions: **17**
- Macros: **5**

#### Types

| Type | Declaration |
| --- | --- |
| `ring` | `typedef struct ring { ... } ring;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_ring_create` | `func ring _ring_create(void* ptr, sz capacity, mutex opt_mutex, callsite site);` |
| `_ring_create_mutexed` | `func ring _ring_create_mutexed(void* ptr, sz capacity, callsite site);` |
| `_ring_create_alloc` | `func ring _ring_create_alloc(allocator parent_alloc, sz capacity, mutex opt_mutex, callsite site);` |
| `_ring_create_alloc_mutexed` | `func ring _ring_create_alloc_mutexed(allocator parent_alloc, sz capacity, callsite site);` |
| `_ring_destroy` | `func void _ring_destroy(ring* rng, callsite site);` |
| `ring_size` | `func sz ring_size(ring* rng);` |
| `ring_space` | `func sz ring_space(ring* rng);` |
| `ring_capacity` | `func sz ring_capacity(ring* rng);` |
| `ring_write` | `func sz ring_write(ring* rng, void* data, sz size);` |
| `ring_read` | `func sz ring_read(ring* rng, void* out, sz size);` |
| `ring_peek` | `func sz ring_peek(ring* rng, void* out, sz size);` |
| `ring_skip` | `func sz ring_skip(ring* rng, sz size);` |
| `ring_reserve_write` | `func void* ring_reserve_write(ring* rng, sz* out_size);` |
| `ring_commit_write` | `func b32 ring_commit_write(ring* rng, sz size);` |
| `ring_reserve_read` | `func const void* ring_reserve_read(ring* rng, sz* out_size);` |
| `ring_commit_read` | `func b32 ring_commit_read(ring* rng, sz size);` |
| `ring_clear` | `func void ring_clear(ring* rng);` |

#### Macros

| Macro | Form |
| --- | --- |
| `ring_create` | `ring_create(ptr, capacity, opt_mutex)` |
| `ring_create_mutexed` | `ring_create_mutexed(ptr, capacity)` |
| `ring_create_alloc` | `ring_create_alloc(parent_alloc, capacity, opt_mutex)` |
| `ring_create_alloc_mutexed` | `ring_create_alloc_mutexed(parent_alloc, capacity)` |
| `ring_destroy` | `ring_destroy(rng)` |

### `memory/scratch.h`

- Types: **1**
- Functions: **2**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `scratch` | `typedef struct scratch { ... } scratch;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `scratch_begin` | `func scratch scratch_begin(arena* arn);` |
| `scratch_end` | `func void scratch_end(scratch* scr);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `memory/vmem.h`

- Types: **1**
- Functions: **11**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `vmem_stats` | `typedef struct vmem_stats { ... } vmem_stats;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `vmem_page_size` | `func sz vmem_page_size(void);` |
| `vmem_reserve` | `func void* vmem_reserve(sz size);` |
| `vmem_commit` | `func b32 vmem_commit(void* ptr, sz size);` |
| `vmem_decommit` | `func b32 vmem_decommit(void* ptr, sz size);` |
| `vmem_release` | `func b32 vmem_release(void* ptr, sz size);` |
| `vmem_alloc` | `func void* vmem_alloc(sz size);` |
| `vmem_calloc` | `func void* vmem_calloc(sz count, sz size);` |
| `vmem_realloc` | `func void* vmem_realloc(void* ptr, sz old_size, sz new_size);` |
| `vmem_free` | `func b32 vmem_free(void* ptr, sz size);` |
| `vmem_get_allocator` | `func allocator vmem_get_allocator(void);` |
| `vmem_get_stats` | `func vmem_stats vmem_get_stats(void);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

## processes

Process launching, pipes, and current-process utilities.

### `processes/common_processes.h`

- Types: **0**
- Functions: **3**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `process_open_weblink` | `func b32 process_open_weblink(cstr8 url);` |
| `process_open_file_window` | `func b32 process_open_file_window(cstr8 location);` |
| `process_open_terminal` | `func b32 process_open_terminal(cstr8 location);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `processes/process.h`

- Types: **3**
- Functions: **14**
- Macros: **4**

#### Types

| Type | Declaration |
| --- | --- |
| `process` | `typedef void* process;` |
| `process_snapshot_entry` | `typedef struct process_snapshot_entry { ... } process_snapshot_entry;` |
| `process_options` | `typedef struct process_options { ... } process_options;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `process_options_default` | `func process_options process_options_default(void);` |
| `process_options_captured` | `func process_options process_options_captured(void);` |
| `_process_create` | `func process _process_create(cstr8 const* args, callsite site);` |
| `_process_create_with` | `func process _process_create_with(cstr8 const* args, process_options options, callsite site);` |
| `process_is_valid` | `func b32 process_is_valid(process prc);` |
| `process_get_id` | `func u64 process_get_id(process prc);` |
| `process_snapshot_get` | `func process_snapshot_entry* process_snapshot_get(sz* out_count);` |
| `process_snapshot_free` | `func void process_snapshot_free(process_snapshot_entry* ptr);` |
| `process_read` | `func void* process_read(process prc, sz* out_size, i32* out_exit_code);` |
| `process_read_free` | `func void process_read_free(void* ptr);` |
| `process_wait` | `func b32 process_wait(process prc, b32 block, i32* out_exit_code);` |
| `process_wait_timeout` | `func b32 process_wait_timeout(process prc, i32 timeout_ms, i32* out_exit_code);` |
| `process_kill` | `func b32 process_kill(process prc, b32 force);` |
| `_process_destroy` | `func void _process_destroy(process prc, callsite site);` |

#### Macros

| Macro | Form |
| --- | --- |
| `PROCESS_NAME_CAP` | `PROCESS_NAME_CAP` |
| `process_create` | `process_create(args)` |
| `process_create_with` | `process_create_with(args, options)` |
| `process_destroy` | `process_destroy(prc)` |

### `processes/process_current.h`

- Types: **1**
- Functions: **7**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `process_priority` | `typedef enum process_priority { ... } process_priority;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `process_id` | `func u64 process_id(void);` |
| `process_get_priority` | `func process_priority process_get_priority(void);` |
| `process_set_priority` | `func b32 process_set_priority(process_priority priority);` |
| `process_is_unique` | `func b32 process_is_unique(void);` |
| `process_restart` | `func b32 process_restart(void);` |
| `process_exit` | `func no_return void process_exit(i32 exit_code);` |
| `process_abort` | `func no_return void process_abort(void);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `processes/process_pipe.h`

- Types: **1**
- Functions: **11**
- Macros: **4**

#### Types

| Type | Declaration |
| --- | --- |
| `process_pipe` | `typedef void* process_pipe;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_process_pipe_stdin` | `func process_pipe _process_pipe_stdin(process prc, callsite site);` |
| `_process_pipe_stdout` | `func process_pipe _process_pipe_stdout(process prc, callsite site);` |
| `_process_pipe_stderr` | `func process_pipe _process_pipe_stderr(process prc, callsite site);` |
| `process_pipe_is_valid` | `func b32 process_pipe_is_valid(process_pipe pip);` |
| `process_pipe_read` | `func sz process_pipe_read(process_pipe pip, void* ptr, sz size);` |
| `process_pipe_write` | `func sz process_pipe_write(process_pipe pip, const void* ptr, sz size);` |
| `process_pipe_read_nonblocking` | `func sz process_pipe_read_nonblocking(process_pipe pip, void* ptr, sz size);` |
| `process_pipe_write_nonblocking` | `func sz process_pipe_write_nonblocking(process_pipe pip, const void* ptr, sz size);` |
| `process_pipe_poll_readable` | `func b32 process_pipe_poll_readable(process_pipe pip, i32 timeout_ms);` |
| `process_pipe_flush` | `func b32 process_pipe_flush(process_pipe pip);` |
| `_process_pipe_close` | `func void _process_pipe_close(process_pipe pip, callsite site);` |

#### Macros

| Macro | Form |
| --- | --- |
| `process_pipe_stdin` | `process_pipe_stdin(prc)` |
| `process_pipe_stdout` | `process_pipe_stdout(prc)` |
| `process_pipe_stderr` | `process_pipe_stderr(prc)` |
| `process_pipe_close` | `process_pipe_close(pip)` |

## strings

Character, string, C-string, and Unicode conversion helpers.

### `strings/char.h`

- Types: **0**
- Functions: **48**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `c8_is_alnum` | `func b32 c8_is_alnum(c8 chr);` |
| `c8_is_alpha` | `func b32 c8_is_alpha(c8 chr);` |
| `c8_is_blank` | `func b32 c8_is_blank(c8 chr);` |
| `c8_is_cntrl` | `func b32 c8_is_cntrl(c8 chr);` |
| `c8_is_digit` | `func b32 c8_is_digit(c8 chr);` |
| `c8_is_graph` | `func b32 c8_is_graph(c8 chr);` |
| `c8_is_lower` | `func b32 c8_is_lower(c8 chr);` |
| `c8_is_print` | `func b32 c8_is_print(c8 chr);` |
| `c8_is_punct` | `func b32 c8_is_punct(c8 chr);` |
| `c8_is_space` | `func b32 c8_is_space(c8 chr);` |
| `c8_is_upper` | `func b32 c8_is_upper(c8 chr);` |
| `c8_is_xdigit` | `func b32 c8_is_xdigit(c8 chr);` |
| `c8_to_lower` | `func c8 c8_to_lower(c8 chr);` |
| `c8_to_upper` | `func c8 c8_to_upper(c8 chr);` |
| `c8_hex_to_nibble` | `func i32 c8_hex_to_nibble(c8 chr);` |
| `c8_nibble_to_hex` | `func c8 c8_nibble_to_hex(u8 nibble);` |
| `c16_is_alnum` | `func b32 c16_is_alnum(c16 chr);` |
| `c16_is_alpha` | `func b32 c16_is_alpha(c16 chr);` |
| `c16_is_blank` | `func b32 c16_is_blank(c16 chr);` |
| `c16_is_cntrl` | `func b32 c16_is_cntrl(c16 chr);` |
| `c16_is_digit` | `func b32 c16_is_digit(c16 chr);` |
| `c16_is_graph` | `func b32 c16_is_graph(c16 chr);` |
| `c16_is_lower` | `func b32 c16_is_lower(c16 chr);` |
| `c16_is_print` | `func b32 c16_is_print(c16 chr);` |
| `c16_is_punct` | `func b32 c16_is_punct(c16 chr);` |
| `c16_is_space` | `func b32 c16_is_space(c16 chr);` |
| `c16_is_upper` | `func b32 c16_is_upper(c16 chr);` |
| `c16_is_xdigit` | `func b32 c16_is_xdigit(c16 chr);` |
| `c16_to_lower` | `func c16 c16_to_lower(c16 chr);` |
| `c16_to_upper` | `func c16 c16_to_upper(c16 chr);` |
| `c16_hex_to_nibble` | `func i32 c16_hex_to_nibble(c16 chr);` |
| `c16_nibble_to_hex` | `func c16 c16_nibble_to_hex(u8 nibble);` |
| `c32_is_alnum` | `func b32 c32_is_alnum(c32 chr);` |
| `c32_is_alpha` | `func b32 c32_is_alpha(c32 chr);` |
| `c32_is_blank` | `func b32 c32_is_blank(c32 chr);` |
| `c32_is_cntrl` | `func b32 c32_is_cntrl(c32 chr);` |
| `c32_is_digit` | `func b32 c32_is_digit(c32 chr);` |
| `c32_is_graph` | `func b32 c32_is_graph(c32 chr);` |
| `c32_is_lower` | `func b32 c32_is_lower(c32 chr);` |
| `c32_is_print` | `func b32 c32_is_print(c32 chr);` |
| `c32_is_punct` | `func b32 c32_is_punct(c32 chr);` |
| `c32_is_space` | `func b32 c32_is_space(c32 chr);` |
| `c32_is_upper` | `func b32 c32_is_upper(c32 chr);` |
| `c32_is_xdigit` | `func b32 c32_is_xdigit(c32 chr);` |
| `c32_to_lower` | `func c32 c32_to_lower(c32 chr);` |
| `c32_to_upper` | `func c32 c32_to_upper(c32 chr);` |
| `c32_hex_to_nibble` | `func i32 c32_hex_to_nibble(c32 chr);` |
| `c32_nibble_to_hex` | `func c32 c32_nibble_to_hex(u8 nibble);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `strings/cstrings.h`

- Types: **15**
- Functions: **111**
- Macros: **5**

#### Types

| Type | Declaration |
| --- | --- |
| `str8_tiny` | `typedef c8 str8_tiny[STR_CAP_TINY];` |
| `str8_short` | `typedef c8 str8_short[STR_CAP_SHORT];` |
| `str8_medium` | `typedef c8 str8_medium[STR_CAP_MEDIUM];` |
| `str8_long` | `typedef c8 str8_long[STR_CAP_LONG];` |
| `str8_large` | `typedef c8 str8_large[STR_CAP_LARGE];` |
| `str16_tiny` | `typedef c16 str16_tiny[STR_CAP_TINY];` |
| `str16_short` | `typedef c16 str16_short[STR_CAP_SHORT];` |
| `str16_medium` | `typedef c16 str16_medium[STR_CAP_MEDIUM];` |
| `str16_long` | `typedef c16 str16_long[STR_CAP_LONG];` |
| `str16_large` | `typedef c16 str16_large[STR_CAP_LARGE];` |
| `str32_tiny` | `typedef c32 str32_tiny[STR_CAP_TINY];` |
| `str32_short` | `typedef c32 str32_short[STR_CAP_SHORT];` |
| `str32_medium` | `typedef c32 str32_medium[STR_CAP_MEDIUM];` |
| `str32_long` | `typedef c32 str32_long[STR_CAP_LONG];` |
| `str32_large` | `typedef c32 str32_large[STR_CAP_LARGE];` |

#### Functions

| Function | Declaration |
| --- | --- |
| `cstr8_len` | `func sz cstr8_len(cstr8 str);` |
| `cstr8_is_empty` | `func b32 cstr8_is_empty(cstr8 str);` |
| `cstr8_clear` | `func void cstr8_clear(c8* str);` |
| `cstr8_cmp` | `func b32 cstr8_cmp(cstr8 lhs, cstr8 rhs);` |
| `cstr8_cmp_n` | `func b32 cstr8_cmp_n(cstr8 lhs, cstr8 rhs, sz cnt);` |
| `cstr8_cmp_nocase` | `func b32 cstr8_cmp_nocase(cstr8 lhs, cstr8 rhs);` |
| `cstr8_cpy` | `func sz cstr8_cpy(c8* dst, sz dst_size, cstr8 src);` |
| `cstr8_cpy_n` | `func sz cstr8_cpy_n(c8* dst, sz dst_size, cstr8 src, sz cnt);` |
| `cstr8_cat` | `func sz cstr8_cat(c8* dst, sz dst_cap, cstr8 src);` |
| `cstr8_append_char` | `func sz cstr8_append_char(c8* dst, sz dst_cap, c8 chr);` |
| `cstr8_truncate` | `func void cstr8_truncate(c8* str, sz length);` |
| `cstr8_format` | `func b32 cstr8_format(c8* dst, sz dst_cap, cstr8 fmt, ...);` |
| `cstr8_vformat` | `func b32 cstr8_vformat(c8* dst, sz dst_cap, cstr8 fmt, va_list args);` |
| `cstr8_append_format` | `func b32 cstr8_append_format(c8* dst, sz dst_cap, cstr8 fmt, ...);` |
| `cstr8_append_vformat` | `func b32 cstr8_append_vformat(c8* dst, sz dst_cap, cstr8 fmt, va_list args);` |
| `cstr8_scan` | `func b32 cstr8_scan(cstr8 str, cstr8 fmt, ...);` |
| `cstr8_find` | `func cstr8 cstr8_find(cstr8 str, cstr8 sub);` |
| `cstr8_find_last` | `func cstr8 cstr8_find_last(cstr8 str, cstr8 sub);` |
| `cstr8_find_char` | `func cstr8 cstr8_find_char(cstr8 str, c8 chr);` |
| `cstr8_find_last_char` | `func cstr8 cstr8_find_last_char(cstr8 str, c8 chr);` |
| `cstr8_count_char` | `func sz cstr8_count_char(cstr8 str, c8 chr);` |
| `cstr8_starts_with` | `func b32 cstr8_starts_with(cstr8 str, cstr8 prefix);` |
| `cstr8_ends_with` | `func b32 cstr8_ends_with(cstr8 str, cstr8 suffix);` |
| `cstr8_hash32` | `func u32 cstr8_hash32(cstr8 str);` |
| `cstr8_hash64` | `func u64 cstr8_hash64(cstr8 str);` |
| `cstr8_to_upper` | `func void cstr8_to_upper(c8* str);` |
| `cstr8_to_lower` | `func void cstr8_to_lower(c8* str);` |
| `cstr8_trim` | `func void cstr8_trim(c8* str);` |
| `cstr8_replace_char` | `func void cstr8_replace_char(c8* str, c8 from_chr, c8 to_chr);` |
| `cstr8_remove_char` | `func sz cstr8_remove_char(c8* str, c8 chr);` |
| `cstr8_remove_whitespace` | `func sz cstr8_remove_whitespace(c8* str);` |
| `cstr8_remove_prefix` | `func b32 cstr8_remove_prefix(c8* str, cstr8 prefix);` |
| `cstr8_remove_suffix` | `func b32 cstr8_remove_suffix(c8* str, cstr8 suffix);` |
| `cstr8_replace` | `func sz cstr8_replace(c8* str, sz str_cap, cstr8 from, cstr8 rep);` |
| `cstr8_common_prefix` | `func sz cstr8_common_prefix(cstr8 lhs, cstr8 rhs, c8* buf, sz buf_cap);` |
| `cstr8_beautify` | `func void cstr8_beautify(c8* str);` |
| `cstr8_to_i64` | `func b32 cstr8_to_i64(cstr8 str, i64* out);` |
| `cstr8_to_u64` | `func b32 cstr8_to_u64(cstr8 str, u64 max_value, u64* out);` |
| `cstr8_to_f64` | `func b32 cstr8_to_f64(cstr8 str, f64* out);` |
| `cstr16_len` | `func sz cstr16_len(cstr16 str);` |
| `cstr16_is_empty` | `func b32 cstr16_is_empty(cstr16 str);` |
| `cstr16_clear` | `func void cstr16_clear(c16* str);` |
| `cstr16_cmp` | `func b32 cstr16_cmp(cstr16 lhs, cstr16 rhs);` |
| `cstr16_cmp_n` | `func b32 cstr16_cmp_n(cstr16 lhs, cstr16 rhs, sz cnt);` |
| `cstr16_cmp_nocase` | `func b32 cstr16_cmp_nocase(cstr16 lhs, cstr16 rhs);` |
| `cstr16_cpy` | `func sz cstr16_cpy(c16* dst, sz dst_size, cstr16 src);` |
| `cstr16_cpy_n` | `func sz cstr16_cpy_n(c16* dst, sz dst_size, cstr16 src, sz cnt);` |
| `cstr16_cat` | `func sz cstr16_cat(c16* dst, sz dst_cap, cstr16 src);` |
| `cstr16_append_char` | `func sz cstr16_append_char(c16* dst, sz dst_cap, c16 chr);` |
| `cstr16_truncate` | `func void cstr16_truncate(c16* str, sz length);` |
| `cstr16_find` | `func cstr16 cstr16_find(cstr16 str, cstr16 sub);` |
| `cstr16_find_last` | `func cstr16 cstr16_find_last(cstr16 str, cstr16 sub);` |
| `cstr16_find_char` | `func cstr16 cstr16_find_char(cstr16 str, c16 chr);` |
| `cstr16_find_last_char` | `func cstr16 cstr16_find_last_char(cstr16 str, c16 chr);` |
| `cstr16_count_char` | `func sz cstr16_count_char(cstr16 str, c16 chr);` |
| `cstr16_starts_with` | `func b32 cstr16_starts_with(cstr16 str, cstr16 prefix);` |
| `cstr16_ends_with` | `func b32 cstr16_ends_with(cstr16 str, cstr16 suffix);` |
| `cstr16_hash32` | `func u32 cstr16_hash32(cstr16 str);` |
| `cstr16_hash64` | `func u64 cstr16_hash64(cstr16 str);` |
| `cstr16_to_upper` | `func void cstr16_to_upper(c16* str);` |
| `cstr16_to_lower` | `func void cstr16_to_lower(c16* str);` |
| `cstr16_trim` | `func void cstr16_trim(c16* str);` |
| `cstr16_replace_char` | `func void cstr16_replace_char(c16* str, c16 from_chr, c16 to_chr);` |
| `cstr16_remove_char` | `func sz cstr16_remove_char(c16* str, c16 chr);` |
| `cstr16_remove_whitespace` | `func sz cstr16_remove_whitespace(c16* str);` |
| `cstr16_remove_prefix` | `func b32 cstr16_remove_prefix(c16* str, cstr16 prefix);` |
| `cstr16_remove_suffix` | `func b32 cstr16_remove_suffix(c16* str, cstr16 suffix);` |
| `cstr16_replace` | `func sz cstr16_replace(c16* str, sz str_cap, cstr16 from, cstr16 rep);` |
| `cstr16_common_prefix` | `func sz cstr16_common_prefix(cstr16 lhs, cstr16 rhs, c16* buf, sz buf_cap);` |
| `cstr16_beautify` | `func void cstr16_beautify(c16* str);` |
| `cstr16_to_i64` | `func b32 cstr16_to_i64(cstr16 str, i64* out);` |
| `cstr16_to_f64` | `func b32 cstr16_to_f64(cstr16 str, f64* out);` |
| `cstr32_len` | `func sz cstr32_len(cstr32 str);` |
| `cstr32_is_empty` | `func b32 cstr32_is_empty(cstr32 str);` |
| `cstr32_clear` | `func void cstr32_clear(c32* str);` |
| `cstr32_cmp` | `func b32 cstr32_cmp(cstr32 lhs, cstr32 rhs);` |
| `cstr32_cmp_n` | `func b32 cstr32_cmp_n(cstr32 lhs, cstr32 rhs, sz cnt);` |
| `cstr32_cmp_nocase` | `func b32 cstr32_cmp_nocase(cstr32 lhs, cstr32 rhs);` |
| `cstr32_cpy` | `func sz cstr32_cpy(c32* dst, sz dst_size, cstr32 src);` |
| `cstr32_cpy_n` | `func sz cstr32_cpy_n(c32* dst, sz dst_size, cstr32 src, sz cnt);` |
| `cstr32_cat` | `func sz cstr32_cat(c32* dst, sz dst_cap, cstr32 src);` |
| `cstr32_append_char` | `func sz cstr32_append_char(c32* dst, sz dst_cap, c32 chr);` |
| `cstr32_truncate` | `func void cstr32_truncate(c32* str, sz length);` |
| `cstr32_find` | `func cstr32 cstr32_find(cstr32 str, cstr32 sub);` |
| `cstr32_find_last` | `func cstr32 cstr32_find_last(cstr32 str, cstr32 sub);` |
| `cstr32_find_char` | `func cstr32 cstr32_find_char(cstr32 str, c32 chr);` |
| `cstr32_find_last_char` | `func cstr32 cstr32_find_last_char(cstr32 str, c32 chr);` |
| `cstr32_count_char` | `func sz cstr32_count_char(cstr32 str, c32 chr);` |
| `cstr32_starts_with` | `func b32 cstr32_starts_with(cstr32 str, cstr32 prefix);` |
| `cstr32_ends_with` | `func b32 cstr32_ends_with(cstr32 str, cstr32 suffix);` |
| `cstr32_hash32` | `func u32 cstr32_hash32(cstr32 str);` |
| `cstr32_hash64` | `func u64 cstr32_hash64(cstr32 str);` |
| `cstr32_to_upper` | `func void cstr32_to_upper(c32* str);` |
| `cstr32_to_lower` | `func void cstr32_to_lower(c32* str);` |
| `cstr32_trim` | `func void cstr32_trim(c32* str);` |
| `cstr32_replace_char` | `func void cstr32_replace_char(c32* str, c32 from_chr, c32 to_chr);` |
| `cstr32_remove_char` | `func sz cstr32_remove_char(c32* str, c32 chr);` |
| `cstr32_remove_whitespace` | `func sz cstr32_remove_whitespace(c32* str);` |
| `cstr32_remove_prefix` | `func b32 cstr32_remove_prefix(c32* str, cstr32 prefix);` |
| `cstr32_remove_suffix` | `func b32 cstr32_remove_suffix(c32* str, cstr32 suffix);` |
| `cstr32_replace` | `func sz cstr32_replace(c32* str, sz str_cap, cstr32 from, cstr32 rep);` |
| `cstr32_common_prefix` | `func sz cstr32_common_prefix(cstr32 lhs, cstr32 rhs, c32* buf, sz buf_cap);` |
| `cstr32_beautify` | `func void cstr32_beautify(c32* str);` |
| `cstr32_to_i64` | `func b32 cstr32_to_i64(cstr32 str, i64* out);` |
| `cstr32_to_f64` | `func b32 cstr32_to_f64(cstr32 str, f64* out);` |
| `cstr8_to_cstr16` | `func sz cstr8_to_cstr16(cstr8 src, c16* buf, sz buf_cap);` |
| `cstr8_to_cstr32` | `func sz cstr8_to_cstr32(cstr8 src, c32* buf, sz buf_cap);` |
| `cstr16_to_cstr8` | `func sz cstr16_to_cstr8(cstr16 src, c8* buf, sz buf_cap);` |
| `cstr16_to_cstr32` | `func sz cstr16_to_cstr32(cstr16 src, c32* buf, sz buf_cap);` |
| `cstr32_to_cstr8` | `func sz cstr32_to_cstr8(cstr32 src, c8* buf, sz buf_cap);` |
| `cstr32_to_cstr16` | `func sz cstr32_to_cstr16(cstr32 src, c16* buf, sz buf_cap);` |

#### Macros

| Macro | Form |
| --- | --- |
| `STR_CAP_TINY` | `STR_CAP_TINY` |
| `STR_CAP_SHORT` | `STR_CAP_SHORT` |
| `STR_CAP_MEDIUM` | `STR_CAP_MEDIUM` |
| `STR_CAP_LONG` | `STR_CAP_LONG` |
| `STR_CAP_LARGE` | `STR_CAP_LARGE` |

### `strings/strings.h`

- Types: **3**
- Functions: **107**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `str8` | `typedef struct str8 { ... } str8;` |
| `str16` | `typedef struct str16 { ... } str16;` |
| `str32` | `typedef struct str32 { ... } str32;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `str8_make` | `func str8 str8_make(c8* ptr, sz cap);` |
| `str8_empty` | `func str8 str8_empty(c8* ptr, sz cap);` |
| `str8_from_cstr` | `func str8 str8_from_cstr(c8* ptr, sz cap, cstr8 src);` |
| `str8_is_empty` | `func b32 str8_is_empty(str8 str);` |
| `str8_cmp` | `func b32 str8_cmp(str8 lhs, str8 rhs);` |
| `str8_cmp_nocase` | `func b32 str8_cmp_nocase(str8 lhs, str8 rhs);` |
| `str8_find` | `func cstr8 str8_find(str8 str, cstr8 sub);` |
| `str8_find_char` | `func cstr8 str8_find_char(str8 str, c8 chr);` |
| `str8_find_last` | `func cstr8 str8_find_last(str8 str, cstr8 sub);` |
| `str8_find_last_char` | `func cstr8 str8_find_last_char(str8 str, c8 chr);` |
| `str8_count_char` | `func sz str8_count_char(str8 str, c8 chr);` |
| `str8_starts_with` | `func b32 str8_starts_with(str8 str, cstr8 prefix);` |
| `str8_ends_with` | `func b32 str8_ends_with(str8 str, cstr8 suffix);` |
| `str8_hash32` | `func u32 str8_hash32(str8 str);` |
| `str8_hash64` | `func u64 str8_hash64(str8 str);` |
| `str8_to_i64` | `func b32 str8_to_i64(str8 str, i64* out);` |
| `str8_to_u64` | `func b32 str8_to_u64(str8 str, u64 max_value, u64* out);` |
| `str8_to_f64` | `func b32 str8_to_f64(str8 str, f64* out);` |
| `str8_clear` | `func void str8_clear(str8* str);` |
| `str8_cpy` | `func sz str8_cpy(str8* str, cstr8 src);` |
| `str8_cat` | `func sz str8_cat(str8* str, cstr8 src);` |
| `str8_append_char` | `func sz str8_append_char(str8* str, c8 chr);` |
| `str8_truncate` | `func void str8_truncate(str8* str, sz length);` |
| `str8_format` | `func b32 str8_format(str8* str, cstr8 fmt, ...);` |
| `str8_vformat` | `func b32 str8_vformat(str8* str, cstr8 fmt, va_list args);` |
| `str8_append_format` | `func b32 str8_append_format(str8* str, cstr8 fmt, ...);` |
| `str8_append_vformat` | `func b32 str8_append_vformat(str8* str, cstr8 fmt, va_list args);` |
| `str8_to_upper` | `func void str8_to_upper(str8* str);` |
| `str8_to_lower` | `func void str8_to_lower(str8* str);` |
| `str8_trim` | `func void str8_trim(str8* str);` |
| `str8_replace_char` | `func void str8_replace_char(str8* str, c8 from_chr, c8 to_chr);` |
| `str8_remove_char` | `func sz str8_remove_char(str8* str, c8 chr);` |
| `str8_remove_whitespace` | `func sz str8_remove_whitespace(str8* str);` |
| `str8_remove_prefix` | `func b32 str8_remove_prefix(str8* str, cstr8 prefix);` |
| `str8_remove_suffix` | `func b32 str8_remove_suffix(str8* str, cstr8 suffix);` |
| `str8_replace` | `func sz str8_replace(str8* str, cstr8 from, cstr8 rep);` |
| `str8_beautify` | `func void str8_beautify(str8* str);` |
| `str16_make` | `func str16 str16_make(c16* ptr, sz cap);` |
| `str16_empty` | `func str16 str16_empty(c16* ptr, sz cap);` |
| `str16_from_cstr` | `func str16 str16_from_cstr(c16* ptr, sz cap, cstr16 src);` |
| `str16_is_empty` | `func b32 str16_is_empty(str16 str);` |
| `str16_cmp` | `func b32 str16_cmp(str16 lhs, str16 rhs);` |
| `str16_cmp_nocase` | `func b32 str16_cmp_nocase(str16 lhs, str16 rhs);` |
| `str16_find` | `func cstr16 str16_find(str16 str, cstr16 sub);` |
| `str16_find_char` | `func cstr16 str16_find_char(str16 str, c16 chr);` |
| `str16_find_last` | `func cstr16 str16_find_last(str16 str, cstr16 sub);` |
| `str16_find_last_char` | `func cstr16 str16_find_last_char(str16 str, c16 chr);` |
| `str16_count_char` | `func sz str16_count_char(str16 str, c16 chr);` |
| `str16_starts_with` | `func b32 str16_starts_with(str16 str, cstr16 prefix);` |
| `str16_ends_with` | `func b32 str16_ends_with(str16 str, cstr16 suffix);` |
| `str16_hash32` | `func u32 str16_hash32(str16 str);` |
| `str16_hash64` | `func u64 str16_hash64(str16 str);` |
| `str16_to_i64` | `func b32 str16_to_i64(str16 str, i64* out);` |
| `str16_to_f64` | `func b32 str16_to_f64(str16 str, f64* out);` |
| `str16_clear` | `func void str16_clear(str16* str);` |
| `str16_cpy` | `func sz str16_cpy(str16* str, cstr16 src);` |
| `str16_cat` | `func sz str16_cat(str16* str, cstr16 src);` |
| `str16_append_char` | `func sz str16_append_char(str16* str, c16 chr);` |
| `str16_truncate` | `func void str16_truncate(str16* str, sz length);` |
| `str16_to_upper` | `func void str16_to_upper(str16* str);` |
| `str16_to_lower` | `func void str16_to_lower(str16* str);` |
| `str16_trim` | `func void str16_trim(str16* str);` |
| `str16_replace_char` | `func void str16_replace_char(str16* str, c16 from_chr, c16 to_chr);` |
| `str16_remove_char` | `func sz str16_remove_char(str16* str, c16 chr);` |
| `str16_remove_whitespace` | `func sz str16_remove_whitespace(str16* str);` |
| `str16_remove_prefix` | `func b32 str16_remove_prefix(str16* str, cstr16 prefix);` |
| `str16_remove_suffix` | `func b32 str16_remove_suffix(str16* str, cstr16 suffix);` |
| `str16_replace` | `func sz str16_replace(str16* str, cstr16 from, cstr16 rep);` |
| `str16_beautify` | `func void str16_beautify(str16* str);` |
| `str32_make` | `func str32 str32_make(c32* ptr, sz cap);` |
| `str32_empty` | `func str32 str32_empty(c32* ptr, sz cap);` |
| `str32_from_cstr` | `func str32 str32_from_cstr(c32* ptr, sz cap, cstr32 src);` |
| `str32_is_empty` | `func b32 str32_is_empty(str32 str);` |
| `str32_cmp` | `func b32 str32_cmp(str32 lhs, str32 rhs);` |
| `str32_cmp_nocase` | `func b32 str32_cmp_nocase(str32 lhs, str32 rhs);` |
| `str32_find` | `func cstr32 str32_find(str32 str, cstr32 sub);` |
| `str32_find_char` | `func cstr32 str32_find_char(str32 str, c32 chr);` |
| `str32_find_last` | `func cstr32 str32_find_last(str32 str, cstr32 sub);` |
| `str32_find_last_char` | `func cstr32 str32_find_last_char(str32 str, c32 chr);` |
| `str32_count_char` | `func sz str32_count_char(str32 str, c32 chr);` |
| `str32_starts_with` | `func b32 str32_starts_with(str32 str, cstr32 prefix);` |
| `str32_ends_with` | `func b32 str32_ends_with(str32 str, cstr32 suffix);` |
| `str32_hash32` | `func u32 str32_hash32(str32 str);` |
| `str32_hash64` | `func u64 str32_hash64(str32 str);` |
| `str32_to_i64` | `func b32 str32_to_i64(str32 str, i64* out);` |
| `str32_to_f64` | `func b32 str32_to_f64(str32 str, f64* out);` |
| `str32_clear` | `func void str32_clear(str32* str);` |
| `str32_cpy` | `func sz str32_cpy(str32* str, cstr32 src);` |
| `str32_cat` | `func sz str32_cat(str32* str, cstr32 src);` |
| `str32_append_char` | `func sz str32_append_char(str32* str, c32 chr);` |
| `str32_truncate` | `func void str32_truncate(str32* str, sz length);` |
| `str32_to_upper` | `func void str32_to_upper(str32* str);` |
| `str32_to_lower` | `func void str32_to_lower(str32* str);` |
| `str32_trim` | `func void str32_trim(str32* str);` |
| `str32_replace_char` | `func void str32_replace_char(str32* str, c32 from_chr, c32 to_chr);` |
| `str32_remove_char` | `func sz str32_remove_char(str32* str, c32 chr);` |
| `str32_remove_whitespace` | `func sz str32_remove_whitespace(str32* str);` |
| `str32_remove_prefix` | `func b32 str32_remove_prefix(str32* str, cstr32 prefix);` |
| `str32_remove_suffix` | `func b32 str32_remove_suffix(str32* str, cstr32 suffix);` |
| `str32_replace` | `func sz str32_replace(str32* str, cstr32 from, cstr32 rep);` |
| `str32_beautify` | `func void str32_beautify(str32* str);` |
| `str8_to_str16` | `func sz str8_to_str16(str8 src, str16* dst);` |
| `str8_to_str32` | `func sz str8_to_str32(str8 src, str32* dst);` |
| `str16_to_str8` | `func sz str16_to_str8(str16 src, str8* dst);` |
| `str16_to_str32` | `func sz str16_to_str32(str16 src, str32* dst);` |
| `str32_to_str8` | `func sz str32_to_str8(str32 src, str8* dst);` |
| `str32_to_str16` | `func sz str32_to_str16(str32 src, str16* dst);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `strings/unicode.h`

- Types: **0**
- Functions: **16**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `unicode_is_valid` | `func b32 unicode_is_valid(c32 codepoint);` |
| `utf8_encode_size` | `func sz utf8_encode_size(c32 codepoint);` |
| `utf8_byte_count` | `func sz utf8_byte_count(c8 first_byte);` |
| `utf8_decode` | `func c32 utf8_decode(cstr8 ptr, sz* consumed);` |
| `utf8_encode` | `func sz utf8_encode(c32 codepoint, c8* out);` |
| `utf8_codepoint_count` | `func sz utf8_codepoint_count(cstr8 src, sz src_size);` |
| `utf16_encode_size` | `func sz utf16_encode_size(c32 codepoint);` |
| `utf16_decode` | `func c32 utf16_decode(cstr16 ptr, sz* consumed);` |
| `utf16_encode` | `func sz utf16_encode(c32 codepoint, c16* out);` |
| `utf16_codepoint_count` | `func sz utf16_codepoint_count(cstr16 src, sz src_size);` |
| `utf8_to_utf16` | `func sz utf8_to_utf16(cstr8 src, sz src_size, c16* dst, sz dst_cap);` |
| `utf8_to_utf32` | `func sz utf8_to_utf32(cstr8 src, sz src_size, c32* dst, sz dst_cap);` |
| `utf16_to_utf8` | `func sz utf16_to_utf8(cstr16 src, sz src_size, c8* dst, sz dst_cap);` |
| `utf16_to_utf32` | `func sz utf16_to_utf32(cstr16 src, sz src_size, c32* dst, sz dst_cap);` |
| `utf32_to_utf8` | `func sz utf32_to_utf8(cstr32 src, sz src_size, c8* dst, sz dst_cap);` |
| `utf32_to_utf16` | `func sz utf32_to_utf16(cstr32 src, sz src_size, c16* dst, sz dst_cap);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

## system

CPU, hardware, locale, and runtime/system information.

### `system/cpu_info.h`

- Types: **2**
- Functions: **1**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `cpu_instruction_set_info` | `typedef struct cpu_instruction_set_info { ... } cpu_instruction_set_info;` |
| `cpu_info` | `typedef struct cpu_info { ... } cpu_info;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `cpu_info_query` | `func b32 cpu_info_query(cpu_info* out_info);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `system/hardware_info.h`

- Types: **1**
- Functions: **1**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `hardware_info` | `typedef struct hardware_info { ... } hardware_info;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `hardware_info_query` | `func b32 hardware_info_query(hardware_info* out_info);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `system/language.h`

- Types: **1**
- Functions: **1**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `language_info` | `typedef struct language_info { ... } language_info;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `language_query` | `func b32 language_query(language_info* out_info);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `system/system_info.h`

- Types: **1**
- Functions: **1**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `system_info` | `typedef struct system_info { ... } system_info;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `system_info_query` | `func b32 system_info_query(system_info* out_info);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `system/system_runtime.h`

- Types: **1**
- Functions: **1**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `system_runtime_info` | `typedef struct system_runtime_info { ... } system_runtime_info;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `system_runtime_query` | `func b32 system_runtime_query(system_runtime_info* out_info);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

## threads

Threading, atomics, mutexes, semaphores, and synchronization primitives.

### `threads/atomics.h`

- Types: **6**
- Functions: **73**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `atomic_i32` | `typedef struct atomic_i32 { ... } atomic_i32;` |
| `atomic_u32` | `typedef struct atomic_u32 { ... } atomic_u32;` |
| `atomic_i64` | `typedef struct atomic_i64 { ... } atomic_i64;` |
| `atomic_u64` | `typedef struct atomic_u64 { ... } atomic_u64;` |
| `atomic_ptr` | `typedef struct atomic_ptr { ... } atomic_ptr;` |
| `atomic_memory_order` | `typedef enum atomic_memory_order { ... } atomic_memory_order;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `atomic_i32_get` | `func i32 atomic_i32_get(atomic_i32* atom);` |
| `atomic_i32_get_explicit` | `func i32 atomic_i32_get_explicit(atomic_i32* atom, atomic_memory_order order);` |
| `atomic_i32_set` | `func i32 atomic_i32_set(atomic_i32* atom, i32 val);` |
| `atomic_i32_set_explicit` | `func i32 atomic_i32_set_explicit(atomic_i32* atom, i32 val, atomic_memory_order order);` |
| `atomic_i32_cmpex` | `func b32 atomic_i32_cmpex(atomic_i32* atom, i32* expected, i32 desired);` |
| `atomic_i32_add` | `func i32 atomic_i32_add(atomic_i32* atom, i32 delta);` |
| `atomic_i32_and` | `func i32 atomic_i32_and(atomic_i32* atom, i32 mask);` |
| `atomic_i32_or` | `func i32 atomic_i32_or(atomic_i32* atom, i32 mask);` |
| `atomic_i32_xor` | `func i32 atomic_i32_xor(atomic_i32* atom, i32 mask);` |
| `atomic_i32_sub` | `func i32 atomic_i32_sub(atomic_i32* atom, i32 delta);` |
| `atomic_i32_eq` | `func b32 atomic_i32_eq(atomic_i32* atom, i32 val);` |
| `atomic_i32_neq` | `func b32 atomic_i32_neq(atomic_i32* atom, i32 val);` |
| `atomic_i32_lt` | `func b32 atomic_i32_lt(atomic_i32* atom, i32 val);` |
| `atomic_i32_gt` | `func b32 atomic_i32_gt(atomic_i32* atom, i32 val);` |
| `atomic_i32_lte` | `func b32 atomic_i32_lte(atomic_i32* atom, i32 val);` |
| `atomic_i32_gte` | `func b32 atomic_i32_gte(atomic_i32* atom, i32 val);` |
| `atomic_u32_get` | `func u32 atomic_u32_get(atomic_u32* atom);` |
| `atomic_u32_get_explicit` | `func u32 atomic_u32_get_explicit(atomic_u32* atom, atomic_memory_order order);` |
| `atomic_u32_set` | `func u32 atomic_u32_set(atomic_u32* atom, u32 val);` |
| `atomic_u32_set_explicit` | `func u32 atomic_u32_set_explicit(atomic_u32* atom, u32 val, atomic_memory_order order);` |
| `atomic_u32_cmpex` | `func b32 atomic_u32_cmpex(atomic_u32* atom, u32* expected, u32 desired);` |
| `atomic_u32_add` | `func u32 atomic_u32_add(atomic_u32* atom, u32 delta);` |
| `atomic_u32_and` | `func u32 atomic_u32_and(atomic_u32* atom, u32 mask);` |
| `atomic_u32_or` | `func u32 atomic_u32_or(atomic_u32* atom, u32 mask);` |
| `atomic_u32_xor` | `func u32 atomic_u32_xor(atomic_u32* atom, u32 mask);` |
| `atomic_u32_sub` | `func u32 atomic_u32_sub(atomic_u32* atom, u32 delta);` |
| `atomic_u32_eq` | `func b32 atomic_u32_eq(atomic_u32* atom, u32 val);` |
| `atomic_u32_neq` | `func b32 atomic_u32_neq(atomic_u32* atom, u32 val);` |
| `atomic_u32_lt` | `func b32 atomic_u32_lt(atomic_u32* atom, u32 val);` |
| `atomic_u32_gt` | `func b32 atomic_u32_gt(atomic_u32* atom, u32 val);` |
| `atomic_u32_lte` | `func b32 atomic_u32_lte(atomic_u32* atom, u32 val);` |
| `atomic_u32_gte` | `func b32 atomic_u32_gte(atomic_u32* atom, u32 val);` |
| `atomic_i64_get` | `func i64 atomic_i64_get(atomic_i64* atom);` |
| `atomic_i64_get_explicit` | `func i64 atomic_i64_get_explicit(atomic_i64* atom, atomic_memory_order order);` |
| `atomic_i64_set` | `func i64 atomic_i64_set(atomic_i64* atom, i64 val);` |
| `atomic_i64_set_explicit` | `func i64 atomic_i64_set_explicit(atomic_i64* atom, i64 val, atomic_memory_order order);` |
| `atomic_i64_cmpex` | `func b32 atomic_i64_cmpex(atomic_i64* atom, i64* expected, i64 desired);` |
| `atomic_i64_add` | `func i64 atomic_i64_add(atomic_i64* atom, i64 delta);` |
| `atomic_i64_and` | `func i64 atomic_i64_and(atomic_i64* atom, i64 mask);` |
| `atomic_i64_or` | `func i64 atomic_i64_or(atomic_i64* atom, i64 mask);` |
| `atomic_i64_xor` | `func i64 atomic_i64_xor(atomic_i64* atom, i64 mask);` |
| `atomic_i64_sub` | `func i64 atomic_i64_sub(atomic_i64* atom, i64 delta);` |
| `atomic_i64_eq` | `func b32 atomic_i64_eq(atomic_i64* atom, i64 val);` |
| `atomic_i64_neq` | `func b32 atomic_i64_neq(atomic_i64* atom, i64 val);` |
| `atomic_i64_lt` | `func b32 atomic_i64_lt(atomic_i64* atom, i64 val);` |
| `atomic_i64_gt` | `func b32 atomic_i64_gt(atomic_i64* atom, i64 val);` |
| `atomic_i64_lte` | `func b32 atomic_i64_lte(atomic_i64* atom, i64 val);` |
| `atomic_i64_gte` | `func b32 atomic_i64_gte(atomic_i64* atom, i64 val);` |
| `atomic_u64_get` | `func u64 atomic_u64_get(atomic_u64* atom);` |
| `atomic_u64_get_explicit` | `func u64 atomic_u64_get_explicit(atomic_u64* atom, atomic_memory_order order);` |
| `atomic_u64_set` | `func u64 atomic_u64_set(atomic_u64* atom, u64 val);` |
| `atomic_u64_set_explicit` | `func u64 atomic_u64_set_explicit(atomic_u64* atom, u64 val, atomic_memory_order order);` |
| `atomic_u64_cmpex` | `func b32 atomic_u64_cmpex(atomic_u64* atom, u64* expected, u64 desired);` |
| `atomic_u64_add` | `func u64 atomic_u64_add(atomic_u64* atom, u64 delta);` |
| `atomic_u64_and` | `func u64 atomic_u64_and(atomic_u64* atom, u64 mask);` |
| `atomic_u64_or` | `func u64 atomic_u64_or(atomic_u64* atom, u64 mask);` |
| `atomic_u64_xor` | `func u64 atomic_u64_xor(atomic_u64* atom, u64 mask);` |
| `atomic_u64_sub` | `func u64 atomic_u64_sub(atomic_u64* atom, u64 delta);` |
| `atomic_u64_eq` | `func b32 atomic_u64_eq(atomic_u64* atom, u64 val);` |
| `atomic_u64_neq` | `func b32 atomic_u64_neq(atomic_u64* atom, u64 val);` |
| `atomic_u64_lt` | `func b32 atomic_u64_lt(atomic_u64* atom, u64 val);` |
| `atomic_u64_gt` | `func b32 atomic_u64_gt(atomic_u64* atom, u64 val);` |
| `atomic_u64_lte` | `func b32 atomic_u64_lte(atomic_u64* atom, u64 val);` |
| `atomic_u64_gte` | `func b32 atomic_u64_gte(atomic_u64* atom, u64 val);` |
| `atomic_ptr_get` | `func void* atomic_ptr_get(atomic_ptr* atom);` |
| `atomic_ptr_set` | `func void* atomic_ptr_set(atomic_ptr* atom, void* val);` |
| `atomic_ptr_cmpex` | `func b32 atomic_ptr_cmpex(atomic_ptr* atom, void** expected, void* desired);` |
| `atomic_ptr_eq` | `func b32 atomic_ptr_eq(atomic_ptr* atom, void* val);` |
| `atomic_ptr_neq` | `func b32 atomic_ptr_neq(atomic_ptr* atom, void* val);` |
| `atomic_fence_acquire` | `func void atomic_fence_acquire(void);` |
| `atomic_fence_release` | `func void atomic_fence_release(void);` |
| `atomic_fence` | `func void atomic_fence(void);` |
| `atomic_pause` | `func void atomic_pause(void);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `threads/condvar.h`

- Types: **1**
- Functions: **7**
- Macros: **2**

#### Types

| Type | Declaration |
| --- | --- |
| `condvar` | `typedef void* condvar;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_condvar_create` | `func condvar _condvar_create(callsite site);` |
| `_condvar_destroy` | `func b32 _condvar_destroy(condvar cond, callsite site);` |
| `condvar_is_valid` | `func b32 condvar_is_valid(condvar cond);` |
| `condvar_wait` | `func void condvar_wait(condvar cond, mutex mtx);` |
| `condvar_wait_timeout` | `func b32 condvar_wait_timeout(condvar cond, mutex mtx, u32 millis);` |
| `condvar_signal` | `func void condvar_signal(condvar cond);` |
| `condvar_broadcast` | `func void condvar_broadcast(condvar cond);` |

#### Macros

| Macro | Form |
| --- | --- |
| `condvar_create` | `condvar_create()` |
| `condvar_destroy` | `condvar_destroy(cond)` |

### `threads/mutex.h`

- Types: **1**
- Functions: **7**
- Macros: **2**

#### Types

| Type | Declaration |
| --- | --- |
| `mutex` | `typedef void* mutex;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_mutex_create` | `func mutex _mutex_create(callsite site);` |
| `_mutex_destroy` | `func b32 _mutex_destroy(mutex mtx, callsite site);` |
| `mutex_is_valid` | `func b32 mutex_is_valid(mutex mtx);` |
| `mutex_lock` | `func void mutex_lock(mutex mtx);` |
| `mutex_trylock` | `func b32 mutex_trylock(mutex mtx);` |
| `mutex_timed_lock` | `func b32 mutex_timed_lock(mutex mtx, i32 timeout_ms);` |
| `mutex_unlock` | `func void mutex_unlock(mutex mtx);` |

#### Macros

| Macro | Form |
| --- | --- |
| `mutex_create` | `mutex_create()` |
| `mutex_destroy` | `mutex_destroy(mtx)` |

### `threads/rwlock.h`

- Types: **1**
- Functions: **11**
- Macros: **2**

#### Types

| Type | Declaration |
| --- | --- |
| `rwlock` | `typedef void* rwlock;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_rwlock_create` | `func rwlock _rwlock_create(callsite site);` |
| `_rwlock_destroy` | `func b32 _rwlock_destroy(rwlock rw, callsite site);` |
| `rwlock_is_valid` | `func b32 rwlock_is_valid(rwlock rw);` |
| `rwlock_read_lock` | `func void rwlock_read_lock(rwlock rw);` |
| `rwlock_read_unlock` | `func void rwlock_read_unlock(rwlock rw);` |
| `rwlock_write_lock` | `func void rwlock_write_lock(rwlock rw);` |
| `rwlock_write_unlock` | `func void rwlock_write_unlock(rwlock rw);` |
| `rwlock_try_read_lock` | `func b32 rwlock_try_read_lock(rwlock rw);` |
| `rwlock_try_write_lock` | `func b32 rwlock_try_write_lock(rwlock rw);` |
| `rwlock_timed_read_lock` | `func b32 rwlock_timed_read_lock(rwlock rw, i32 timeout_ms);` |
| `rwlock_timed_write_lock` | `func b32 rwlock_timed_write_lock(rwlock rw, i32 timeout_ms);` |

#### Macros

| Macro | Form |
| --- | --- |
| `rwlock_create` | `rwlock_create()` |
| `rwlock_destroy` | `rwlock_destroy(rw)` |

### `threads/semaphore.h`

- Types: **1**
- Functions: **7**
- Macros: **2**

#### Types

| Type | Declaration |
| --- | --- |
| `semaphore` | `typedef void* semaphore;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_semaphore_create` | `func semaphore _semaphore_create(u32 initial_count, callsite site);` |
| `_semaphore_destroy` | `func b32 _semaphore_destroy(semaphore sem, callsite site);` |
| `semaphore_is_valid` | `func b32 semaphore_is_valid(semaphore sem);` |
| `semaphore_acquire` | `func void semaphore_acquire(semaphore sem);` |
| `semaphore_try_acquire` | `func b32 semaphore_try_acquire(semaphore sem);` |
| `semaphore_acquire_timeout` | `func b32 semaphore_acquire_timeout(semaphore sem, u32 millis);` |
| `semaphore_release` | `func void semaphore_release(semaphore sem);` |

#### Macros

| Macro | Form |
| --- | --- |
| `semaphore_create` | `semaphore_create(initial_count)` |
| `semaphore_destroy` | `semaphore_destroy(sem)` |

### `threads/spinlock.h`

- Types: **1**
- Functions: **6**
- Macros: **2**

#### Types

| Type | Declaration |
| --- | --- |
| `spinlock` | `typedef void* spinlock;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_spinlock_create` | `func spinlock _spinlock_create(callsite site);` |
| `_spinlock_destroy` | `func b32 _spinlock_destroy(spinlock sl, callsite site);` |
| `spinlock_is_valid` | `func b32 spinlock_is_valid(spinlock sl);` |
| `spinlock_lock` | `func void spinlock_lock(spinlock sl);` |
| `spinlock_unlock` | `func void spinlock_unlock(spinlock sl);` |
| `spinlock_try_lock` | `func b32 spinlock_try_lock(spinlock sl);` |

#### Macros

| Macro | Form |
| --- | --- |
| `spinlock_create` | `spinlock_create()` |
| `spinlock_destroy` | `spinlock_destroy(sl)` |

### `threads/thread.h`

- Types: **2**
- Functions: **7**
- Macros: **4**

#### Types

| Type | Declaration |
| --- | --- |
| `thread` | `typedef void* thread;` |
| `thread_func` | `typedef i32 (*thread_func)(void* arg);` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_thread_create` | `func thread _thread_create(thread_func entry, void* arg, ctx_setup setup, callsite site);` |
| `_thread_create_named` | `func thread _thread_create_named( thread_func entry, void* arg, cstr8 name, ctx_setup setup, callsite site);` |
| `_thread_detach` | `func b32 _thread_detach(thread thd, callsite site);` |
| `_thread_join` | `func b32 _thread_join(thread thd, i32* out_exit_code, callsite site);` |
| `thread_is_valid` | `func b32 thread_is_valid(thread thd);` |
| `thread_get_id` | `func u64 thread_get_id(thread thd);` |
| `thread_get_name` | `func cstr8 thread_get_name(thread thd);` |

#### Macros

| Macro | Form |
| --- | --- |
| `thread_create` | `thread_create(entry, arg, setup)` |
| `thread_create_named` | `thread_create_named(entry, arg, name, setup)` |
| `thread_detach` | `thread_detach(thd)` |
| `thread_join` | `thread_join(thd, out_exit_code)` |

### `threads/thread_current.h`

- Types: **1**
- Functions: **7**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `thread_priority` | `typedef enum thread_priority { ... } thread_priority;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `thread_id` | `func u64 thread_id(void);` |
| `thread_get_priority` | `func thread_priority thread_get_priority(void);` |
| `thread_set_priority` | `func b32 thread_set_priority(thread_priority priority);` |
| `thread_sleep` | `func void thread_sleep(u32 millis);` |
| `thread_yield` | `func void thread_yield(void);` |
| `thread_sleep_ns` | `func void thread_sleep_ns(u64 nanos);` |
| `thread_sleep_precise` | `func void thread_sleep_precise(u64 nanos);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `threads/thread_group.h`

- Types: **2**
- Functions: **8**
- Macros: **3**

#### Types

| Type | Declaration |
| --- | --- |
| `thread_group_func` | `typedef i32 (*thread_group_func)(u32 idx, void* arg);` |
| `thread_group` | `typedef void* thread_group;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `_thread_group_create` | `func thread_group _thread_group_create( u32 count, thread_group_func entry, void* arg, ctx_setup setup, callsite site);` |
| `_thread_group_create_named` | `func thread_group _thread_group_create_named( u32 count, thread_group_func entry, void* arg, ctx_setup setup, cstr8 base_name, callsite site);` |
| `_thread_group_destroy` | `func b32 _thread_group_destroy(thread_group group, callsite site);` |
| `thread_group_is_valid` | `func b32 thread_group_is_valid(thread_group group);` |
| `thread_group_get_count` | `func u32 thread_group_get_count(thread_group group);` |
| `thread_group_get` | `func thread thread_group_get(thread_group group, u32 idx);` |
| `thread_group_join_all` | `func b32 thread_group_join_all(thread_group group, i32* out_exit_codes);` |
| `thread_group_detach_all` | `func b32 thread_group_detach_all(thread_group group);` |

#### Macros

| Macro | Form |
| --- | --- |
| `thread_group_create` | `thread_group_create(count, entry, arg, setup)` |
| `thread_group_create_named` | `thread_group_create_named(count, entry, arg, setup, base_name)` |
| `thread_group_destroy` | `thread_group_destroy(group)` |

## utils

Utility helpers such as command line parsing, compression, logging, timers, IDs, and versioning.

### `utils/cmdline.h`

- Types: **1**
- Functions: **10**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `cmdline` | `typedef struct cmdline { ... } cmdline;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `cmdline_build` | `func cmdline cmdline_build(sz count, c8** args);` |
| `cmdline_get_count` | `func sz cmdline_get_count(cmdline cmdl);` |
| `cmdline_is_empty` | `func b32 cmdline_is_empty(cmdline cmdl);` |
| `cmdline_get_arg` | `func cstr8 cmdline_get_arg(cmdline cmdl, sz idx);` |
| `cmdline_get_program` | `func cstr8 cmdline_get_program(cmdline cmdl);` |
| `cmdline_find` | `func b32 cmdline_find(cmdline cmdl, cstr8 arg, sz* out_idx);` |
| `cmdline_has` | `func b32 cmdline_has(cmdline cmdl, cstr8 arg);` |
| `cmdline_get_option` | `func cstr8 cmdline_get_option(cmdline cmdl, cstr8 name);` |
| `cmdline_get_option_i64` | `func b32 cmdline_get_option_i64(cmdline cmdl, cstr8 name, i64* out);` |
| `cmdline_get_option_f64` | `func b32 cmdline_get_option_f64(cmdline cmdl, cstr8 name, f64* out);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `utils/compress.h`

- Types: **1**
- Functions: **3**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `compress_error` | `typedef enum compress_error { ... } compress_error;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `compress_error_to_cstr` | `func cstr8 compress_error_to_cstr(compress_error error);` |
| `compress_encode` | `func compress_error compress_encode(buffer src, allocator alloc, buffer* out_compressed);` |
| `compress_decode` | `func compress_error compress_decode(buffer src, allocator alloc, buffer* out_decompressed);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `utils/crc.h`

- Types: **0**
- Functions: **8**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `crc32_init` | `func u32 crc32_init(void);` |
| `crc32_update` | `func u32 crc32_update(u32 crc, const void* data, sz size);` |
| `crc32_finalize` | `func u32 crc32_finalize(u32 crc);` |
| `crc32` | `func u32 crc32(const void* data, sz size);` |
| `crc64_init` | `func u64 crc64_init(void);` |
| `crc64_update` | `func u64 crc64_update(u64 crc, const void* data, sz size);` |
| `crc64_finalize` | `func u64 crc64_finalize(u64 crc);` |
| `crc64` | `func u64 crc64(const void* data, sz size);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `utils/digits.h`

- Types: **0**
- Functions: **20**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `u8_digits` | `func sz u8_digits(u8 value);` |
| `u16_digits` | `func sz u16_digits(u16 value);` |
| `u32_digits` | `func sz u32_digits(u32 value);` |
| `u64_digits` | `func sz u64_digits(u64 value);` |
| `i8_digits` | `func sz i8_digits(i8 value);` |
| `i16_digits` | `func sz i16_digits(i16 value);` |
| `i32_digits` | `func sz i32_digits(i32 value);` |
| `i64_digits` | `func sz i64_digits(i64 value);` |
| `u8x_digits` | `func sz u8x_digits(u8x value);` |
| `u16x_digits` | `func sz u16x_digits(u16x value);` |
| `u32x_digits` | `func sz u32x_digits(u32x value);` |
| `u64x_digits` | `func sz u64x_digits(u64x value);` |
| `sz_digits` | `func sz sz_digits(sz value);` |
| `up_digits` | `func sz up_digits(up value);` |
| `i8x_digits` | `func sz i8x_digits(i8x value);` |
| `i16x_digits` | `func sz i16x_digits(i16x value);` |
| `i32x_digits` | `func sz i32x_digits(i32x value);` |
| `i64x_digits` | `func sz i64x_digits(i64x value);` |
| `sp_digits` | `func sz sp_digits(sp value);` |
| `dp_digits` | `func sz dp_digits(dp value);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `utils/endian.h`

- Types: **0**
- Functions: **14**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `endian_is_little` | `func b32 endian_is_little(void);` |
| `endian_is_big` | `func b32 endian_is_big(void);` |
| `endian_le16_to_native` | `func u16 endian_le16_to_native(u16 value);` |
| `endian_le32_to_native` | `func u32 endian_le32_to_native(u32 value);` |
| `endian_le64_to_native` | `func u64 endian_le64_to_native(u64 value);` |
| `endian_be16_to_native` | `func u16 endian_be16_to_native(u16 value);` |
| `endian_be32_to_native` | `func u32 endian_be32_to_native(u32 value);` |
| `endian_be64_to_native` | `func u64 endian_be64_to_native(u64 value);` |
| `endian_native_to_le16` | `func u16 endian_native_to_le16(u16 value);` |
| `endian_native_to_le32` | `func u32 endian_native_to_le32(u32 value);` |
| `endian_native_to_le64` | `func u64 endian_native_to_le64(u64 value);` |
| `endian_native_to_be16` | `func u16 endian_native_to_be16(u16 value);` |
| `endian_native_to_be32` | `func u32 endian_native_to_be32(u32 value);` |
| `endian_native_to_be64` | `func u64 endian_native_to_be64(u64 value);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `utils/huffman.h`

- Types: **2**
- Functions: **8**
- Macros: **4**

#### Types

| Type | Declaration |
| --- | --- |
| `huffman_code` | `typedef struct huffman_code { ... } huffman_code;` |
| `huffman_tree` | `typedef struct huffman_tree { ... } huffman_tree;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `huffman_tree_clear` | `func void huffman_tree_clear(huffman_tree* tree);` |
| `huffman_tree_build` | `func b32 huffman_tree_build(huffman_tree* tree, const u64* frequencies, sz symbol_count);` |
| `huffman_tree_build_for_bytes` | `func b32 huffman_tree_build_for_bytes(huffman_tree* tree, buffer src);` |
| `huffman_tree_has_symbol` | `func b32 huffman_tree_has_symbol(const huffman_tree* tree, u8 symbol);` |
| `huffman_tree_get_code` | `func huffman_code huffman_tree_get_code(const huffman_tree* tree, u8 symbol);` |
| `huffman_code_get_bit` | `func b32 huffman_code_get_bit(const huffman_code* code, sz bit_idx);` |
| `huffman_tree_bit_length_for_bytes` | `func u64 huffman_tree_bit_length_for_bytes(const huffman_tree* tree, buffer src);` |
| `huffman_tree_decode_symbol` | `func b32 huffman_tree_decode_symbol( const huffman_tree* tree, buffer bitstream, sz bit_offset, u8* out_symbol, sz* out_next_bit_offset);` |

#### Macros

| Macro | Form |
| --- | --- |
| `HUFFMAN_SYMBOL_CAP` | `HUFFMAN_SYMBOL_CAP` |
| `HUFFMAN_CODE_WORD_CAP` | `HUFFMAN_CODE_WORD_CAP` |
| `HUFFMAN_NODE_CAP` | `HUFFMAN_NODE_CAP` |
| `HUFFMAN_INVALID_NODE` | `HUFFMAN_INVALID_NODE` |

### `utils/id.h`

- Types: **4**
- Functions: **44**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `id8` | `typedef struct id8 { ... } id8;` |
| `id16` | `typedef struct id16 { ... } id16;` |
| `id32` | `typedef struct id32 { ... } id32;` |
| `id64` | `typedef struct id64 { ... } id64;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `id8_zero` | `func id8 id8_zero(void);` |
| `id8_make` | `func id8 id8_make(u8 value);` |
| `id8_get` | `func u8 id8_get(id8 ident);` |
| `id8_is_zero` | `func b32 id8_is_zero(id8 ident);` |
| `id8_is_valid` | `func b32 id8_is_valid(id8 ident);` |
| `id8_next` | `func id8 id8_next(id8 ident);` |
| `id8_cmp` | `func i32 id8_cmp(id8 lhs, id8 rhs);` |
| `id8_string_length` | `func sz id8_string_length(id8 ident);` |
| `id8_parse_cstr8` | `func b32 id8_parse_cstr8(cstr8 src, id8* out);` |
| `id8_to_cstr8` | `func b32 id8_to_cstr8(id8 ident, c8* dst, sz cap);` |
| `id8_to_str8` | `func b32 id8_to_str8(id8 ident, str8* dst);` |
| `id16_zero` | `func id16 id16_zero(void);` |
| `id16_make` | `func id16 id16_make(u16 value);` |
| `id16_get` | `func u16 id16_get(id16 ident);` |
| `id16_is_zero` | `func b32 id16_is_zero(id16 ident);` |
| `id16_is_valid` | `func b32 id16_is_valid(id16 ident);` |
| `id16_next` | `func id16 id16_next(id16 ident);` |
| `id16_cmp` | `func i32 id16_cmp(id16 lhs, id16 rhs);` |
| `id16_string_length` | `func sz id16_string_length(id16 ident);` |
| `id16_parse_cstr8` | `func b32 id16_parse_cstr8(cstr8 src, id16* out);` |
| `id16_to_cstr8` | `func b32 id16_to_cstr8(id16 ident, c8* dst, sz cap);` |
| `id16_to_str8` | `func b32 id16_to_str8(id16 ident, str8* dst);` |
| `id32_zero` | `func id32 id32_zero(void);` |
| `id32_make` | `func id32 id32_make(u32 value);` |
| `id32_get` | `func u32 id32_get(id32 ident);` |
| `id32_is_zero` | `func b32 id32_is_zero(id32 ident);` |
| `id32_is_valid` | `func b32 id32_is_valid(id32 ident);` |
| `id32_next` | `func id32 id32_next(id32 ident);` |
| `id32_cmp` | `func i32 id32_cmp(id32 lhs, id32 rhs);` |
| `id32_string_length` | `func sz id32_string_length(id32 ident);` |
| `id32_parse_cstr8` | `func b32 id32_parse_cstr8(cstr8 src, id32* out);` |
| `id32_to_cstr8` | `func b32 id32_to_cstr8(id32 ident, c8* dst, sz cap);` |
| `id32_to_str8` | `func b32 id32_to_str8(id32 ident, str8* dst);` |
| `id64_zero` | `func id64 id64_zero(void);` |
| `id64_make` | `func id64 id64_make(u64 value);` |
| `id64_get` | `func u64 id64_get(id64 ident);` |
| `id64_is_zero` | `func b32 id64_is_zero(id64 ident);` |
| `id64_is_valid` | `func b32 id64_is_valid(id64 ident);` |
| `id64_next` | `func id64 id64_next(id64 ident);` |
| `id64_cmp` | `func i32 id64_cmp(id64 lhs, id64 rhs);` |
| `id64_string_length` | `func sz id64_string_length(id64 ident);` |
| `id64_parse_cstr8` | `func b32 id64_parse_cstr8(cstr8 src, id64* out);` |
| `id64_to_cstr8` | `func b32 id64_to_cstr8(id64 ident, c8* dst, sz cap);` |
| `id64_to_str8` | `func b32 id64_to_str8(id64 ident, str8* dst);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `utils/log_state.h`

- Types: **4**
- Functions: **18**
- Macros: **10**

#### Types

| Type | Declaration |
| --- | --- |
| `log_state` | `typedef struct log_state log_state;` |
| `log_level` | `typedef enum log_level { ... } log_level;` |
| `log_msg` | `typedef struct log_msg { ... } log_msg;` |
| `log_frame` | `typedef struct log_frame { ... } log_frame;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `log_level_to_str` | `func cstr8 log_level_to_str(log_level level);` |
| `log_state_set_level` | `func void log_state_set_level(log_state* state, log_level level);` |
| `log_msg_next` | `func log_msg* log_msg_next(log_msg* msg);` |
| `log_msg_level` | `func log_level log_msg_level(log_msg* msg);` |
| `log_msg_site` | `func callsite log_msg_site(log_msg* msg);` |
| `log_msg_text` | `func cstr8 log_msg_text(log_msg* msg);` |
| `log_frame_has_msgs` | `func b32 log_frame_has_msgs(log_frame* frame);` |
| `log_frame_msg_count` | `func sz log_frame_msg_count(log_frame* frame);` |
| `log_frame_first` | `func log_msg* log_frame_first(log_frame* frame);` |
| `log_frame_last` | `func log_msg* log_frame_last(log_frame* frame);` |
| `_log_state_init` | `func b32 _log_state_init(log_state* state, mutex mutex_handle, allocator alloc, callsite site);` |
| `_log_state_quit` | `func void _log_state_quit(log_state* state, callsite site);` |
| `log_state_is_init` | `func b32 log_state_is_init(log_state* state);` |
| `log_state_clear` | `func b32 log_state_clear(log_state* state);` |
| `log_state_sync` | `func void log_state_sync(log_state* dst, log_state* src);` |
| `log_state_begin_frame` | `func void log_state_begin_frame(log_state* state);` |
| `log_state_end_frame` | `func log_frame* log_state_end_frame(log_state* state, u32 severity_mask);` |
| `_log` | `func void _log(log_state* state, log_level level, callsite site, const char* msg, ...);` |

#### Macros

| Macro | Form |
| --- | --- |
| `LOG_LEVEL_DEFAULT` | `LOG_LEVEL_DEFAULT` |
| `log_state_init` | `log_state_init(state, mutex_handle, alloc)` |
| `log_state_quit` | `log_state_quit(state)` |
| `log_state_fatal` | `log_state_fatal(state, ...)` |
| `log_state_error` | `log_state_error(state, ...)` |
| `log_state_warn` | `log_state_warn(state, ...)` |
| `log_state_info` | `log_state_info(state, ...)` |
| `log_state_debug` | `log_state_debug(state, ...)` |
| `log_state_verbose` | `log_state_verbose(state, ...)` |
| `log_state_trace` | `log_state_trace(state, ...)` |

### `utils/random_series.h`

- Types: **1**
- Functions: **29**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `random_series` | `typedef struct random_series { ... } random_series;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `random_series_seed` | `func void random_series_seed(random_series* series, u32 seed);` |
| `random_series_reset` | `func void random_series_reset(random_series* series);` |
| `random_series_chance_f64` | `func b32 random_series_chance_f64(random_series* series, f64 chance);` |
| `random_series_chance_f32` | `func b32 random_series_chance_f32(random_series* series, f32 chance);` |
| `random_series_chance` | `func b32 random_series_chance(random_series* series, u32x chance);` |
| `random_series_u8` | `func u8 random_series_u8(random_series* series);` |
| `random_series_u16` | `func u16 random_series_u16(random_series* series);` |
| `random_series_u32` | `func u32 random_series_u32(random_series* series);` |
| `random_series_u64` | `func u64 random_series_u64(random_series* series);` |
| `random_series_i8` | `func i8 random_series_i8(random_series* series);` |
| `random_series_i16` | `func i16 random_series_i16(random_series* series);` |
| `random_series_i32` | `func i32 random_series_i32(random_series* series);` |
| `random_series_i64` | `func i64 random_series_i64(random_series* series);` |
| `random_series_f32` | `func f32 random_series_f32(random_series* series);` |
| `random_series_f64` | `func f64 random_series_f64(random_series* series);` |
| `random_series_rng_u8` | `func u8 random_series_rng_u8(random_series* series, u8 min_value, u8 max_value);` |
| `random_series_rng_u16` | `func u16 random_series_rng_u16(random_series* series, u16 min_value, u16 max_value);` |
| `random_series_rng_u32` | `func u32 random_series_rng_u32(random_series* series, u32 min_value, u32 max_value);` |
| `random_series_rng_u64` | `func u64 random_series_rng_u64(random_series* series, u64 min_value, u64 max_value);` |
| `random_series_rng_i8` | `func i8 random_series_rng_i8(random_series* series, i8 min_value, i8 max_value);` |
| `random_series_rng_i16` | `func i16 random_series_rng_i16(random_series* series, i16 min_value, i16 max_value);` |
| `random_series_rng_i32` | `func i32 random_series_rng_i32(random_series* series, i32 min_value, i32 max_value);` |
| `random_series_rng_i64` | `func i64 random_series_rng_i64(random_series* series, i64 min_value, i64 max_value);` |
| `random_series_rng_f32` | `func f32 random_series_rng_f32(random_series* series, f32 min_value, f32 max_value);` |
| `random_series_rng_f64` | `func f64 random_series_rng_f64(random_series* series, f64 min_value, f64 max_value);` |
| `random_series_u_f32` | `func f32 random_series_u_f32(random_series* series);` |
| `random_series_u_f64` | `func f64 random_series_u_f64(random_series* series);` |
| `random_series_b_f32` | `func f32 random_series_b_f32(random_series* series);` |
| `random_series_b_f64` | `func f64 random_series_b_f64(random_series* series);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `utils/stacktrace.h`

- Types: **1**
- Functions: **1**
- Macros: **1**

#### Types

| Type | Declaration |
| --- | --- |
| `stacktrace_frame` | `typedef struct stacktrace_frame { ... } stacktrace_frame;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `stacktrace_capture` | `func sz stacktrace_capture(stacktrace_frame* out_frames, sz out_capacity, sz skip_frames);` |

#### Macros

| Macro | Form |
| --- | --- |
| `STACKTRACE_CAPTURE_CAP` | `STACKTRACE_CAPTURE_CAP` |

### `utils/timer.h`

- Types: **0**
- Functions: **5**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `timer_clear` | `func void timer_clear(f32* timer);` |
| `timer_bump` | `func void timer_bump(f32* timer);` |
| `timer_increment` | `func void timer_increment(f32* timer, f32 dt, f32 scale);` |
| `timer_consume` | `func b32 timer_consume(f32* timer, f32 rate);` |
| `timer_consume_once` | `func b32 timer_consume_once(f32* timer, f32 rate);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `utils/timestamp.h`

- Types: **1**
- Functions: **19**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `timestamp` | `typedef struct timestamp { ... } timestamp;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `timestamp_zero` | `func timestamp timestamp_zero(void);` |
| `timestamp_now` | `func timestamp timestamp_now(void);` |
| `timestamp_from_microseconds` | `func timestamp timestamp_from_microseconds(i64 microseconds);` |
| `timestamp_from_milliseconds` | `func timestamp timestamp_from_milliseconds(i64 milliseconds);` |
| `timestamp_from_seconds` | `func timestamp timestamp_from_seconds(f64 seconds);` |
| `timestamp_from_minutes` | `func timestamp timestamp_from_minutes(f64 minutes);` |
| `timestamp_from_hours` | `func timestamp timestamp_from_hours(f64 hours);` |
| `timestamp_as_microseconds` | `func i64 timestamp_as_microseconds(timestamp value);` |
| `timestamp_as_milliseconds` | `func f64 timestamp_as_milliseconds(timestamp value);` |
| `timestamp_as_seconds` | `func f64 timestamp_as_seconds(timestamp value);` |
| `timestamp_as_minutes` | `func f64 timestamp_as_minutes(timestamp value);` |
| `timestamp_as_hours` | `func f64 timestamp_as_hours(timestamp value);` |
| `timestamp_is_zero` | `func b32 timestamp_is_zero(timestamp value);` |
| `timestamp_add` | `func timestamp timestamp_add(timestamp lhs, timestamp rhs);` |
| `timestamp_sub` | `func timestamp timestamp_sub(timestamp lhs, timestamp rhs);` |
| `timestamp_scale` | `func timestamp timestamp_scale(timestamp value, f64 factor);` |
| `timestamp_abs` | `func timestamp timestamp_abs(timestamp value);` |
| `timestamp_clamp` | `func timestamp timestamp_clamp(timestamp value, timestamp min_value, timestamp max_value);` |
| `timestamp_cmp` | `func i32 timestamp_cmp(timestamp lhs, timestamp rhs);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `utils/uuid.h`

- Types: **1**
- Functions: **20**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `uuid` | `typedef struct uuid { ... } uuid;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `uuid_zero` | `func uuid uuid_zero(void);` |
| `uuid_from_bytes` | `func uuid uuid_from_bytes(const u8* bytes);` |
| `uuid_from_u64` | `func uuid uuid_from_u64(u64 upper, u64 lower);` |
| `uuid_get_bytes` | `func void uuid_get_bytes(uuid value, u8* dst);` |
| `uuid_get_upper` | `func u64 uuid_get_upper(uuid value);` |
| `uuid_get_lower` | `func u64 uuid_get_lower(uuid value);` |
| `uuid_is_zero` | `func b32 uuid_is_zero(uuid value);` |
| `uuid_equal` | `func b32 uuid_equal(uuid lhs, uuid rhs);` |
| `uuid_cmp` | `func i32 uuid_cmp(uuid lhs, uuid rhs);` |
| `uuid_get_version` | `func u8 uuid_get_version(uuid value);` |
| `uuid_get_variant` | `func u8 uuid_get_variant(uuid value);` |
| `uuid_string_length` | `func sz uuid_string_length(void);` |
| `uuid_parse_cstr8` | `func b32 uuid_parse_cstr8(cstr8 src, uuid* out);` |
| `uuid_to_cstr8` | `func b32 uuid_to_cstr8(uuid value, c8* dst, sz cap);` |
| `uuid_to_cstr16` | `func b32 uuid_to_cstr16(uuid value, c16* dst, sz cap);` |
| `uuid_to_cstr32` | `func b32 uuid_to_cstr32(uuid value, c32* dst, sz cap);` |
| `uuid_to_str8` | `func b32 uuid_to_str8(uuid value, str8* dst);` |
| `uuid_to_str16` | `func b32 uuid_to_str16(uuid value, str16* dst);` |
| `uuid_to_str32` | `func b32 uuid_to_str32(uuid value, str32* dst);` |
| `uuid_generate_v4` | `func uuid uuid_generate_v4(random_series* series);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |

### `utils/version.h`

- Types: **1**
- Functions: **17**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| `version` | `typedef union version { ... } version;` |

#### Functions

| Function | Declaration |
| --- | --- |
| `version_zero` | `func version version_zero(void);` |
| `version_from_packed` | `func version version_from_packed(u32 packed);` |
| `version_make` | `func version version_make(u8 major, u8 minor, u16 patch);` |
| `version_get_packed` | `func u32 version_get_packed(version ver);` |
| `version_get_major` | `func u8 version_get_major(version ver);` |
| `version_get_minor` | `func u8 version_get_minor(version ver);` |
| `version_get_patch` | `func u16 version_get_patch(version ver);` |
| `version_is_zero` | `func b32 version_is_zero(version ver);` |
| `version_string_length` | `func sz version_string_length(version ver);` |
| `version_cmp` | `func i32 version_cmp(version lhs, version rhs);` |
| `version_to_cstr8` | `func b32 version_to_cstr8(version ver, c8* dst, sz cap);` |
| `version_to_cstr16` | `func b32 version_to_cstr16(version ver, c16* dst, sz cap);` |
| `version_to_cstr32` | `func b32 version_to_cstr32(version ver, c32* dst, sz cap);` |
| `version_to_str8` | `func b32 version_to_str8(version ver, str8* dst);` |
| `version_to_str16` | `func b32 version_to_str16(version ver, str16* dst);` |
| `version_to_str32` | `func b32 version_to_str32(version ver, str32* dst);` |
| `version_parse_cstr8` | `func b32 version_parse_cstr8(cstr8 src, version* out_ver);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |
