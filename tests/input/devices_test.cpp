// MIT License
// Copyright (c) 2026 Christian Luppi

#include "test_common.hpp"

TEST(input_devices_test, type_names_and_audio_id_encoding_roundtrip) {
  EXPECT_STREQ("keyboard", devices_get_type_name(DEVICE_TYPE_KEYBOARD));
  EXPECT_STREQ("unknown", devices_get_type_name(DEVICE_TYPE_UNKNOWN));
  EXPECT_STREQ("playback", devices_get_audio_type_name(AUDIO_DEVICE_TYPE_PLAYBACK));
  EXPECT_STREQ("recording", devices_get_audio_type_name(AUDIO_DEVICE_TYPE_RECORDING));

  device_id aud_id = devices_make_audio_device_id(77, AUDIO_DEVICE_TYPE_RECORDING);
  EXPECT_TRUE(device_id_is_valid(aud_id) != 0);
  EXPECT_EQ(DEVICE_TYPE_AUDIO, aud_id.type);
  EXPECT_EQ(AUDIO_DEVICE_TYPE_RECORDING, devices_get_audio_device_type(aud_id));
  EXPECT_EQ(77U, devices_get_audio_native_id(aud_id));

  EXPECT_TRUE(audio_device_type_is_valid(AUDIO_DEVICE_TYPE_UNKNOWN) == 0);
}

TEST(input_devices_test, invalid_ids_are_reported_as_disconnected) {
  device_id bad_id = {};
  EXPECT_TRUE(device_id_is_valid(bad_id) == 0);
  EXPECT_TRUE(devices_is_connected(bad_id) == 0);
  EXPECT_TRUE(devices_get_info(bad_id, nullptr) == 0);

  sz key_count = devices_get_count(DEVICE_TYPE_KEYBOARD);
  device_id out_id = {};
  if (key_count == 0) {
    EXPECT_TRUE(devices_get_device(DEVICE_TYPE_KEYBOARD, 0, &out_id) == 0);
  } else {
    EXPECT_TRUE(devices_get_device(DEVICE_TYPE_KEYBOARD, 0, &out_id) != 0);
    EXPECT_TRUE(device_id_is_valid(out_id) != 0);
  }
}
