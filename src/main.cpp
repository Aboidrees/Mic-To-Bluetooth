#include "AudioTools.h"
#include "BluetoothA2DPSource.h"
#include "arduinoFFT.h"

const uint16_t samples = 64; // This value MUST ALWAYS be a power of 2
const float samplingFrequency = 16000;
AnalogAudioStream adc;
BluetoothA2DPSource a2dp_source;
double real[samples];
double imag[samples];
ArduinoFFT<float> FFT = new ArduinoFFT<float>(real, imag, samples, samplingFrequency); /* Create FFT object */

const int up = 18;
const int down = 19;

int increasing = 0;
int decreasing = 0;

int volume = 120;

bool lock = false;

// Noise Reduction Function
void apply_fft_noise_reduction(int16_t *samples)
{
  // Step 1: Convert audio samples to double precision
  for (int i = 0; i < samples; i++)
  {
    real[i] = (double)samples[i];

    imag[i] = 0.0;
  }

  // Step 2: Perform FFT
  FFT.windowing(real, samples, FFT_WIN_HAMMING, FFT_FORWARD);
  FFT.compute(real, imag, samples, FFT_FORWARD);

  // Step 3: Apply Noise Reduction
  for (int i = 1; i < (samples / 2); i++)
  {
    double magnitude = sqrt(real[i] * real[i] + imag[i] * imag[i]);

    // Set threshold (adjust this for better filtering)
    if (magnitude < 50)
    {
      real[i] = 0;
      imag[i] = 0;
    }
  }

  // Step 4: Perform Inverse FFT to reconstruct audio
  FFT.compute(real, imag, samples, FFT_INVERSE);
  FFT.complexToMagnitude(real, imag, samples);

  // Step 5: Convert back to 16-bit integer
  for (int i = 0; i < samples; i++)
  {
    samples[i] = (int16_t)real[i];
  }
}

// callback used by A2DP to provide the sound data
int32_t get_sound_data(Frame *frames, int32_t frameCount)
{
  uint8_t *data = (uint8_t *)frames;
  int16_t sampleBuffer[samples];
  for (int i = 0; i < samples; i += 2)
  {
    sampleBuffer[i] = (data[i + 1] << 8) | data[i];
  }

  // Apply FFT-based noise reduction
  apply_fft_noise_reduction(sampleBuffer);

  // Convert back to bytes
  for (int i = 0; i < samples; i += 2)
  {
    data[i] = sampleBuffer[i] & 0xFF;
    data[i + 1] = (sampleBuffer[i] >> 8) & 0xFF;
  }
}

void setup(void)
{
  // Serial.begin(115200);
  // AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  // initialize the pushbutton pin as an input
  pinMode(up, INPUT);
  // initialize the LED pin as an output
  pinMode(down, INPUT);
  // Configure ADC for audio input
  auto config = adc.defaultConfig(RX_MODE);
  config.adc_pin = 34; // Specify the ADC pin (e.g., GPIO34)
  adc.begin(config);

  // Start Bluetooth A2DP Source
  a2dp_source.set_auto_reconnect(false);               // Enable auto-reconnect
  a2dp_source.set_volume(127);                         // Max volume
  a2dp_source.start("soundcore R50i", get_sound_data); // Stream to soundcore R50i
  // a2dp_source.start("LP5", get_sound_data); // Stream to LP5
  delay(200);
}

// Arduino loop - repeated processing
void loop()
{
  // Read the button states
  increasing = digitalRead(up);
  decreasing = digitalRead(down);

  // Handle increasing logic
  if (increasing == HIGH && lock == false)
  {
    volume += 10;                       // Increase the volume
    volume = constrain(volume, 0, 120); // Ensure volume stays within range
    lock = true;                        // Lock to prevent repeated increases
    // Serial.printf("Volume increased to: %d\n", volume);
  }
  else if (increasing == LOW && lock == true)
  {
    lock = false; // Unlock when the condition is no longer true
    // Serial.println("Lock reset after increasing.");
  }

  // Handle decreasing logic
  if (decreasing == HIGH && lock == false)
  {
    volume -= 10;                       // Decrease the volume
    volume = constrain(volume, 0, 120); // Ensure volume stays within range
    lock = true;                        // Lock to prevent repeated decreases
    // Serial.printf("Volume decreased to: %d\n", volume);
  }
  else if (decreasing == LOW && lock == true)
  {
    lock = false; // Unlock when the condition is no longer true
    // Serial.println("Lock reset after decreasing.");
  }

  // Update the Bluetooth volume
  a2dp_source.set_volume(volume);

  // Small delay to debounce button inputs
  delay(100); // Adjust the delay as needed
}