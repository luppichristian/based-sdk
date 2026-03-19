// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

#if defined(_WIN32)
#  define ECHO_CMD  "whoami.exe"
#  define ECHO_ARGS "whoami.exe", NULL
#else
#  define ECHO_CMD  "/bin/echo"
#  define ECHO_ARGS "/bin/echo", "hello", NULL
#endif

TEST(processes_process_test, options_default) {
  process_options opts = process_options_default();
  EXPECT_EQ(0, opts.pipe_stdin);
  EXPECT_EQ(0, opts.pipe_stdout);
  EXPECT_EQ(0, opts.pipe_stderr);
  EXPECT_EQ(0, opts.stderr_to_stdout);
  EXPECT_EQ(0, opts.background);
}

TEST(processes_process_test, options_captured) {
  process_options opts = process_options_captured();
  EXPECT_EQ(0, opts.pipe_stdin);
  EXPECT_NE(0, opts.pipe_stdout);
  EXPECT_NE(0, opts.stderr_to_stdout);
}

TEST(processes_process_test, create_invalid) {
  cstr8 const invalid_args[] = {NULL};
  process prc = process_create(invalid_args);
  EXPECT_EQ(0, process_is_valid(prc));
}

TEST(processes_process_test, create_with_echo) {
  cstr8 const args[] = {ECHO_ARGS};
  process prc = process_create(args);
  EXPECT_NE(0, process_is_valid(prc));
  process_destroy(prc);
}

TEST(processes_process_test, create_with_options) {
  process_options opts = process_options_default();
  opts.pipe_stdout = 1;
  cstr8 const args[] = {ECHO_ARGS};
  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));
  process_destroy(prc);
}

TEST(processes_process_test, get_id) {
  cstr8 const args[] = {ECHO_ARGS};
  process prc = process_create(args);
  EXPECT_NE(0, process_is_valid(prc));
  u64 proc_id = process_get_id(prc);
  EXPECT_NE(0U, proc_id);
  process_destroy(prc);
}

TEST(processes_process_test, wait_blocking) {
  cstr8 const args[] = {ECHO_ARGS};
  process prc = process_create(args);
  EXPECT_NE(0, process_is_valid(prc));

  i32 exit_code = -1;
  b32 waited = process_wait(prc, 1, &exit_code);
  EXPECT_NE(0, waited);
  EXPECT_EQ(0, exit_code);

  process_destroy(prc);
}

TEST(processes_process_test, wait_nonblocking) {
#if defined(_WIN32)
  cstr8 const args[] = {"cmd.exe", "/c", "ping -n 4 127.0.0.1 > nul", NULL};
#else
  cstr8 const args[] = {"/bin/sleep", "1", NULL};
#endif
  process prc = process_create(args);
  EXPECT_NE(0, process_is_valid(prc));

  i32 exit_code = -1;
  b32 waited = process_wait(prc, 0, &exit_code);
  EXPECT_EQ(0, waited);
  EXPECT_EQ(-1, exit_code);

  b32 killed = process_kill(prc, 1);
  if (killed == 0) {
    process_destroy(prc);
    GTEST_SKIP() << "Process kill is not available on this platform/environment";
  }

  b32 completed = process_wait_timeout(prc, 5000, NULL);
  EXPECT_NE(0, completed);

  process_destroy(prc);
}

TEST(processes_process_test, wait_timeout) {
  cstr8 const args[] = {ECHO_ARGS};
  process prc = process_create(args);
  EXPECT_NE(0, process_is_valid(prc));

  i32 exit_code = -1;
  b32 waited = process_wait_timeout(prc, 5000, &exit_code);
  EXPECT_NE(0, waited);
  EXPECT_EQ(0, exit_code);

  process_destroy(prc);
}

TEST(processes_process_test, kill) {
  process_options opts = process_options_default();
  opts.background = 1;

#if defined(_WIN32)
  cstr8 const args[] = {"cmd.exe", "/c", "ping -t localhost", NULL};
#else
  cstr8 const args[] = {"/bin/sleep", "60", NULL};
#endif

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  b32 killed = process_kill(prc, 1);
  if (killed == 0) {
    process_destroy(prc);
    GTEST_SKIP() << "Process kill is not available on this platform/environment";
  }
  EXPECT_NE(0, killed);

  i32 exit_code = -1;
  b32 waited = process_wait_timeout(prc, 5000, &exit_code);
  EXPECT_NE(0, waited);

  process_destroy(prc);
}
