// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

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