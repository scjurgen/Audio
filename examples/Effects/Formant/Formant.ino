// 3 stage Formant using biquad peak equalizer
// example by juergen schwietering
// public domain

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=150,310
AudioMixer4              mixer1;         //xy=300,310
AudioFilterBiquad        biquad1;        //xy=450,310
AudioOutputI2S           i2s2;           //xy=600,310
AudioConnection          patchCord1(i2s1, 0, mixer1, 0);
AudioConnection          patchCord2(i2s1, 1, mixer1, 1);
AudioConnection          patchCord3(mixer1, biquad1);
AudioConnection          patchCord4(biquad1, 0, i2s2, 0);
AudioConnection          patchCord5(biquad1, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;
// GUItool: end automatically generated code

float formantTableMen[9][3] =
{
    {270,2300,3000},
    {400,2000,2550},
    {530,1850,2500},
    {660,1700,2400},
    {730,1100,2450},
    {570, 850,2400},
    {440,1000,2250},
    {300, 850,2250},
    {640,1200,2400},
    {490,1350,1700},
};


class Formant
{
public:
  Formant()
          : m_qFactor(1.0)
          , m_gain({6,4,4})
          , m_adjustSize(1.0)
          , m_lastIndex(0)
  {
  }

  void setQFactor(float factor)
  {
    m_qFactor = factor;
  }

  void setGains(float gainMain, float gainSecond, float gainHigh)
  {
    m_gain[0] = gainMain;
    m_gain[1] = gainSecond;
    m_gain[2] = gainHigh;
  }

  void setFormantIndex(int8_t index)
  {
    m_lastIndex = index;
  }

    void update()
    {
    AudioNoInterrupts();
    for (uint32_t i = 0; i < 3; i++)
    {
      float f = formantTableMen[index][i] * m_adjustSize;
      biquad1.setPeakEQ(i, f, m_gain[i], m_qFactor);
    }
    AudioInterrupts();
    }

  void setMale()
  {
    m_adjustSize = 1.0;
  }
  // the following factors are VERY rough, actually they vary depending on vowel
  void setFemale()
  {
    m_adjustSize = 1.16;
  }
  void setChild()
  {
    m_adjustSize = 1.34;
  }
  // dragons, trolls and autobots
  void setOptimusPrime()
  {
    m_adjustSize = 0.5;
  }
private:
  float m_qFactor;
  float m_gain[3];
  float m_adjustSize;
  int m_lastIndex;
};

Formant formant;

void setup()
{
  AudioNoInterrupts();
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.75);
  sgtl5000_1.lineInLevel(9);
  AudioInterrupts();
  Serial.println("all set");
}

unsigned long m_next = 0;

void loop()
{
    if (millis() >= m_next)
    {
        m_next=millis()+200;
        formant.setFormantIndex(rand() % 9);
        formant.update();
    }
}
