#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// 8 stage phaser

// GUItool: begin automatically generated code
AudioInputI2S i2s1;    // xy=57,122
AudioEffectDelay delay1;  // xy=290,220
AudioFilterBiquad biquad1; // xy=506,222
AudioFilterBiquad biquad2; // xy=507,288
AudioMixer4 mixer1;  // xy=683,203
AudioOutputI2S i2s2;    // xy=827,344
AudioConnection patchCord1(i2s1, 1, delay1, 0);
AudioConnection patchCord2(delay1, 0, mixer1, 0);
AudioConnection patchCord3(delay1, 1, biquad1, 0);
AudioConnection patchCord4(biquad1, biquad2);
AudioConnection patchCord7(biquad2, 0, mixer1, 1);
AudioConnection patchCord8(mixer1, 0, i2s2, 0);
AudioConnection patchCord9(mixer1, 0, i2s2, 1);
AudioControlSGTL5000 sgtl5000_1;
// GUItool: end automatically generated code

class Phaser
{
public:
  Phaser()
          : m_qFactor(1.0)
            , m_f(100.0)
            , m_direction(10.0)
            , m_minF(200.0)
            , m_maxF(1000.0)
            , m_updateInMsecs(10)
  {
    m_nextUpdate = millis();
  }

  // triangle wave from minF to maxF in updateInMsecs steps
  // obviously it would be nice to use a wave generator
  void tick()
  {
    if (millis() >= m_nextUpdate)
    {
      m_nextUpdate = millis() + m_updateInMsecs;
      newParams(f);
      if (direction >= 0)
      {
        f += direction;
        if (f >= m_maxF) direction = -direction;
      } else
      {
        f += direction;
        if (f <= m_minF) direction = -direction;
      }
    }
  }

  void setLFOFrequency(double f)
  {
    if (f == 0)
    {
      direction = 0;
      return;
    }
    direction = (2 * (maxF - minF))(1000.0 / m_updateInMsecs * f);
  }

private:
  void newParams(float new_f)
  {
    for (uint32_t i = 0; i < 4; i++)
    {
      AudioNoInterrupts();
      biquad1.setAllPass(i, new_f, m_qFactor);
      biquad2.setAllPass(i, new_f, m_qFactor);
      AudioInterrupts();
    }
  }

  float m_qFactor;
  float m_f;
  float m_direction;
  float m_minF;
  float m_maxF;
  unsigned long m_updateInMsecs;
  unsigned long m_nextUpdate;
};

Phaser phaser;

void setup()
{
  AudioNoInterrupts();
  AudioMemory(60);

  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.75);
  sgtl5000_1.lineInLevel(9);
  delay1.delay(0, 0);
  delay1.delay(1, 20);
  AudioInterrupts();
  phaser.tick();
  Serial.println("all set");
}

void analyseAudioUsage()
{
  static unsigned long nextAnalysis = 0;
  if (millis() >= nextAnalysis)
  {
    nextAnalysis += 10000;
    Serial.print("AudioProcessorUsage: ");
    Serial.print(round(AudioProcessorUsage()));
    Serial.print("% (max=");
    Serial.print(round(AudioProcessorUsageMax()));
    Serial.print("%)    ");
    Serial.print("AudioMemoryUsage: ");
    Serial.print(round(AudioMemoryUsage()));
    Serial.print("%, (max=");
    Serial.print(round(AudioMemoryUsageMax()));
    Serial.println("%)");
    AudioProcessorUsageMaxReset();
  }
}

void loop()
{
  phaser.tick();
  analyseAudioUsage();
}
