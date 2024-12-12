#include <Arduino.h>
#include <WiFi.h>               
#include <Firebase_ESP_Client.h>
#include <DHT.h> 
#include <TFT_eSPI.h> 
#include <SPI.h>  
            
#define statusLedBlue 22
#define statusLedRed 27
#define O1_pin 15
#define O2_pin 27
#define O3_pin 27
#define DHT_SENSOR_PIN 21
#define DHT_SENSOR_TYPE DHT11//???????????
#define xPin 33
#define yPin 32
#define swPin 19
#define background TFT_BLACK
#define led 27 //to be erased

int xVal=0, yVal=0, screenType=0, x, screenWidth = 160, screenHeight = 128, valueO1=0, valueO2=0, valueO3=0, sentVal=0, humiTft =0 , tempTft= 0, tempCityTft=0, humiCityTft=0;
float temperature, humidity;
bool state = false;

unsigned long sendDataPrevMillis = 0;
unsigned long sendDataPrevMillis2 = 0;
int count = 0;
bool signupOK = false;    //since we are doing an anonymous sign in 
float tempCity = 0; 
float humiCity = 0;

TFT_eSPI tft = TFT_eSPI();

void mainScreen();
void firstScreen();
void secondScreen();
void thirdScreen();

//To provide the ESP32 / ESP8266 with the connection and the sensor type
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

//Provide the token generation process info.s
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Orange_Swiatlowod_2E30"
#define WIFI_PASSWORD "fortnite123"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCcaqEz7jtyBOsQwXnXusmHX6ko1LLgg4o"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://real-time-data-31f84-default-rtdb.europe-west1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
                 
void setup(){

  pinMode(statusLedBlue, OUTPUT);
  pinMode(statusLedRed, OUTPUT);
  pinMode(O1_pin, OUTPUT);
  pinMode(O2_pin, OUTPUT);
  pinMode(O3_pin, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT);

  dht_sensor.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  

  // setting screen parameters
  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(background);
  tft.setTextColor(TFT_BLUE, background); 
  tft.setTextSize(2);
  
  //checking wifi status
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    digitalWrite(statusLedRed, HIGH);
    delay(300);
  }
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  digitalWrite(statusLedRed, LOW);
  digitalWrite(statusLedBlue, HIGH);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){

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

  //temperature and humidity measured should be stored in variables 
   temperature = dht_sensor.readTemperature();
   humidity = dht_sensor.readHumidity();
  


  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 50000 || sendDataPrevMillis == 0)){
    //since we want the data to be updated every second
    sendDataPrevMillis = millis();
    // Enter Temperature in to the DHT_11 Table
    if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Temperature", temperature)){
      // This command will be executed even if you dont serial print but we will make sure its working
      Serial.print("Temperature : ");
      Serial.println(temperature);
    }
    else {
      Serial.println("Failed to Read from the Sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Humidity", humidity)){
      Serial.print("Humidity : ");
      Serial.println(humidity);
    }
    else {
      Serial.println("Failed to Read from the Sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.getFloat(&fbdo, "Weather/tempCity", &tempCity)){
        Serial.print("TempCity : ");
        Serial.println(tempCity);
    } else{
        Serial.println("failed to get data " + fbdo.errorReason());
    }
    if (Firebase.RTDB.getFloat(&fbdo, "Weather/humiCity", &humiCity)){ 
        Serial.print("HumiCity : ");
        Serial.println(humiCity);
    } else{
        Serial.println("failed to get data " + fbdo.errorReason());
    }
  }

  // taking value from databse and changing value of a pin
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis2 > 1000 || sendDataPrevMillis2 == 0)){
    sendDataPrevMillis2 = millis();
    if( sentVal ==1){
      Firebase.RTDB.setInt(&fbdo, "Outlet/O1", valueO1);
      sentVal=0;
    }
    if(Firebase.RTDB.getInt(&fbdo, "Outlet/O1", &valueO1)){
      digitalWrite(O1_pin, !valueO1);
    } else{
        Serial.println("Failed O1");
      }    
    if(Firebase.RTDB.getInt(&fbdo, "Outlet/O2", &valueO2)){
      digitalWrite(O2_pin, !valueO2);
    } else{
        Serial.println("Failed O2");
      }
    if(Firebase.RTDB.getInt(&fbdo, "Outlet/O3", &valueO3)){
      digitalWrite(O3_pin, !valueO3);
    } else{
        Serial.println("Failed O3");
      }
    
  }

}

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
    tft.setCursor(20+(120-(4*12))/2,10); //(4)lenght of string
    tft.print("Temp");
    tft.setCursor(20+(120-(7*12))/2,40);
    tft.print("weather");
    tft.setCursor(20+(120-(7*12))/2,70);
    tft.print("Control");

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

    //conversion from float to int to string
    tempTft = (int)temperature;
    humiTft = (int)humidity;
    String strTempTft, strHumiTft;

    //layout
    drawReturn();
    
    tft.setTextColor(TFT_BLUE, background);
    tft.drawRoundRect(15,15,55,30,5,TFT_WHITE);
    strTempTft = String(tempTft);
    tft.drawCentreString(strTempTft,43,22,1);
    tft.drawCentreString("Temp",43,52,1);

    tft.drawRoundRect(90,15,55,30,5,TFT_WHITE);
    strHumiTft = String(humiTft);
    tft.drawCentreString(strHumiTft, 119,22,1);
    tft.drawCentreString("Hum",119,52,1);
    
    //return button action
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

    //conversion from flot to int to string
    humiCityTft = (int)humiCity;
    tempCityTft = (int)tempCity;
    String strTempCityTft, strhumiCityTft;

    //layout
    drawReturn();
    tft.setTextColor(TFT_BLUE, background);
    tft.drawRoundRect(30,10,100,30,5,TFT_WHITE);
    tft.drawCentreString("Wroclaw",81,17,1);

    tft.drawRoundRect(15,48,55,30,5,TFT_WHITE);
    strTempCityTft = String(tempCityTft);
    tft.drawCentreString(strTempCityTft,43,55,1);
    tft.drawCentreString("Temp",43,85,1);

    tft.drawRoundRect(90,48,55,30,5,TFT_WHITE);
    strhumiCityTft = String(humiCityTft);
    tft.drawCentreString(strhumiCityTft,119,55,1);
    tft.drawCentreString("Hum",119,85,1);


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
    tft.print("O1: ");

    
    //button operation 
    x = digitalRead(swPin);

    if(!x && xVal < 35 && yVal > 100 ){

      screenType = 4;
      tft.fillScreen(background);
      delay(200);
      return;

    }else if(!x && xVal < 80 && xVal > 60  && yVal > 50 && yVal < 70 ){
      sentVal =1;
      valueO1 = valueO1^1;
      delay(200);

    }

    //changing color of button (circle)
    if(valueO1==1){
      tft.fillCircle(70,60,10,TFT_BLUE);
    }else{
      tft.fillCircle(70,60,10,TFT_RED);
    }
    
    drawCursor();
    delay(70);
  }