// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

#if defined(_WIN32)
#  define CAT_CMD  "cmd.exe"
#  define CAT_ARGS "cmd.exe", "/c", "more", nullptr
#else
#  define CAT_CMD  "/bin/cat"
#  define CAT_ARGS "/bin/cat", nullptr
#endif

TEST(processes_pipe_test, stdin_invalid) {
  process_options opts = process_options_default();

#if defined(_WIN32)
  cstr8 const args[] = {"cmd.exe", "/c", "exit 0", nullptr};
#else
  cstr8 const args[] = {"/bin/true", nullptr};
#endif

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  pipe pin = pipe_stdin(prc);
  EXPECT_EQ(nullptr, pin);

  process_wait(prc, 1, nullptr);
  process_destroy(prc);
}

TEST(processes_pipe_test, stdout_valid) {
  process_options opts = process_options_default();
  opts.pipe_stdout = 1;

#if defined(_WIN32)
  cstr8 const args[] = {"cmd.exe", "/c", "echo test", nullptr};
#else
  cstr8 const args[] = {"/bin/echo", "test", nullptr};
#endif

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  pipe pout = pipe_stdout(prc);
  EXPECT_NE(nullptr, pout);
  EXPECT_NE(0, pipe_is_valid(pout));

  process_wait(prc, 1, nullptr);
  process_destroy(prc);
}

TEST(processes_pipe_test, stderr_valid) {
  process_options opts = process_options_default();
  opts.pipe_stderr = 1;

#if defined(_WIN32)
  cstr8 const args[] = {"cmd.exe", "/c", "echo test 1>&2", nullptr};
#else
  cstr8 const args[] = {"/bin/sh", "-c", "echo test >&2", nullptr};
#endif

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  pipe perr = pipe_stderr(prc);
  EXPECT_NE(nullptr, perr);
  EXPECT_NE(0, pipe_is_valid(perr));

  process_wait(prc, 1, nullptr);
  process_destroy(prc);
}

TEST(processes_pipe_test, pipe_read) {
  process_options opts = process_options_default();
  opts.pipe_stdout = 1;

#if defined(_WIN32)
  cstr8 const args[] = {"whoami.exe", nullptr};
#else
  cstr8 const args[] = {"/bin/echo", "pipe", nullptr};
#endif

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  pipe pout = pipe_stdout(prc);
  EXPECT_NE(nullptr, pout);

  i32 exit_code = -1;
  EXPECT_NE(0, process_wait(prc, 1, &exit_code));

  c8 buffer[256] = {0};
  sz bytes_read = pipe_read(pout, buffer, sizeof(buffer) - 1);

  EXPECT_GT(bytes_read, 0U);
  EXPECT_GT(cstr8_len(buffer), 0U);

  EXPECT_EQ(0, exit_code);
  process_destroy(prc);
}

TEST(processes_pipe_test, pipe_write) {
  process_options opts = process_options_default();
  opts.pipe_stdin = 1;
  opts.pipe_stdout = 1;

  cstr8 const args[] = {CAT_ARGS};

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  pipe pin = pipe_stdin(prc);
  EXPECT_NE(nullptr, pin);

  const c8 test_data[] = "test input data\n";
  sz bytes_written = pipe_write(pin, test_data, sizeof(test_data) - 1);

  EXPECT_GT(bytes_written, 0U);

  pipe_close(pin);

  process_wait(prc, 1, nullptr);
  process_destroy(prc);
}

TEST(processes_pipe_test, pipe_poll_readable) {
  process_options opts = process_options_default();
  opts.pipe_stdout = 1;

#if defined(_WIN32)
  cstr8 const args[] = {"cmd.exe", "/c", "echo test", nullptr};
#else
  cstr8 const args[] = {"/bin/echo", "test", nullptr};
#endif

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  pipe pout = pipe_stdout(prc);
  EXPECT_NE(nullptr, pout);

  b32 readable = pipe_poll_readable(pout, 1000);
  EXPECT_NE(0, readable);

  process_wait(prc, 1, nullptr);
  process_destroy(prc);
}

TEST(processes_pipe_test, pipe_close) {
  process_options opts = process_options_default();
  opts.pipe_stdin = 1;
  opts.pipe_stdout = 1;

  cstr8 const args[] = {CAT_ARGS};

  process prc = process_create_with(args, opts);
  EXPECT_NE(0, process_is_valid(prc));

  pipe pin = pipe_stdin(prc);
  EXPECT_NE(nullptr, pin);

  pipe_close(pin);

  i32 exit_code = -1;
  b32 waited = process_wait(prc, 1, &exit_code);
  EXPECT_NE(0, waited);

  process_destroy(prc);
}
