
#include "AudioTools.h"
#include "BluetoothA2DPSource.h"

AnalogAudioStream adc;
BluetoothA2DPSource a2dp_source;

// callback used by A2DP to provide the sound data
int32_t get_sound_data(Frame *frames, int32_t frameCount)
{
  uint8_t *data = (uint8_t *)frames;
  int frameSize = 4;
  size_t resultBytes = adc.readBytes(data, frameCount * frameSize);
  return resultBytes / frameSize;
}

void setup(void)
{
  Serial.begin(115200);

  adc.begin(adc.defaultConfig(RX_MODE));

  // start the bluetooth

  a2dp_source.set_auto_reconnect(false);
  a2dp_source.set_volume(50);
  a2dp_source.start("LP5", get_sound_data);
}

// Arduino loop - repeated processing
void loop()
{
  delay(1000);
}