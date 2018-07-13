
#include "Ucglib.h"
#include "URTouch.h"
#include <OneWire.h>
#include <DallasTemperature.h>


const PROGMEM int SCREEN_WIDTH = 320;
const PROGMEM int SCREEN_HEIGHT = 240;

//Full pinout can be seen at: URL

const PROGMEM byte LCD_CD = 10;
const PROGMEM byte LCD_CS = 9;
const PROGMEM byte LCD_RESET = 8;

const PROGMEM byte T_CLK = 2;
const PROGMEM byte T_CS = 3;
const PROGMEM byte T_DIN = 4;
const PROGMEM byte T_DO = 5;
const PROGMEM byte T_IRQ = 6;

const PROGMEM byte RELAIS_MOTOR = 18;
const PROGMEM byte RELAIS_HEAT = 17;
const PROGMEM byte RELAIS_FAN = 16;
const PROGMEM byte RELAIS_LED = 0;


const PROGMEM int ROTATIONTIME_MILLIS = 0;

Ucglib_ILI9341_18x240x320_HWSPI display(LCD_CD, LCD_CS, LCD_RESET);
URTouch touchscreen(T_CLK, T_CS, T_DIN, T_DO, T_IRQ);



const PROGMEM byte TEMPERATURE_SENSOR = 15;
OneWire oneWire(TEMPERATURE_SENSOR);
DallasTemperature tempSensor(&oneWire);

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

byte c = 0;

screens currentScreen;

char tmpS[0];
int getStringWidth(String s){
	tmpS[sizeof(s)];
  s.toCharArray(tmpS, sizeof(tmpS));
	return display.getStrWidth(tmpS);
}

void drawStartScreen(void){
  //Init Screen
  currentScreen = START;
  
  display.setColor(100, 100, 100);
  display.setFont(FONT62);
  display.drawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  display.setPrintPos(140,140);
  display.print(F("Hi"));
  
}

byte timeSelectHours = 12;
byte timeSelectMinutes = 0;

void drawTimeSelectScreen(void){
	//Time Select screen
	currentScreen = TIMESELECT;
	//Redraw background and clean everything
	display.setColor(180, 180, 180);
	display.drawBox(3, 3, SCREEN_WIDTH-6, SCREEN_HEIGHT-6);
	
	//draw time up triangles
	display.setColor(255, 255, 255);
	display.drawTriangle(72,41,  104,18,  136,41);
	display.drawTriangle(178,41,  210,18,  242,41);
	//draw time font
	display.setFont(FONT42);
	display.setPrintPos(62,113);
	display.print(timeSelectHours+":0"+timeSelectMinutes);
	//draw time down triangles
	display.drawTriangle(72, 126,  104,149, 136,126);
	display.drawTriangle(178, 126,  210,149, 242,126);
	//draw confirm Button
	display.setColor(0, 255, 0);
	display.drawRBox(105,170, 105,30, 7);
	//draw OK textattr
	display.setColor(255,255,255);
	display.setPrintPos(144,195);
	display.setFont(FONT26);
	display.print(F("Ok"));
	

}
void updateTimeSelectHours(){
	display.setColor(180,180,180);
	display.setFont(FONT42);
	display.drawBox(62, 68, getStringWidth(String(timeSelectHours)),45);
	display.setPrintPos(62, 113);
	if(timeSelectHours > 9){
		display.print(String(timeSelectHours));
	}
	else{
		display.print("0"+ String(timeSelectHours));
	}
	
	delay(400);
}
void updateTimeSelectMinutes(){
	display.setColor(180,180,180);
	display.setFont(FONT42);
	display.drawBox(62+getStringWidth(String(timeSelectHours)+":"), 68, getStringWidth(String(timeSelectMinutes)), 45);
	display.setPrintPos(62+2+getStringWidth(String(timeSelectHours)+":"), 113);
	if(timeSelectMinutes > 9){
		display.print(String(timeSelectMinutes));
	}
	else{
		display.print("0"+String(timeSelectMinutes));
	}
}

void drawRunningScreen(void){
    currentScreen = RUNNING;

    display.setColor(100, 100, 100);
    display.setFont(FONT26);
    display.drawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    //Button PLAY/PAUSE
    display.setColor(180, 180, 180); 
    display.drawBox(3,3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2 -6);
	  display.setColor(255,0,0);
	  display.drawBox(65,42,9,27);
	  display.drawBox(87,42,9,27);
    
    //Button STOP
    display.setColor(180, 180, 180); 
    display.drawBox(3,SCREEN_HEIGHT/2+3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2 -6);
    display.setColor(255,0,0);
    display.drawTetragon(60,156,  71,145,  92,145,  103,156);
	  display.drawBox(60,157,43,19);
	  display.drawTetragon(60,177,  103,177,  92,188,  71,188);
	  display.setColor(255,255,255);
	  display.setPrintPos(54,223);
	

   
	//EMPTY
	display.setColor(180, 180, 180); 
    display.drawBox(SCREEN_WIDTH/2+3,3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2 -6);
	 //Button Manuell
    display.setColor(180, 180, 180); 
    display.drawBox(SCREEN_WIDTH/2+3,SCREEN_HEIGHT/2+3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2 -6);
    display.setColor(255,255,255);
    display.setPrintPos(180,190);
    display.print(F("manuell"));
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
  
  	display.begin(UCG_FONT_MODE_TRANSPARENT);
  	display.setRotate90();
  	display.clearScreen();
  	drawStartScreen();
  
  	touchscreen.InitTouch();
  	touchscreen.setPrecision(PREC_HI);
	
	  tempSensor.begin();
	  Serial.println(F("sc"));
}

