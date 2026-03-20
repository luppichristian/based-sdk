// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

#if defined(_WIN32)
#  define CAT_CMD  "cmd.exe"
#  define CAT_ARGS "cmd.exe", "/c", "more", NULL
#else
#  define CAT_CMD  "/bin/sh"
#  define CAT_ARGS "/bin/sh", "-c", "cat", NULL
#endif

namespace {

  func void process_wait_and_destroy(process prc) {
    if (process_is_valid(prc) == 0) {
      return;
    }

    i32 exit_code = -1;
    b32 finished = process_wait_timeout(prc, 2000, &exit_code);
    if (finished == 0) {
      process_kill(prc, 1);
      process_wait_timeout(prc, 2000, NULL);
    }

    process_destroy(prc);
  }

}  // namespace

TEST(processes_process_pipe_test, stdin_invalid) {
  process_options opts = process_options_default();

#if defined(_WIN32)
  cstr8 const args[] = {"cmd.exe", "/c", "exit 0", NULL};
#else
  cstr8 const args[] = {"/bin/echo", "stdin-invalid", NULL};
#endif

  process prc = process_create_with(args, opts);
  if (process_is_valid(prc) == 0) {
    GTEST_SKIP() << "Unable to spawn test process on this environment";
  }
  EXPECT_NE(0, process_is_valid(prc));

  process_pipe pin = process_pipe_stdin(prc);
  EXPECT_EQ(nullptr, pin);

  process_wait_and_destroy(prc);
}

TEST(processes_process_pipe_test, stdout_valid) {
  process_options opts = process_options_default();
  opts.pipe_stdout = 1;

#if defined(_WIN32)
  cstr8 const args[] = {"cmd.exe", "/c", "echo test", NULL};
#else
  cstr8 const args[] = {"/bin/sh", "-c", "printf test", NULL};
#endif

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  process_pipe pout = process_pipe_stdout(prc);
  EXPECT_NE(nullptr, pout);
  EXPECT_NE(0, process_pipe_is_valid(pout));

  process_wait_and_destroy(prc);
}

TEST(processes_process_pipe_test, stderr_valid) {
  process_options opts = process_options_default();
  opts.pipe_stderr = 1;

#if defined(_WIN32)
  cstr8 const args[] = {"cmd.exe", "/c", "echo test 1>&2", NULL};
#else
  cstr8 const args[] = {"/bin/sh", "-c", "echo test >&2", NULL};
#endif

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  process_pipe perr = process_pipe_stderr(prc);
  EXPECT_NE(nullptr, perr);
  EXPECT_NE(0, process_pipe_is_valid(perr));

  process_wait_and_destroy(prc);
}

TEST(processes_process_pipe_test, process_pipe_read) {
  process_options opts = process_options_default();
  opts.pipe_stdout = 1;

#if defined(_WIN32)
  cstr8 const args[] = {"whoami.exe", NULL};
#else
  cstr8 const args[] = {"/bin/sh", "-c", "printf pipe", NULL};
#endif

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  process_pipe pout = process_pipe_stdout(prc);
  EXPECT_NE(nullptr, pout);

  i32 exit_code = -1;
  EXPECT_NE(0, process_wait(prc, 1, &exit_code));

  c8 buffer[256] = {0};
  sz bytes_read = process_pipe_read(pout, buffer, sizeof(buffer) - 1);

  EXPECT_GT(bytes_read, 0U);
  EXPECT_GT(cstr8_len(buffer), 0U);

  EXPECT_EQ(0, exit_code);
  process_wait_and_destroy(prc);
}

TEST(processes_process_pipe_test, process_pipe_write) {
  process_options opts = process_options_default();
  opts.pipe_stdin = 1;
  opts.pipe_stdout = 1;

  cstr8 const args[] = {CAT_ARGS};

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  process_pipe pin = process_pipe_stdin(prc);
  EXPECT_NE(nullptr, pin);

  const c8 test_data[] = "test input data\n";
  sz bytes_written = process_pipe_write(pin, test_data, sizeof(test_data) - 1);

  EXPECT_GT(bytes_written, 0U);

  process_pipe_close(pin);

  process_wait_and_destroy(prc);
}

TEST(processes_process_pipe_test, process_pipe_close) {
  process_options opts = process_options_default();
  opts.pipe_stdin = 1;
  opts.pipe_stdout = 1;

  cstr8 const args[] = {CAT_ARGS};

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  process_pipe pin = process_pipe_stdin(prc);
  EXPECT_NE(nullptr, pin);

  process_pipe_close(pin);

  i32 exit_code = -1;
  b32 waited = process_wait(prc, 1, &exit_code);
  EXPECT_NE(0, waited);

  process_wait_and_destroy(prc);
}
