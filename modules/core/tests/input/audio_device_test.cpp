// MIT License
// Copyright (c) 2026 Christian Luppi

#include "internal.h"
#include "test_common.hpp"

TEST(input_audio_device_test, id_conversion_type_and_generic_conversion_roundtrip) {
  audio_device aud_id = audio_device_from_native_id(77, AUDIO_DEVICE_TYPE_RECORDING);
  EXPECT_TRUE(audio_device_is_valid(aud_id) != 0);
  EXPECT_EQ((up)77, audio_device_to_native_id(aud_id));
  EXPECT_EQ(AUDIO_DEVICE_TYPE_RECORDING, audio_device_get_type(aud_id));
  EXPECT_STREQ("playback", audio_device_get_type_name(AUDIO_DEVICE_TYPE_PLAYBACK));
  EXPECT_STREQ("recording", audio_device_get_type_name(AUDIO_DEVICE_TYPE_RECORDING));
  EXPECT_TRUE(audio_device_type_is_valid(AUDIO_DEVICE_TYPE_UNKNOWN) == 0);

  device dev_id = devices_make_audio_device(77, AUDIO_DEVICE_TYPE_RECORDING);
  ASSERT_TRUE(device_is_valid(dev_id) != 0);
  audio_device from_dev = device_get_audio_device(dev_id);
  EXPECT_TRUE(audio_device_is_valid(from_dev) != 0);
  EXPECT_EQ((up)77, audio_device_to_native_id(from_dev));
  EXPECT_EQ(AUDIO_DEVICE_TYPE_RECORDING, audio_device_get_type(from_dev));
}

TEST(input_audio_device_test, enumeration_consistency_when_devices_exist) {
  audio_device out_id = NULL;
  sz playback_count = audio_device_get_total_count(AUDIO_DEVICE_TYPE_PLAYBACK);
  if (playback_count > 0) {
    out_id = audio_device_get_from_idx(AUDIO_DEVICE_TYPE_PLAYBACK, 0);
    ASSERT_TRUE(out_id != NULL);
    EXPECT_TRUE(audio_device_is_valid(out_id) != 0);
    EXPECT_EQ(AUDIO_DEVICE_TYPE_PLAYBACK, audio_device_get_type(out_id));
  }

  out_id = NULL;
  sz recording_count = audio_device_get_total_count(AUDIO_DEVICE_TYPE_RECORDING);
  if (recording_count > 0) {
    out_id = audio_device_get_from_idx(AUDIO_DEVICE_TYPE_RECORDING, 0);
    ASSERT_TRUE(out_id != NULL);
    EXPECT_TRUE(audio_device_is_valid(out_id) != 0);
    EXPECT_EQ(AUDIO_DEVICE_TYPE_RECORDING, audio_device_get_type(out_id));
  }
}
