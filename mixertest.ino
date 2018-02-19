/* Notes:
 *  This INO sketch was created to test the classes for playing a WAV-Sample, 
 *  Mix different samples into one main-output and hand it over to an
 *  I2S-DAC.
 *  Finally after playing some samples in an unspecified loop the system 
 *  should stop playing new samples and should switch off the output without to much noise.
 *  I have tested to play 4 sounds out of progmem at the same time without any bigger issues.
 *  
 *  The purpose is to use this to create Your own homegrown music-sampleplayer, loop-playback, 
 *  e-drum, e-percussion, sound-art-installation or 
 *  an midi-instrument.
 *  
 *  I wanted to get the library to the point to play 4 sounds indipendently like a drummer could do.
 *  the sounds were sampled from an softsynth by myself. I used only 2 sounds, one short and one with a longer duration 
 *  to be able to test overlapping durations.
 *  
 *  Erich Heinemann
 *  
 *  modified to test spiffs and AudiogeneratorWAV
 *  DAgnall
 */


#include <Arduino.h>

#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include "AudioFileSourcePROGMEM.h"
#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorWAV.h"
//#include "AudioGeneratorSAMPLE.h"
#include "AudioOutputI2SNoDAC.h"
// #include "AudioOutputI2S.h"
#include "AudioMixerOutBuffer.h"
#include "AudioMixerInBuffer.h"


// Sample, 16 Bit, Mono, Midi c4 - Note 60
//#include "angels.h"
//#include "arabicbass.h"


AudioGeneratorWAV *wav1;
AudioGeneratorWAV *wav2;
AudioGeneratorWAV *wav3;
AudioGeneratorWAV *wav4;

AudioFileSourceSPIFFS *file1;
AudioFileSourceSPIFFS *file2;
AudioFileSourceSPIFFS *file3;
AudioFileSourceSPIFFS *file4;

AudioOutputI2SNoDAC *out;
// AudioOutputI2S *out2;

AudioMixerInBuffer *channel1;
AudioMixerInBuffer *channel2;
AudioMixerInBuffer *channel3;
AudioMixerInBuffer *channel4;

AudioMixerOutBuffer *mainOut;
  int soundplays = 0;
 
void setup()
{
  soundplays = 0;
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);
  delay(1000);
  file1 = new AudioFileSourceSPIFFS("/initiated.wav");
   file2 = new AudioFileSourceSPIFFS("/F1.wav");
 file3 = new AudioFileSourceSPIFFS("/F2.wav");
  file4 = new AudioFileSourceSPIFFS("/F3.wav");
  
   out = new AudioOutputI2SNoDAC();
  // Adjust Volume of Output
  out->SetGain(0.2);

  
 wav1 = new AudioGeneratorWAV();
 wav2 = new AudioGeneratorWAV();   
 wav3 = new AudioGeneratorWAV();   
 wav4 = new AudioGeneratorWAV();   
 
  
  Serial.println("Define MainOut of Mixer");
  // new with Mixer, connect mainOut to the output
  mainOut = new AudioMixerOutBuffer( 2, out );
  
  mainOut->SetRate(11025);
  mainOut->SetChannels(2); // 2=Stereo, 1=Mono
  mainOut->SetBitsPerSample(16); // 16 Bit

  Serial.println("Begin MainOut");
  mainOut->begin();

  // define channel1 of the Mixer and strip it to mainOut as Strip 1
  // defined with buffersize==8, sink==mainout, channelnumber==1
  Serial.println("Define Channel1 of Mixer");
  channel1 = new AudioMixerInBuffer( 8, mainOut, 1 );

  // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel2 of Mixer");  
  channel2 = new AudioMixerInBuffer( 8, mainOut, 2 );
  channel2->begin();

  Serial.println("Define Channel3 of Mixer");  
  channel3 = new AudioMixerInBuffer( 8, mainOut, 3 );
  channel3->begin();
  
  Serial.println("Define Channel4 of Mixer");  
  channel4 = new AudioMixerInBuffer( 8, mainOut, 4 );
  channel4->begin();

  Serial.println("Begin Wav1");
  wav1->begin(file1, channel1);
  wav2->begin(file2, channel2);
  wav3->begin(file3, channel3);
  wav4->begin(file4, channel4);
 
}

void loop()
{
  if (wav1->isRunning()  ) {
    if (!wav1->loop() ) {
      soundplays =soundplays + 1;
      Serial.println(" play +"); 
      wav1->stop(); 
      channel1->stop();
      yield();
      if ( soundplays < 40 ){
       file1 = new AudioFileSourceSPIFFS("/initiated.wav");
        wav1->begin(file1, channel1);
      }  
    }  
  } 

  if (wav2->isRunning()  ) {
    if (!wav2->loop() ) {
      soundplays =soundplays + 1;Serial.println(" play +"); 
      wav2->stop(); 
      channel2->stop();
      yield();
      if ( soundplays < 40 ){
        file2 = new AudioFileSourceSPIFFS("/F1.wav");
        wav2->begin(file2, channel2);
      }  
    }
  } 


  if (wav3->isRunning()   ) {
    if (!wav3->loop() ) {
      soundplays =soundplays + 1;Serial.println(" play +"); 
      wav3->stop(); 
      channel3->stop();
      yield();
      if ( soundplays < 40 ){
         file3 = new AudioFileSourceSPIFFS("/F2.wav");
        wav3->begin(file3, channel3);
        }  
    }
  } 

  if (wav4->isRunning()  ) {
    if (!wav4->loop() ) {
      soundplays =soundplays + 1;Serial.println(" play +"); 
      wav4->stop(); 
      channel4->stop();
      yield();
      if ( soundplays < 40 ){
        file4 = new AudioFileSourceSPIFFS("/F3.wav");
        wav4->begin(file4, channel4);
      }  
    }
  } 



  if ( soundplays > 50 ){
    // Stop
    soundplays = 0;
    }

/*  
  // Stop the Output if all is done ..
  if ( soundplays >=20 && soundplays < 27) {
    out->stop();
  //   out2->stop();
  }
  */
  
}
