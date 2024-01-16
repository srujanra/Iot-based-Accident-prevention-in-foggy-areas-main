#define BLYNK_TEMPLATE_ID "TMPLKUdORu5N"
#define BLYNK_DEVICE_NAME "Fog Notification"
#define BLYNK_AUTH_TOKEN "FDjGv6SdoDtnZMvol-XBRivgM0sIWOnq"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
BlynkTimer timer;
char auths[] = BLYNK_AUTH_TOKEN;

#include "DHT.h"
#define DHTPIN 5
#define DHTTYPE DHT11

#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

DHT dht(DHTPIN, DHTTYPE);

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Scanning"
#define WIFI_PASSWORD "nopassword"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBdhJzeWAsDCXFE_XR-rmUvZ7OhhvTlgjE"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "dht11-dfccb-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//unsigned long sendDataPrevMillis = 0;
//int count = 0;
bool signupOK = false;

void setup(){

 
  
  pinMode(DHTPIN, INPUT);
  dht.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

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

   Blynk.begin(auths, WIFI_SSID, WIFI_PASSWORD);
  timer.setInterval(2500L, notifyOnfog);
}
double humidity;
double temp;
void loop(){
 delay(15000);
  float h = dht.readHumidity();

  float t = dht.readTemperature();

  temp = t;
  humidity = h;
  
  
  if (Firebase.ready() && signupOK ) {
    
    if (Firebase.RTDB.pushFloat(&fbdo, "DHT/humidity",h)){
//      Serial.println("PASSED");
       Serial.print("Humidity: ");
       Serial.println(h);
      
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.pushFloat(&fbdo, "DHT/temperature", t)){
//      Serial.println("PASSED");
       Serial.print("Temperature: ");
       Serial.println(t);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
  Serial.println("______________________________");

  Blynk.run();
  timer.run();
}
void notifyOnfog()
{
  Blynk.virtualWrite(V0, humidity);
  Blynk.virtualWrite(V5, temp);
  if(temp<28){
     Serial.println("its foggy!!!");
     //Blynk.email("neelvilasparab@gmail.com", "temp_alert", "It's foggy!!!");
     Blynk.logEvent("temp_alert","It's foggy!!!");
  }
}
