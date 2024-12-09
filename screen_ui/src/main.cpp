#include <Arduino.h>
#include <TFT_eSPI.h> 
#include <SPI.h>


#define xPin 2
#define yPin 15
#define swPin 19

TFT_eSPI tft = TFT_eSPI(); 

int xVal=0, yVal=0, butt=1, x;

void mainScreen();
void secondScreen();

void setup() {

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT);

  Serial.begin(9600);

  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_BLUE, TFT_BLACK); 
  tft.setTextSize(2);
}

void loop() {
   x = digitalRead(swPin);
  if(!x && yVal < 30 && yVal > 5 ){
    butt = butt ^1;
    tft.fillScreen(TFT_BLACK);
    delay(200);
  }
  if(butt){
    mainScreen();
  } else if (!butt){

    secondScreen();
  }

}

void clearCursor(int x, int y){
  tft.fillCircle(x,y,5, TFT_BLACK);
}

void drawCursor(int x, int y){
  tft.fillCircle(x,y,5, TFT_DARKGREEN);
}

void mainScreen(){
  clearCursor(xVal,yVal);
  xVal = xVal + map(analogRead(xPin)+300,0,4096,-5,5);
  yVal = yVal + map(analogRead(yPin)+300,0,4096,-5,5);
  if(yVal > 115){
    yVal =115;
  } else if(yVal < 10){
    yVal = 10;
  }

  if(xVal > 150){
    xVal =150;
  } else if(xVal < 10){
    xVal = 10;
  }
  
  if(yVal>5 && yVal < 30){
    tft.fillRoundRect(5,5,150,25,5,TFT_SKYBLUE);
  }else{
    tft.fillRoundRect(5,5,150,25,5,TFT_BLUE);
  }

  
  drawCursor(xVal, yVal);

  delay(70);
}

void secondScreen(){
  tft.drawRoundRect(80,50,40,60,5,TFT_CYAN);
  delay(200);
}