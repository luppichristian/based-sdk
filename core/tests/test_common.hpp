// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

// Tracy cant work with GoogleTest
#define BASED_PROFILER_ENABLED 0

#include <gtest/gtest.h>

#define ENTRY_TYPE_RUN
#include <based_core.h>