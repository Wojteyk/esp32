#include <Arduino.h>
#include <TFT_eSPI.h> 
#include <SPI.h>
#include <string.h>


#define xPin 2
#define yPin 15
#define swPin 19
#define background TFT_BLACK
#define led 21

TFT_eSPI tft = TFT_eSPI(); 

int xVal=0, yVal=0, screenType=0, x, screenWidth = 160, screenHeight = 128;

bool state = false;

void mainScreen();
void firstScreen();
void secondScreen();
void thirdScreen();

void setup() {

  pinMode(led, OUTPUT);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT);

  Serial.begin(9600);

  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(background);
  tft.setTextColor(TFT_BLUE, background); 
  tft.setTextSize(2);
}

void loop() {

  //changing screens
  switch (screenType)
  {
  case 1:
    firstScreen();
    break;

  case 2:
    secondScreen();
  break;
  
  case 3:
    thirdScreen();
  break;

  case 4:
    mainScreen();
  break;

  default:
    mainScreen();
    break;
  }
  
}


  /**************************************************************************************/
  /**************************************************************************************/
  /******************************* Definitions of functions *****************************/
  /**************************************************************************************/
  /**************************************************************************************/


  void clearCursor(){
    tft.fillCircle(xVal,yVal,5, background);    //erasing previous cursor
  }

  void drawReturn(){
    tft.drawRoundRect(5,105,31,20,5,TFT_WHITE);
    tft.setCursor(8,108);
    tft.setTextColor(TFT_RED);
    tft.print("<-");
  }

  void drawCursor(){

    //mapping current jostick position to move the cursor
    xVal = xVal + map(analogRead(xPin)+300,0,4096,-5,5);
    yVal = yVal + map(analogRead(yPin)+300,0,4096,-5,5);

    //checking if the cursor is outside of the screen
    if(yVal > screenHeight - 8){
      yVal =screenHeight - 8;
    } else if(yVal < 10){
      yVal = 10;
    }

    if(xVal > screenWidth -10){
      xVal =screenWidth - 10;
    } else if(xVal < 10){
      xVal = 10;
    }

    tft.fillCircle(xVal,yVal,5, TFT_DARKGREEN);   //drawing cursor
  }

  void mainScreen(){

    x = digitalRead(swPin);
    
    clearCursor();

    //buttons
    tft.setTextColor(TFT_BLUE);
    tft.setCursor(20+(120-(4*12))/2,10);
    tft.print("Temp");
    tft.setCursor(20+(120-(7*12))/2,40);
    tft.print("weather");
    tft.setCursor(20+(120-(5*12))/2,70);
    tft.print("Cipki");

    //light the button if cursors is on it  
    if(yVal>5 && yVal < 30){

      tft.drawRoundRect(20,5,120,25,5,TFT_SKYBLUE);
      
    }else if(yVal > 35 && yVal < 60){

      tft.drawRoundRect(20,35,120,25,5,TFT_SKYBLUE);

    }else if(yVal > 65 && yVal < 90){

      tft.drawRoundRect(20,65,120,25,5,TFT_SKYBLUE);

    }else{

      tft.drawRoundRect(20,5,120,25,5,TFT_BLUE);
      tft.drawRoundRect(20,35,120,25,5,TFT_BLUE);
      tft.drawRoundRect(20,65,120,25,5,TFT_BLUE);

    }

    //making action when button is clicked and in  the cursor is in the right position
    if(!x && yVal < 30 && yVal > 5 ){

      screenType = 1;
      tft.fillScreen(background);
      delay(200);

    }else if(!x && yVal < 60 && yVal > 35 ){

      screenType = 2;
      tft.fillScreen(background);
      delay(200);

    }else if(!x && yVal < 90 && yVal > 65 ){

      screenType = 3;
      tft.fillScreen(background);
      delay(200);

    }

    drawCursor();
    delay(70);
    
  }
  
  void firstScreen(){

    clearCursor();
    
    drawReturn();

    x = digitalRead(swPin);

    if(!x && xVal < 35 && yVal > 100 ){

      screenType = 4;
      tft.fillScreen(background);
      delay(200);

    }

    drawCursor();
    delay(70);
  }

  void secondScreen(){

    clearCursor();

    drawReturn();

    x = digitalRead(swPin);

    if(!x && xVal < 35 && yVal > 100 ){

      screenType = 4;
      tft.fillScreen(background);
      delay(200);

    }

    drawCursor();
    delay(70);
  }

  void thirdScreen(){

    clearCursor();

    drawReturn();

    tft.setTextColor(TFT_BLUE);
    tft.setCursor(10,50);
    tft.print("LED:");

    
    //button operation 
    x = digitalRead(swPin);

    if(!x && xVal < 35 && yVal > 100 ){

      screenType = 4;
      tft.fillScreen(background);
      delay(200);
      return;

    }else if(!x && xVal < 80 && xVal > 60  && yVal > 50 && yVal < 70 ){

      state = !state;
      delay(200);

    }else{
      
    }

    //changing color of button (circle)
    if(state){
      tft.fillCircle(70,60,10,TFT_BLUE);
      digitalWrite(led,state);
    }else{
      digitalWrite(led,state);
      tft.fillCircle(70,60,10,TFT_RED);
    }
    
    drawCursor();
    delay(70);
  }