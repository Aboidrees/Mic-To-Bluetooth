// #include "AudioTools.h"
// #include "BluetoothA2DPSource.h"
// #include "arduinoFFT.h"

// #define SAMPLES 64               // Must be a power of 2
// #define SAMPLING_FREQUENCY 16000 // Adjust based on microphone sample rate

// AnalogAudioStream adc;
// BluetoothA2DPSource a2dp_source;
// double real[SAMPLES];
// double imag[SAMPLES];

// ArduinoFFT<float> FFT = ArduinoFFT::ArduinoFFT<float>(real, imag, SAMPLES, SAMPLING_FREQUENCY); /* Create FFT object */

// const int up = 18;
// const int down = 19;

// int increasing = 0;
// int decreasing = 0;

// int volume = 120;

// bool lock = false;

// // Noise Reduction Function
// void apply_fft_noise_reduction(int16_t *audioSamples)
// {
//   // Step 1: Convert audio samples to double precision
//   for (int i = 0; i < SAMPLES; i++)
//   {
//     real[i] = (double)audioSamples[i];
//     imag[i] = 0.0;
//   }

//   // Step 2: Perform FFT
//   FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
//   FFT.compute(FFTDirection::Forward);

//   // Step 3: Apply Noise Reduction
//   for (int i = 1; i < (SAMPLES / 2); i++)
//   {
//     double magnitude = sqrt(real[i] * real[i] + imag[i] * imag[i]);

//     // Set threshold (adjust this for better filtering)
//     if (magnitude < 50)
//     {
//       real[i] = 0;
//       imag[i] = 0;
//     }
//   }

//   // Step 4: Perform Inverse FFT to reconstruct audio
//   FFT.compute(FFTDirection::Reverse);
//   FFT.complexToMagnitude();
//   // Step 5: Convert back to 16-bit integer
//   for (int i = 0; i < SAMPLES; i++)
//   {
//     audioSamples[i] = (int16_t)real[i];
//   }
// }

// // callback used by A2DP to provide the sound data
// int32_t get_sound_data(Frame *frames, int32_t frameCount)
// {
//   uint8_t *data = (uint8_t *)frames;
//   int16_t sampleBuffer[SAMPLES];
//   for (int i = 0; i < SAMPLES; i += 2)
//   {
//     sampleBuffer[i] = (data[i + 1] << 8) | data[i];
//   }

//   // Apply FFT-based noise reduction
//   apply_fft_noise_reduction(sampleBuffer);

//   // Convert back to bytes
//   for (int i = 0; i < SAMPLES; i += 2)
//   {
//     data[i] = sampleBuffer[i] & 0xFF;
//     data[i + 1] = (sampleBuffer[i] >> 8) & 0xFF;
//   }
// }

// void setup(void)
// {
//   // Serial.begin(115200);
//   // AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

//   // initialize the pushbutton pin as an input
//   pinMode(up, INPUT);
//   // initialize the LED pin as an output
//   pinMode(down, INPUT);
//   // Configure ADC for audio input
//   auto config = adc.defaultConfig(RX_MODE);
//   config.adc_pin = 34; // Specify the ADC pin (e.g., GPIO34)
//   adc.begin(config);

//   // Start Bluetooth A2DP Source
//   a2dp_source.set_auto_reconnect(false);               // Enable auto-reconnect
//   a2dp_source.set_volume(127);                         // Max volume
//   a2dp_source.start("soundcore R50i", get_sound_data); // Stream to soundcore R50i
//   // a2dp_source.start("LP5", get_sound_data); // Stream to LP5
//   delay(200);
// }

// // Arduino loop - repeated processing
// void loop()
// {
//   // Read the button states
//   increasing = digitalRead(up);
//   decreasing = digitalRead(down);

//   // Handle increasing logic
//   if (increasing == HIGH && lock == false)
//   {
//     volume += 10;                       // Increase the volume
//     volume = constrain(volume, 0, 120); // Ensure volume stays within range
//     lock = true;                        // Lock to prevent repeated increases
//     // Serial.printf("Volume increased to: %d\n", volume);
//   }
//   else if (increasing == LOW && lock == true)
//   {
//     lock = false; // Unlock when the condition is no longer true
//     // Serial.println("Lock reset after increasing.");
//   }

//   // Handle decreasing logic
//   if (decreasing == HIGH && lock == false)
//   {
//     volume -= 10;                       // Decrease the volume
//     volume = constrain(volume, 0, 120); // Ensure volume stays within range
//     lock = true;                        // Lock to prevent repeated decreases
//     // Serial.printf("Volume decreased to: %d\n", volume);
//   }
//   else if (decreasing == LOW && lock == true)
//   {
//     lock = false; // Unlock when the condition is no longer true
//     // Serial.println("Lock reset after decreasing.");
//   }

//   // Update the Bluetooth volume
//   a2dp_source.set_volume(volume);

//   // Small delay to debounce button inputs
//   delay(100); // Adjust the delay as needed
// }