int touchedX = 0;
int touchedY = 0;

void loop(void){
	
	//DEBUG
	//tempSensor.requestTemperatures();
	//Serial.println(tempSensor.getTempCByIndex(0));
	//DEBUG
	
	if(currentScreen == START){
		if(touchscreen.dataAvailable()){
			drawTimeSelectScreen();
		}
	}
	else if(currentScreen == TIMESELECT){
    	if(touchscreen.dataAvailable()){
     		touchscreen.read();
      		touchedX = touchscreen.getX();
      		touchedY = touchscreen.getY();
			//Need to make sure we get valid inputs
			if((touchedX > -1) && (touchedY > -1)){
				//Hours up triangle
      			if((touchedX > 72) && (touchedX < 136) && (touchedY > 18) && (touchedY < 41)){
					if(timeSelectHours < 23){
						timeSelectHours++;
						updateTimeSelectHours();
					}
				}
				//Minutes up triangle
				else if((touchedX > 178) && (touchedX < 242) && (touchedY > 18) && (touchedY < 41)){
					if(timeSelectMinutes < 60){
						timeSelectMinutes += 15;
						updateTimeSelectMinutes();
					}	
				}
				//Hours down triangle
				else if((touchedX > 72) && (touchedX < 136) && (touchedY > 126) && (touchedY < 149)){
					if(timeSelectHours > 0){
						timeSelectHours--;
						updateTimeSelectMinutes;
					}
				}
				//Minutes down triangle
				else if((touchedX > 178) && (touchedX < 242) && (touchedY > 126) && (touchedY < 149)){
					if(timeSelectMinutes > 0){
						timeSelectMinutes -= 15;
						updateTimeSelectMinutes();
					}
				}
				//Ok Button
				else if((touchedX > 105) && (touchedX < 210) && (touchedY > 170) && (touchedY < 200)){
					projectRunning = true;
					drawRunningScreen();
					delay(500);
					touchTimePassed = millis();
					timePassed = millis();	
				}		
			}	
    	}
  	}
  	else if(currentScreen == RUNNING){
    	if(millis() - touchTimePassed >= 15000){
      		//Aufruf timeRemaining Screen
      		drawTimeRemaining(timeRemaining);
      		delay(500);
    	}
    	//Reset of counters when screen touched
    	if(touchscreen.dataAvailable()){
			touchTimePassed = millis();
			touchscreen.read();
			touchedX = touchscreen.getX();
			touchedY = touchscreen.getY();
			if((touchedX > -1) && (touchedY > -1)){
				//PLAY PAUSE BUTTON
				//display.drawBox(3,3,SCREEN_WIDTH/2-6,SCREEN_HEIGHT/2 -6);
				if((touchedX > 3) && (touchedX < SCREEN_WIDTH/2-3) && (touchedY > 3) && (touchedY < SCREEN_WIDTH/2-3)){
					if(projectRunning){
						//Redraw Green triangle
						display.setColor(180,180,180);
						display.drawBox(65, 42, 31, 27);
						display.setColor(0, 255, 0);
						display.drawTriangle(65, 42,  92,56,  65, 69);
						projectRunning = false;
					}
					else{
						//Redraw pause symbol
						display.setColor(180,180,180);
						display.drawBox(65, 42, 31, 27);
						display.setColor(255,0,0);
						display.drawBox(65,42,9,27);
						display.drawBox(87,42,9,27);
					}
				}
			}
    	}
  	}
  	else if(currentScreen == TIMEREMAINING){
    	//Check if time for update
    	if(millis() - screenPassed >= 30000){
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
		//Turn the motor on
		pinMode(RELAIS_MOTOR, HIGH);
		//Check if heat is needed
		tempSensor.requestTemperatures();
    c = tempSensor.getTempCByIndex(0);
    Serial.println(c);
		if(c < 30){
			pinMode(RELAIS_HEAT, HIGH);
			pinMode(RELAIS_FAN, HIGH);
		}
		else{
			pinMode(RELAIS_HEAT, LOW);
			pinMode(RELAIS_FAN, LOW);
		}	
  	}		
}
