#include <Arduino.h>
#include <WiFi.h>               
#include <Firebase_ESP_Client.h>
#include <DHT.h>   
#include <TFT_eSPI.h> 
#include <SPI.h>
            
#define statusLedBlue 1
#define statusLedRed 1
#define O1_pin 22
#define O2_pin 23
#define O3_pin 21
#define DHT_SENSOR_PIN 2
#define DHT_SENSOR_TYPE DHT11
int value=0;
String valueString="t", oldString="t";

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

TFT_eSPI tft = TFT_eSPI(); 

unsigned long sendDataPrevMillis = 0;
unsigned long sendDataPrevMillis2 = 0;
int count = 0;
bool signupOK = false;    //since we are doing an anonymous sign in 
float tempCity = 0; 
float humiCity = 0;

                 
void setup(){

  pinMode(statusLedBlue, OUTPUT);
  pinMode(statusLedRed, OUTPUT);
  pinMode(O1_pin, OUTPUT);
  pinMode(O2_pin, OUTPUT);
  pinMode(O3_pin, OUTPUT);

  dht_sensor.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  tft.init();

  // setting screen parameters
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_BLUE, TFT_BLACK); 
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
  //temperature and humidity measured should be stored in variables so the user
  float temperature = dht_sensor.readTemperature();
  float humidity = dht_sensor.readHumidity();

  tft.setCursor(5, 10);
  tft.print("T wew: ");
  tft.print(temperature);
  tft.setCursor(5, 30);
  tft.print("T zew: ");
  tft.print(tempCity);  


  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    //since we want the data to be updated every second
    sendDataPrevMillis = millis();
    // Enter Temperature in to the DHT_11 Table
    if (Firebase.RTDB.setInt(&fbdo, "DHT_11/Temperature", temperature)){
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
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis2 > 100 || sendDataPrevMillis2 == 0)){
    sendDataPrevMillis2 = millis();
    /*if(Firebase.RTDB.getInt(&fbdo, "Outlet/O1", &value)){
      digitalWrite(O1_pin, !value);
    } else{
        Serial.println("Failed O1");
      }
    if(Firebase.RTDB.getInt(&fbdo, "Outlet/O2", &value)){
      digitalWrite(O2_pin, !value);
    } else{
        Serial.println("Failed O2");
      }
    if(Firebase.RTDB.getInt(&fbdo, "Outlet/O3", &value)){
      digitalWrite(O3_pin, !value);
    } else{
        Serial.println("Failed O3");
      }
      */
    if(Firebase.RTDB.getString(&fbdo, "Message/text", &valueString)){

      if(oldString != valueString){
        tft.fillScreen(TFT_BLACK);
      }

      tft.setCursor(5, 50);
      tft.print(valueString);
      oldString = valueString;
      
    } else{
        Serial.println("Failed text message");
      }




    
  }
    
  

}