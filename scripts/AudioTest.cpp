// Testing code taken directly from ESP9266Audio library example "PlayWAVFromFunction.ino" with some mods
// Original source code here: https://github.com/earlephilhower/ESP8266Audio/blob/master/examples/PlayWAVFromFunction/PlayWAVFromFunction.ino

#include <Arduino.h>
#include "AudioFileSourceFunction.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "Constants.h"

float hz = 440.f;

// pre-defined function can also be used to generate the wave
float sine_wave(const float time) {
  float v = sin(TWO_PI * hz * time);  // C
  v *= fmod(time, 1.f);               // change linear
  v *= 0.5;                           // scale
  return v;
};

AudioGeneratorWAV* wav;
AudioFileSourceFunction* file;
AudioOutputI2S* out;

void setup() {
    Serial.begin(115200);
  // ===== create instance with length of song in [sec] =====
  file = new AudioFileSourceFunction(10.);
  file->addAudioGenerators(sine_wave);

  out = new AudioOutputI2S();
  out->SetPinout(BCLK_PIN,LRCLK_PIN,DIN_PIN);
  wav = new AudioGeneratorWAV();
  wav->begin(file, out);
}

void loop() {
  if (wav->isRunning()) {
    Serial.println("Running");
    if (!wav->loop()) 
    {
        wav->stop();
    }
  } else {
    Serial.println("function done!");
  }
}