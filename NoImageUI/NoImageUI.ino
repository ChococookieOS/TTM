
#include <stdlib.h>
#include <SPI.h>
#include <SD.h>
#include "Ucglib.h"
#include "URTouch.h"


Ucglib_ILI9341_18x240x320_HWSPI ucg(/*cd=*/ 10, /*cs=*/ 9, /*reset=*/ 8);
//              TCLK,TCS,TDIN,TDO,TIRQ
URTouch myTouch(2, 3, 4, 5, 6);

enum screens {
  START,
  TIMESELECT,
  RUNNING,
  TIMEREMAINING
};

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

//
boolean projectRunning = false;

//Touch time
unsigned long touchTimePassed = 0;

//Dough time
unsigned long timePassed = 0;
int timeRemaining = 0;
//Screen
unsigned long screenPassed = 0;

screens currentScreen;

void drawStartScreen(void){
  //Init Screen
  currentScreen = START;
  
  ucg.setColor(100, 100, 100);
  ucg.setFont(ucg_font_fur42_tr);
  ucg.drawBox(0, 0, 320, 240);

  //Button Sauerteig
  ucg.setColor(180,180,180);
  ucg.drawBox(3, 3, SCREEN_WIDTH-6, SCREEN_HEIGHT/2-6);
  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(30,75);
  ucg.print("Sauerteig");

  //Button Hefeteig
  ucg.setColor(180,180,180);
  ucg.drawBox(3, SCREEN_HEIGHT/2+3, SCREEN_WIDTH-6, SCREEN_HEIGHT/2-6);
  ucg.setColor(255,255,255);
  ucg.setPrintPos(40,190);
  ucg.print("Hefeteig");
}

void drawTimeSelectScreen(void){
  //Init Screen
  currentScreen = TIMESELECT;

  ucg.setColor(100, 100, 100);
  ucg.setFont(ucg_font_fur42_tr);
  ucg.drawBox(0, 0, 320, 240);

   
  //Button 3h
  ucg.setColor(180,180,180);
  ucg.drawBox(3,3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2-6);
  ucg.setColor(255,255,255);
  ucg.setPrintPos(3+40,3+80);
  ucg.print("3h");
  //Button 6h 
  ucg.setColor(180,180,180);
  ucg.drawBox(SCREEN_WIDTH/2+3, 3, SCREEN_WIDTH/2-3,SCREEN_HEIGHT/2-6);
  ucg.setColor(255,255,255);
  ucg.setPrintPos(SCREEN_WIDTH/2+3+40, 3+80);
  ucg.print("6h");
  //Button 12h
  ucg.setColor(180,180,180);
  ucg.drawBox(3,SCREEN_HEIGHT/2+3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2-6);
  ucg.setColor(255,255,255);
  ucg.setPrintPos(3+25,SCREEN_HEIGHT/2+3+80);
  ucg.print("12h");
  //Button 24h
  ucg.setColor(180,180,180);
  ucg.drawBox(SCREEN_WIDTH/2+3, SCREEN_HEIGHT/2+3, SCREEN_WIDTH/2-3,SCREEN_HEIGHT/2-6);
  ucg.setColor(255,255,255);
  ucg.setPrintPos(SCREEN_WIDTH/2+3+30, SCREEN_HEIGHT/2+3+80);
  ucg.print("24h");
}

