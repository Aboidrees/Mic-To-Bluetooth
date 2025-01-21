#include "AudioTools.h"
#include "BluetoothA2DPSource.h"

AnalogAudioStream adc;
BluetoothA2DPSource a2dp_source;
const int BUFFER_SIZE = 64;
float previous_samples[BUFFER_SIZE] = {0};
int buffer_index = 0;

// Function to apply noise reduction to a single sample
int16_t apply_noise_reduction(int16_t sample)
{
  float filtered_sample = (float)sample;
  for (int j = 0; j < BUFFER_SIZE; j++)
  {
    filtered_sample += (float)previous_samples[j];
  }
  filtered_sample /= (BUFFER_SIZE + 1);

  // Update buffer
  previous_samples[buffer_index] = sample;
  buffer_index = (buffer_index + 1) % BUFFER_SIZE;

  return (int16_t)filtered_sample;
}

// callback used by A2DP to provide the sound data
int32_t get_sound_data(Frame *frames, int32_t frameCount)
{
  uint8_t *data = (uint8_t *)frames;
  int frameSize = 4;
  size_t resultBytes = adc.readBytes(data, frameCount * frameSize);

  // Apply noise reduction
  for (int i = 0; i < resultBytes; i += 2)
  { // Process 16-bit samples
    // Convert bytes to 16-bit sample
    int16_t sample = (data[i + 1] << 8) | data[i];

    // Apply noise reduction
    sample = apply_noise_reduction(sample);

    // Convert back to bytes
    data[i] = sample & 0xFF;
    data[i + 1] = (sample >> 8) & 0xFF;
  }
  return resultBytes / frameSize;
}

void setup(void)
{
  Serial.begin(115200);
  // Configure ADC for audio input
  auto config = adc.defaultConfig(RX_MODE);
  config.adc_pin = 34; // Specify the ADC pin (e.g., GPIO34)
  adc.begin(config);

  // Start Bluetooth A2DP Source
  a2dp_source.set_auto_reconnect(false);    // Enable auto-reconnect
  a2dp_source.set_volume(127);              // Max volume
  a2dp_source.start("LP5", get_sound_data); // Stream to LP5
  Serial.println("Bluetooth A2DP streaming started.");
}

// Arduino loop - repeated processing
void loop()
{
  delay(1000);
}