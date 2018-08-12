#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=57,122
AudioEffectDelay         delay1;         //xy=290,220
AudioFilterBiquad        biquad1;        //xy=506,222
AudioFilterBiquad        biquad2;        //xy=507,288
AudioFilterBiquad        biquad3;        //xy=508,347
AudioFilterBiquad        biquad4;        //xy=510,408
AudioMixer4              mixer1;         //xy=683,203
AudioOutputI2S           i2s2;           //xy=827,344
AudioConnection          patchCord1(i2s1, 1, delay1, 0);
AudioConnection          patchCord2(delay1, 0, mixer1, 0);
AudioConnection          patchCord3(delay1, 1, biquad1, 0);
AudioConnection          patchCord4(biquad1, biquad4);
//AudioConnection          patchCord5(biquad2, biquad3);
//AudioConnection          patchCord6(biquad3, biquad4);
AudioConnection          patchCord7(biquad4, 0, mixer1, 1);
AudioConnection          patchCord8(mixer1, 0, i2s2, 0);
AudioConnection          patchCord9(mixer1, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_LINEIN;

void setup() {
    AudioNoInterrupts();
    AudioMemory(60);

    // Enable the audio shield, select input, and enable output
    sgtl5000_1.enable();
    sgtl5000_1.inputSelect(myInput);
    sgtl5000_1.volume(0.75);
    sgtl5000_1.lineInLevel(9);
    for (int i=0; i < 4; i++)
    {
        biquad1.setAllPass(i, 200, 4.0);
        biquad2.setAllPass(i, 200, 4.0);
        biquad3.setAllPass(i, 200, 4.0);
        biquad4.setAllPass(i, 200, 4.0);
    }
    delay1.delay(0,0);
    delay1.delay(1,20);
    AudioInterrupts();
}

elapsedMillis volmsec=0;

unsigned long nextT;
unsigned long nextAnalysis;

void newParams(float new_f)
{
    for (uint32_t i=0; i < 4; i++)
    {
        AudioNoInterrupts();
        biquad1.setAllPass(i, new_f, 1.0);
        AudioInterrupts();
        AudioNoInterrupts();
        biquad2.setAllPass(i, new_f+30+i*4, 1.0);
        AudioInterrupts();
        AudioNoInterrupts();
        biquad3.setAllPass(i, new_f+10+i*8, 1.0);
        AudioInterrupts();
        AudioNoInterrupts();
        biquad4.setAllPass(i, new_f+20+i*7, 1.0);
        AudioInterrupts();
    }
}

int f = 100;
int direction = 10;
void loop() {

    if (millis() > nextT)
    {
        nextT = millis()+10;
        newParams(f);
        if (direction > 0)
        {
            f += direction;
            if (f > 1000)
                direction = -direction;
        }
        else
        {
            f += direction;
            if (f < 200)
                direction = -direction;
        }
    }
    if (millis()  >= nextAnalysis)
    {
        nextAnalysis += 10000;
        Serial.print("all=");
        Serial.print(AudioProcessorUsage());
        Serial.print(",");
        Serial.print(AudioProcessorUsageMax());
        Serial.print("    ");
        Serial.print("Memory: ");
        Serial.print(AudioMemoryUsage());
        Serial.print(",");
        Serial.print(AudioMemoryUsageMax());
        Serial.println();
        AudioProcessorUsageMaxReset();
    }
}



