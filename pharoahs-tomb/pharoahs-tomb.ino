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
//analogs (defined post internal hidden conversion to facilitate use)
#define IN_LEFT_COL 54
#define IN_RIGHT_COL 55
#define IN_RA_1 56
#define IN_RA_2 57
#define IN_RA_3 58

#define IN_ANUBIS 30

#define RELAY_LIGHTS_MAIN 31
#define RELAY_LIGHTS_SPOT 32
#define RELAY_DOOR_COL_L 33
#define RELAY_DOOR_COL_R 34
#define RELAY_DOOR_EXIT 36


//common breakout and shield pins
#define BO_RST 9
#define BO_CS 10
#define BO_DCS 8

#define SH_RST -1
#define SH_CS 7
#define SH_DCS 6

//sd card chip select
#define SDCS 4
//dreq for background music playing
#define DREQ 3

Adafruit_VS1053_FilePlayer audioPlayer = Adafruit_VS1053_FilePlayer(BO_RST, BO_CS, BO_DCS, DREQ, SDCS);

RF24 radio(7,6);
const uint64_t node[] = {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL};

void setup() {
  Serial.begin(115200);
  if(DEBUG)Serial.println("Device Starting...");

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
  
  if (! audioPlayer.begin()){
    if(DEBUG)Serial.println(F("error: Audio Board FAILED"));
    while (1);
  }
  if(! SD.begin(SDCS)){
    if(DEBUG)Serial.println(F("error: SD Card FAILED"));
    while (1);
  }
  if(DEBUG)Serial.println(F("SD Present"));
  audioPlayer.setVolume(10,10);
  audioPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  audioPlayer.playFullFile("opentone.mp3");
  if(DEBUG)Serial.println(F("Audio Board Initialised"));


  if(DEBUG)Serial.println("RADIO Starting...");
  radio.begin();
  radio.setChannel(120);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  for (int i = 1; i<4 ; i++) radio.openReadingPipe(i,node[i]);
  radio.startListening();
  if(DEBUG)Serial.println("RADIO success...");

}

void loop() {

  int columns[] = {IN_LEFT_COL, IN_RIGHT_COL};
  int maglocks[] = {RELAY_DOOR_COL_L, RELAY_DOOR_COL_R};
  
  for(int i = 0; i<2; i++){
    if(DEBUG)Serial.println(i);
    int readings[] = {1,2,3,4};
    int readings_index = 0;
    int total = 0;
    while(1){
      readings[readings_index] = analogRead(columns[i]);
        for(int x = 0; x<4; x++){ 
        total = total + readings[x];
        if(DEBUG)Serial.print("total: ");if(DEBUG)Serial.println(total);
        if(DEBUG)Serial.print("readings[x]: ");if(DEBUG)Serial.println(readings[x]);
      }
      if(total/4 >= LDR_TARGET_COLUMN) break;
      if(readings_index >= 4){
        readings_index = 0;
      }else{
        if(DEBUG)Serial.print("readings_index: ");if(DEBUG)Serial.println(readings_index);
        readings_index++;
      }
      delay(250);
    }//end while
    delay(250);
    if(DEBUG)Serial.println("exit");
    if(DEBUG)Serial.print("magunlocked: ");
    if(DEBUG)Serial.println(maglocks[i]);
    digitalWrite(maglocks[i], HIGH); //OR LOW idk
    audioPlayer.playFullFile("column2.mp3");  
  }//end for

  //JAR LIFT FROM CUPBOARD. RA SETUP
  while(digitalRead(IN_ANUBIS) == LOW){ //just chill till anubis canopic is lifted
  delay(250);
  }
    audioPlayer.playFullFile("team.mp3");
    audioPlayer.playFullFile("column1.mp3");
    digitalWrite(RELAY_LIGHTS_MAIN,LOW);
    delay(4000);
    audioPlayer.playFullFile("laugh.mp3");
    digitalWrite(RELAY_LIGHTS_SPOT,HIGH);
  
  int accumulator[] = {0,0,0};
  int sensor_in[] = {IN_RA_1, IN_RA_2, IN_RA_3};    
  while(1){     //RA   
    delay(250);
    if(DEBUG)Serial.print("ra sequence.");
    for(int i=0; i<3; i++){
      if(DEBUG)Serial.print("ra "); if(DEBUG)Serial.print(i);if(DEBUG)Serial.println(" check.");
      if(! (sensor_in[i] == 0)){
        accumulator[i] = accumulator[i] + (analogRead(sensor_in[i]) - accumulator[i])/3;
        if(DEBUG)Serial.print("accumulator for sensor ");
        if(DEBUG)Serial.print(i);
        if(DEBUG)Serial.print(": ");
        if(DEBUG)Serial.println(accumulator[i]);
                       
        if(accumulator[i]>LDR_TARGET_RA){
          if(DEBUG)Serial.print("light: ");
          if(DEBUG)Serial.println(i);
          audioPlayer.playFullFile("light1.mp3");
          sensor_in[i]= 0;
        }//end if
      }//end if
    }//end for
    int active_sensors = 0;
    for(int i = 0; i < 3 ; i++) active_sensors = active_sensors + sensor_in[i];
    if(!active_sensors){
      if(DEBUG)Serial.print("out");
      break;
    }
  }//end while
  
  //RA COMPLETE ACTIONS 
  
  digitalWrite(RELAY_LIGHTS_SPOT,LOW);
  audioPlayer.playFullFile("laugh.mp3");
  digitalWrite(RELAY_LIGHTS_MAIN,HIGH);
  
  //  Sarcophogus
  int canopicRead = 0;
  int resetRoom = 0;
  while(resetRoom = 0){
    if(radio.available(0)){ //is this redundant?
      radio.read(&canopicRead, sizeof(canopicRead));
      Serial.println(canopicRead);
      if(canopicRead == 11){
        digitalWrite(RELAY_DOOR_EXIT,HIGH);
        audioPlayer.playFullFile("cup.mp3");
        Serial.println("Exit. countdown to loop begun");
        for(int i=180; i>0; i--){ 
          Serial.println(i);
          delay(1000);
        }
        // resetRoom = 1;
        break;
      }//end if
    }//end if
    delay(250);
  }//end while
}//end loop()
