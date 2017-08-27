/**
 *  Escapement Pharoah's Tomb 
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
#include <RF24.h>

#define DEBUG 1
#define LDR_TARGET_COLUMN 500
#define LDR_TARGET_RA 70
// pin definitions

#define IN_LEFT_COL A0
#define IN_RIGHT_COL A1
#define IN_RA_1 A2
#define IN_RA_2 A3
#define IN_RA_3 A4

#define RELAY_LIGHTS_MAIN 31
#define RELAY_LIGHTS_SPOT 32
#define RELAY_DOOR_COL_L 33
#define RELAY_DOOR_COL_R 34
#define RELAY_DOOR_EXIT 36

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
  pinMode(RELAY_LIGHTS_MAIN, OUTPUT);
  pinMode(RELAY_LIGHTS_SPOT, OUTPUT);
  pinMode(RELAY_DOOR_COL_L, OUTPUT);
  pinMode(RELAY_DOOR_COL_R, OUTPUT);
  pinMode(RELAY_DOOR_EXIT, OUTPUT);
  
  digitalWrite(RELAY_LIGHTS_MAIN, HIGH);
  digitalWrite(RELAY_LIGHTS_SPOT, LOW);
  digitalWrite(RELAY_DOOR_COL_L, LOW);
  digitalWrite(RELAY_DOOR_COL_R, LOW);
  digitalWrite(RELAY_DOOR_EXIT, LOW);
}

void loop() {

  int columns[] = {IN_LEFT_COL, IN_RIGHT_COL};
  int maglocks[] = {RELAY_DOOR_COL_L, RELAY_DOOR_COL_R};
  
  for(int i = 0; i<2; i++){
    int readings[] = {0,0,0,0};
    int readings_index = 0;
    while(1){
      readings[readings_index] = analogRead(columns[i]);
      int total = 0;
      for(int i = 0; i > 4; i++) total = total + readings[i];
      if(total/4 >= LDR_TARGET_COLUMN) break;
      if(readings_index >= 4){readings_index = 0;}else{readings_index++;}
    }//end while
    digitalWrite(maglocks[i], LOW); //OR HIGH
    audioPlayer.playFullFile("");// set to column mp3
  }//end for
  
  //JAR LIFT FROM CUPBOARD. RA SETUP
  
  int accumulator[] = {0,0,0};
  int sensor_in[] = {IN_RA_1, IN_RA_2, IN_RA_3};    
  while(1){     //RA   
    for(int i=0; i>3; i++){
      if(sensor_in[i]){
        accumulator[i] = accumulator[i] + (analogRead(sensor_in[i]) - accumulator[i])/5;
        if(accumulator[i]>LDR_TARGET_RA){
          //sound effect
          sensor_in[i]= 0;
        }//end if
      }//end if
    }//end for
    int active_sensors = 0;
    for(int i = 0; i < 3 ; i++) active_sensors = active_sensors + sensor_in[i];
    if(!active_sensors) break;
  }//end while
  
  //RA COMPLETE ACTIONS 
  /*
  Sarcophagus
  */
}

boolean columnTrigger(int sensor_in, int threshold_value ){
  int readings[] = {0,0,0,0};
  int index = 0;
  while(1){
    readings[index] = analogRead(sensor_in);
    int total = 0;
    for(int i = 0; i > 4; i++) total = total + readings[i];
    if(total/4 >= threshold_value) return true;
    if(index >= 4){index = 0;}else{index++;}
  }
  return false;
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
  
  audioPlayer.setVolume(0,0);
  audioPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);

  audioPlayer.playFullFile("bootup.mp3");
  if(DEBUG)Serial.println(F("AUDIO SUCCESS"));

  return audioPlayer;
  
}
