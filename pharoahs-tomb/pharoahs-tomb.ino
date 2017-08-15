/**
 *  Escapement VS1053 MP3 player logic free base 
 * 
 * 
 * Copyright (c) 2017, The Escapement
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are not permitted.
 *
 */

// libraries used: SPI, SD, VS1053 audio
#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

#define DEBUG 1
// pin definitions

//common breakout and shield pins
#define BO_RST 9
#define BO_CS 10
#define BO_DCS 8

//#define SH_RST -1
//#define SH_CS 7
//#define SH_DCS 6

//sd card chip select
#define SDCS 4
//dreq for background music playing
#define DREQ 3

void setup() {
  Serial.begin(9600);
  if(DEBUG)Serial.println("Device Starting...");
  Adafruit_VS1053_FilePlayer audioPlayer = audioSetup(BO_RST, BO_CS, BO_DCS);
  audioPlayer.startPlayingFile("bootup.mp3");
  
}

void loop() {

}


Adafruit_VS1053_FilePlayer audioSetup(int rst_in, int cs_in, int dcs_in) {

  //AUDIO BOARD SETUP:
  
  Adafruit_VS1053_FilePlayer audioPlayer = Adafruit_VS1053_FilePlayer(rst_in, cs_in, dcs_in, DREQ, SDCS);

  //check status of audio board and sd card:

  if (! audioPlayer.begin()){
    if(DEBUG)Serial.println(F("error: Audio Board FAILED"));
    while (1);
  }
  if(DEBUG)Serial.println(F("Audio Board Initialised"));

  if(! SD.begin(SDCS)){
    if(DEBUG)Serial.println(F("error: SD Card FAILED"));
    while (1);
  }
  if(DEBUG)Serial.println(F("SD Present"));
  
  audioPlayer.setVolume(20,20);
  audioPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);

  audioPlayer.playFullFile("bootup.mp3");
  if(DEBUG)Serial.println(F("AUDIO SUCCESS"));

  return audioPlayer;
  
}
