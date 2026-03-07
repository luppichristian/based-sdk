- Rename display.h, display.c to monitor.h, monitor.c:
  Rename all apis that reference "displays". Rename msg enums, device enums, etc.. Everything should be renamed.
- At the moment the messaging system only supports messages of category MSG_CATEGORY_CORE:
  You need to register user space for all other categories and use the same underlying SDL3 system. 
- Can you add process_is_unique() to process_current api?
  It would be useful to check if multiple instances of the same executable are open.
- Can you add process_restart() to process_current api?
  basically quits the current process, and then opens a new one with the same data/setup, etc...
  Useful for like buttons "This game requires restart..."
- Remove some unnecessary platform detection in env_defines.h,
  realistically we only support windows, linux, unix, macos.
  Make sure that for each platform dependent implementation where you check for these macros, functionality is supported on all detected platforms (windows, linux, unix, macos).
- Are allocators in global_ctx being created with the global mutex? each allocator shouldnt have its own mutex, they just all reference the global_ctx mutex.
- In entry.c we dont properly init/quit all the systems:
  Need to call olib_set_memory_fns and make them all use the thread ctx allocator.
  Need to call SDL_SetMemoryFunctions and make them all use the thread ctx allocator.
  All allocations from library needs to pass through our memory pipeline, if you think we should do this for other libraries feel free to do it.
- Create new module windowing/dialog.h, windowing/dialog.c:
  All functions to create message boxes, file system dialogs like open window, open file
- Create new module processes/common_processes.h
  Open weblink with default browser
  Open application with commandline
  Open filesystem window...
  Open shell/powersheell (for all platforms) terminals.......