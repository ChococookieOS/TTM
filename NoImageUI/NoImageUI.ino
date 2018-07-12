#include <SPI.h>
#include "Ucglib.h"
#include "URTouch.h"


const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

//Full pinout can be seen at: URL

const byte LCD_CD = 10;
const byte LCD_CS = 9;
const byte LCD_RESET = 8;

const byte T_CLK = 2;
const byte T_CS = 3;
const byte T_DIN = 4;
const byte T_DO = 5;
const byte T_IRQ = 6;

const byte RELAIS_MOTOR = 18;
const byte RELAIS_HEAT = 0;
const byte RELAIS_FAN = 0;
const byte RELAIS_LED = 0;

Ucglib_ILI9341_18x240x320_HWSPI display(LCD_CD, LCD_CS, LCD_RESET);
URTouch touchscreen(T_CLK, T_CS, T_DIN, T_DO, T_IRQ);


//Just more convienend variable names
const unsigned char* FONT62 = ucg_font_logisoso62_tn;
const unsigned char* FONT42 = ucg_font_fur42_tr;
const unsigned char* FONT26 = ucg_font_logisoso26_tr;

//Each enum represents one screen
//Every screen is a set of render instructions combined into one function
//A screen is in this case compareable to a scene in other graphical languages
enum screens {
	START,
 	TIMESELECT,
  	RUNNING,
  	TIMEREMAINING
};



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
  
  display.setColor(100, 100, 100);
  display.setFont(FONT42);
  display.drawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  //Button Sauerteig
  display.setColor(180,180,180);
  display.drawBox(3, 3, SCREEN_WIDTH-6, SCREEN_HEIGHT/2-6);
  display.setColor(255, 255, 255);
  display.setPrintPos(30,75);
  display.print("Sauerteig");

  //Button Hefeteig
  display.setColor(180,180,180);
  display.drawBox(3, SCREEN_HEIGHT/2+3, SCREEN_WIDTH-6, SCREEN_HEIGHT/2-6);
  display.setColor(255,255,255);
  display.setPrintPos(40,190);
  display.print("Hefeteig");
}

void drawTimeSelectScreen(void){
  //Init Screen
  currentScreen = TIMESELECT;

  display.setColor(100, 100, 100);
  display.setFont(ucg_font_fur42_tr);
  display.drawBox(0, 0, 320, 240);

   
  //Button 3h
  display.setColor(180,180,180);
  display.drawBox(3,3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2-6);
  display.setColor(255,255,255);
  display.setPrintPos(3+40,3+80);
  display.print("3h");
  //Button 6h 
  display.setColor(180,180,180);
  display.drawBox(SCREEN_WIDTH/2+3, 3, SCREEN_WIDTH/2-3,SCREEN_HEIGHT/2-6);
  display.setColor(255,255,255);
  display.setPrintPos(SCREEN_WIDTH/2+3+40, 3+80);
  display.print("6h");
  //Button 12h
  display.setColor(180,180,180);
  display.drawBox(3,SCREEN_HEIGHT/2+3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2-6);
  display.setColor(255,255,255);
  display.setPrintPos(3+25,SCREEN_HEIGHT/2+3+80);
  display.print("12h");
  //Button 24h
  display.setColor(180,180,180);
  display.drawBox(SCREEN_WIDTH/2+3, SCREEN_HEIGHT/2+3, SCREEN_WIDTH/2-3,SCREEN_HEIGHT/2-6);
  display.setColor(255,255,255);
  display.setPrintPos(SCREEN_WIDTH/2+3+30, SCREEN_HEIGHT/2+3+80);
  display.print("24h");
}

void drawRunningScreen(void){
    currentScreen = RUNNING;

    display.setColor(100, 100, 100);
    display.setFont(FONT26);
    display.drawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    //Button PLAY/PAUSE
    display.setColor(180, 180, 180); 
    display.drawBox(3,3,SCREEN_WIDTH-6,SCREEN_HEIGHT/2 -6);
    display.setColor(255,255,255);
    display.setPrintPos(3+50,3+65);
    display.print("pause/weiter");
    
    //Button STOP
    display.setColor(180, 180, 180); 
    display.drawBox(3,SCREEN_HEIGHT/2+3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2 -6);
    display.setColor(255,255,255);
    display.setPrintPos(35,180);
    display.print("STOP");

    //Button Manuell
    display.setColor(180, 180, 180); 
    display.drawBox(SCREEN_WIDTH/2+3,SCREEN_HEIGHT/2+3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2 -6);
    display.setColor(255,255,255);
    display.setPrintPos(180,190);
    display.print("manuell");
}

int hR;
int mR;
String outputString = "";
void drawTimeRemaining(int tR){
	currentScreen = TIMEREMAINING;
  
  	outputString= "";
  	display.setFont(FONT62);
  	display.setColor(0,0,0);
  	display.drawBox(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
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
  
	display.setColor(255,255,255);
  	display.setPrintPos(70,140);
  	display.print(outputString);
  
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
  	display.begin(UCG_FONT_MODE_TRANSPARENT);
  	display.setRotate90();
  	display.clearScreen();
  	drawStartScreen();
  
  	touchscreen.InitTouch();
  	touchscreen.setPrecision(PREC_HI);
  	Serial.println("Init Touch");
}

int x = 0;
int y = 0;
unsigned long currentTime();

void loop(void){
	if(currentScreen == START){
    	if(touchscreen.dataAvailable()){
      		drawTimeSelectScreen();
      		delay(1500);
    	}
  	}
	else if(currentScreen == TIMESELECT){
    	if(touchscreen.dataAvailable()){
     		touchscreen.read();
      		x = touchscreen.getX();
      		y = touchscreen.getY();
			if((x > -1) && (y > -1)){
      			if(x < SCREEN_WIDTH/2){
        			if(y < SCREEN_HEIGHT/2){
          				//3H
          				timeRemaining = 3 * 60;
          				pinMode(RELAIS_MOTOR, HIGH);
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
          			//24H
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
    		//Reset of counters when screen touched
    		if(touchscreen.dataAvailable()){
      			touchscreen.read();
      			touchTimePassed = millis();
    		}
  		}
  		else if(currentScreen == TIMEREMAINING){
    		//Check if time for update
    		if(millis() - screenPassed >= 30000){
      			screenPassed = millis();
      			drawTimeRemaining(timeRemaining);
    		}
    		//Check if touched -> return to running screen
   			 if(touchscreen.dataAvailable()){
        		touchscreen.read();
        		drawRunningScreen();
        		touchTimePassed = millis();
    		}
  		}
  		if(projectRunning){
    		passTime();
  		}		
  }
}