void drawRunningScreen(void){
    currentScreen = RUNNING;

    ucg.setColor(100, 100, 100);
    ucg.setFont(ucg_font_logisoso26_tr);
    ucg.drawBox(0, 0, 320, 240);

    //Button PLAY/PAUSE
    ucg.setColor(180, 180, 180); 
    ucg.drawBox(3,3,SCREEN_WIDTH-6,SCREEN_HEIGHT/2 -6);
    ucg.setColor(255,255,255);
    ucg.setPrintPos(3+50,3+65);
    ucg.print("pause/weiter");
    
    //Button STOP
    ucg.setColor(180, 180, 180); 
    ucg.drawBox(3,SCREEN_HEIGHT/2+3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2 -6);
    ucg.setColor(255,255,255);
    ucg.setPrintPos(35,180);
    ucg.print("STOP");

    //Button Manuell
    ucg.setColor(180, 180, 180); 
    ucg.drawBox(SCREEN_WIDTH/2+3,SCREEN_HEIGHT/2+3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2 -6);
    ucg.setColor(255,255,255);
    ucg.setPrintPos(180,190);
    ucg.print("manuell");
}

int hR;
int mR;
String outputString = "";
void drawTimeRemaining(int tR){

  currentScreen = TIMEREMAINING;
  
  outputString= "";
  ucg.setFont(ucg_font_logisoso62_tn);
  ucg.setColor(0,0,0);
  ucg.drawBox(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
  hR = tR / 60;
  Serial.println(hR);
  mR = tR - (hR * 60);
  Serial.println(mR);
  if(hR < 10){
    if(mR < 10){
     outputString = "0"+String(hR) + ":0" + String(mR);
    }
    else{
      outputString = "0"+String(hR) +":"+ String(mR);;
    }
  }
  else{
     if(mR < 10){
     outputString = String(hR) + ":0" + String(mR);
    }
    else{
      outputString = String(hR) +":"+ String(mR);;
    }
  }
 
  Serial.println(outputString);
  ucg.setColor(255,255,255);
  ucg.setPrintPos(70,140);
  
  
  ucg.print(outputString);
  
  
}

void passTime(){
  if(millis() - timePassed >= 60000){
      timePassed = millis();
      timeRemaining--;
    }
}

void setup(void) {
  //Screen
  delay(1000);
  Serial.begin(9600);
  
  //Serial.print(map(analogRead(A1), 0, 410, -50, 150));

  Serial.println("DrawScreen");
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.setRotate90();
  ucg.clearScreen();
  drawStartScreen();
  
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_HI);
  Serial.println("Init Touch");

}

int x = 0;
int y = 0;
unsigned long currentTime();

void loop(void)
{


  if(currentScreen == START){
    if(myTouch.dataAvailable()){
      drawTimeSelectScreen();
      delay(1500);
    }
  }
  else if(currentScreen == TIMESELECT){
    if(myTouch.dataAvailable()){
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      if(x < SCREEN_WIDTH/2){
        if(y < SCREEN_HEIGHT/2){
          //3H
          timeRemaining = 3 * 60;
          pinMode(19, HIGH);
        }
        else{
          //12H
          timeRemaining = 12 * 60;
        }
      }
      else{
        if(y < SCREEN_HEIGHT/2){
          //6H
          timeRemaining = 6 * 60;
        }
        else{
          //12H
          timeRemaining = 24 * 60;
        }
      }
      drawRunningScreen();
      projectRunning = true;
      delay(1500);
      touchTimePassed = millis();
      timePassed = millis();
    }
    
    
  }
  else if(currentScreen == RUNNING){
    if(millis() - touchTimePassed >= 15000){

      //Aufruf timeRemaining Screen
      drawTimeRemaining(timeRemaining);
      delay(1500);
      screenPassed = millis();
      //
    }
    //Reset des counters when screen touched
    if(myTouch.dataAvailable()){
      myTouch.read();
      touchTimePassed = millis();
    }
    //
  }
  else if(currentScreen == TIMEREMAINING){
    //Check if time for update
    if(millis() - screenPassed >= 30000){
      screenPassed = millis();
      drawTimeRemaining(timeRemaining);
    }
    //
    //Check if touched -> return to running screen
    if(myTouch.dataAvailable()){
        myTouch.read();
        drawRunningScreen();
        touchTimePassed = millis();
    }
    
  }
  if(projectRunning){
    passTime();
  }
  
  
}

