#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>
#include <Wire.h>


#define DEVICE_TYPE 10012
#define FIRMWARE_VERSION "1.0.2"

char ssid[] = "YOUR_SSID";      // your network SSID (name)
char pass[] = "YOUR_PASSWORD";  // your network password

int wifi_status  = WL_IDLE_STATUS;    // the Wifi radio's status
WiFiClient wifiClient;
PubSubClient client(wifiClient);  

char MqttServer[]     = "iiot.ideaschain.com.tw";  // new ideaschain dashboard MQTT server
int  MqttPort          = 1883;
char ClientId[]       = "DSI5168Test";            // MQTT client ID. it's better to use unique id.
char Username[]       = "CpmmjsKKn8BsHSj1Ks8D";    // device access token
char Password[]       = "CpmmjsKKn8BsHSj1Ks8D";    // no need password
char SubscribeTopic[] = "v1/devices/me/telemetry";  // Fixed topic. ***DO NOT MODIFY***
char PublishTopic[]   = "v1/devices/me/telemetry";  // Fixed topic. ***DO NOT MODIFY***
char SubscribeTopic2[] = "v1/devices/me/attributes";
char PublishTopic2[]   = "v1/devices/me/attributes";  // Fixed topic. ***DO NOT MODIFY***
char PublishPayload[]="{\"weight\":\"0\"}";

///
/// RGB LED
///
#include "ws2812b.h"
#define DIGITALPINNUMBER  10
#define NUM_LEDS  8
ws2812b Ledstrip = ws2812b(DIGITALPINNUMBER , NUM_LEDS);

//
// OLED 128x128
//
#include <U8g2lib.h>
U8G2_SSD1327_MIDAS_128X128_F_HW_I2C u8g2(U8G2_R0,  U8X8_PIN_NONE);


///
/// HX711重量模組
///
#include "HX711.h"
const int DT_PIN = 6;
const int SCK_PIN = 2;
const int SCALE_FACTOR = -20; //the propotional parameter,different from each person
HX711 Scale;

float Measure_Weight = 0.0;
#define W_COUNT 3
float Measure_Weight_Average[W_COUNT];
float TargetWeight = 1; // kg
float Number = 1;
float SumWeight = 0 ;
bool WhetherContinue = true;
bool Next = false ;
String Str = "\0";
unsigned long TimeNow = 0;
unsigned long TimeNow1 = 0;
unsigned long TimeNow2 = 0;
unsigned long TimeNow3 = 0;

void reconnect();

float g2kg(float gVal)
{
  return (gVal / 1000);
}
void uAreaA0Print(const char* msg0, int fontH)
{
  u8g2.setCursor(0, 0 + fontH);
  u8g2.print(msg0);
  u8g2.sendBuffer();
}
void uAreaB0Print(float weight)
{
  int pos_x = 45;
  int font_h = 23;
  u8g2.setFont(u8g2_font_logisoso16_tf);
  /*String Str = "";
  Str += weight;
  Str += "g";*/
  char buff[32];
  sprintf(buff, "%.1fkg", weight);
  Serial.println(buff);
  
  u8g2.setDrawColor(0);// Black
  u8g2.drawBox(pos_x, 0, 128-75, font_h+6);
  u8g2.setDrawColor(1); // White
  //u8g2.drawFrame(pos_x, 0, 128-75, font_h+6);
  u8g2.setCursor(pos_x, 0 + font_h);
  u8g2.print(buff);
  u8g2.sendBuffer();
}
void uAreaA1Print(float weight)
{
  int font_w = 20;
  int font_h = 41;
  u8g2.setFont(u8g2_font_logisoso28_tf);
  /*Str = "";
  Str += weight;
  Str += "g";*/
  char buff[32];
  sprintf(buff, "%.1fkg", weight);
  Serial.println(buff);

  u8g2.setDrawColor(0);// Black
  u8g2.drawBox(0, 32, 128, font_h + 10);
  u8g2.setDrawColor(1); // White
  //u8g2.drawFrame(15, 32, 128 - 30, font_h + 10);
  
  //
  // Center the string
  //
  Serial.print("tempx = ");
  float tempx = strlen(buff);
  Serial.print(tempx);
  Serial.print(",");
  tempx = tempx * font_w;
  Serial.print(tempx);
  Serial.print(",");
  tempx = 128 - tempx;
  Serial.print(tempx);
  Serial.print(",");
  tempx = tempx / 2;
  Serial.print(tempx);
  Serial.print("\n");
  u8g2.setCursor(tempx, 32 + font_h);
  u8g2.print(buff);
  u8g2.sendBuffer();
}
void uAreaA3Print(const char* msg0, const char* msg1, const char* msg2)
{
#define X_MESSAGE 0
#define Y_MESSAGE 64+32+3
#define H_CHAR_SMALL 12
  u8g2.setFont(u8g2_font_7x13_t_symbols);
  u8g2.setDrawColor(0);// Black
  u8g2.drawBox(0, 64+20, 128, 64-20);
  u8g2.setDrawColor(1); // White
  //u8g2.drawFrame(0, 64+20, 128, 64-20);
  if (strlen(msg0) > 0) {
    u8g2.setCursor(X_MESSAGE, Y_MESSAGE + (H_CHAR_SMALL * 0));
    u8g2.print(msg0);
  }
  if (strlen(msg1) > 0) {
    u8g2.setCursor(X_MESSAGE, Y_MESSAGE + (H_CHAR_SMALL * 1));
    u8g2.print(msg1);
  }
  if (strlen(msg2) > 0) {
    u8g2.setCursor(X_MESSAGE, Y_MESSAGE + (H_CHAR_SMALL * 2));
    u8g2.print(msg2);
  }
  u8g2.sendBuffer();
}

void setup(void) {
  Serial.begin(115200);
 
  u8g2.begin();  
  u8g2.enableUTF8Print();    // enable UTF8 support for the Arduino print() function
  u8g2.setFontDirection(0);
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_7x13_t_symbols);
  uAreaA0Print("target", 11);
  
  SumWeight =  TargetWeight * Number; //count the total weight
  uAreaB0Print(SumWeight);

  uAreaA1Print(0);

  u8g2.setFont(u8g2_font_7x13_t_symbols); //set the font
  uAreaA3Print("connecting...", "", ""); 
  
  for (int i = 0; i < 3; i++) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifi_status = WiFi.begin(ssid, pass);
    if (wifi_status == WL_CONNECTED)
      break;
    // wait 4 seconds for connection:
    delay(4000);
  }

  if (wifi_status == WL_CONNECTED) {

    uAreaA3Print("connect successed!", "", ""); 
    
    client.setServer(MqttServer, MqttPort);   
    reconnect();
    
    client.setCallback(callback);
    if (client.connect(Username)){
      Serial.println("Connection has been established, well done");
    }
  }
  else {
    uAreaA3Print("failed to connect", "", "");
  }
  
  Scale.begin(DT_PIN, SCK_PIN);
  Scale.set_scale(SCALE_FACTOR);//set the propotional parameter 設定比例參數
  Scale.tare();//  zero weight sensor 
   
  delay(1000);

  uAreaA3Print("enter", "weight , number", "");
  Ledstrip.begin(); //RGB begin

  //
  // Init the array.
  //
  Measure_Weight_Average[0] = g2kg(Scale.get_units(10)); //measure the weight
  for (int ii = 1; ii < W_COUNT; ii++) {
    Measure_Weight_Average[ii] = Measure_Weight_Average[0];
  }
}

float calAverage(float weight)
{
  static int ai = 0;
  Measure_Weight_Average[ai] = weight;

  float w_sum = 0;
  for (int ii = 0; ii < W_COUNT; ii++) {
    w_sum += Measure_Weight_Average[ii];
  }

  ai++;
  if (ai >= W_COUNT)
    ai = 0;

  return (w_sum / W_COUNT);
}
void loop(void) {
  if (client.connected()) {
    client.loop();
  }
  while(WhetherContinue){ 
    if(millis()- TimeNow > 5){
      TimeNow = millis(); 
      if (client.connected()) {
        client.loop();
      }
    }

    float temp_val = Scale.get_units(10);
    float temp_val2 = g2kg(temp_val);
    float temp_val3 = calAverage(temp_val2);
    Serial.print("temp_val,temp_val2,temp_val3=");
    Serial.print(temp_val);
    Serial.print(",");
    Serial.print(temp_val2);
    Serial.print(",");
    Serial.println(temp_val3);
    Measure_Weight = calAverage(g2kg(Scale.get_units(10))); //measure the weight
    //Serial.printf("temp_val,temp_val2,temp_val3=%f,%f,%f\n", temp_val,temp_val2,temp_val3);
    //if(Measure_Weight < 0) Measure_Weight = 0 ;
    uAreaA1Print(Measure_Weight);
    
    if(millis()- TimeNow1 > 3000 && millis() > TimeNow1 ){
      //Str = String((int)Measure_Weight) + "g";
      char buff[32];
      sprintf(buff, "%.1fkg", Measure_Weight);
      uAreaA3Print("total weight : ", buff, "");
      
      TimeNow1 = 12000.0 + millis() ; //avoid collison with another time
      TimeNow3 = 12000.0 + millis() ; //avoid collison with another time
      TimeNow2 = millis(); 
    }
    if(millis()- TimeNow2 > 3000 && millis() > TimeNow2 ){
      String c = "\0";
      String b = "\0";
      /*
      Serial.println(b);
      Serial.println(SumWeight - Measure_Weight);
      Serial.println(Measure_Weight*0.05  );
      */
      if( fabs(SumWeight - Measure_Weight)> SumWeight*0.05 ){ //if measure_weight smaller or bigger than 0.95 time total_weight
        int need_number = fabs(SumWeight - Measure_Weight)/TargetWeight ;
        if(need_number == 0)need_number = 1;
        if (client.connected()) {
          ///
          ///the payload format to IdeasChain and MQTT Dash to get the relevant value
          ///
          b="{\"Measure_Weight\":\""+ String(Measure_Weight) +"\",\"Target_Weight\":\"" + String(TargetWeight) + "\",\"number\":\"" + String(Number) + "\",\"Y/N\":\"N\",\"color\":\"#ff0000\",\"need\":\""+String(need_number)+"\"}";
          Serial.println(b);
          client.publish(PublishTopic, b.c_str());
        }
        // Serial.println(need_number);
        Str = "need  ";
        Str  = Str + String(need_number);
        //u8g2.clear();
        if(SumWeight - Measure_Weight < 0){ //measure_weight smaller or bigger than 0.95 total_weigh
          uAreaA3Print("fail!!", Str.c_str(), "more elements");
        }
        else{
          uAreaA3Print("fail!!", Str.c_str(), "less elements");
        }
        for(int i = 0 ; i < 8 ; i+=3){
          Ledstrip.setPixelColor(i,255,0,0); //All_RGB_to_Red
        }
        Ledstrip.show();
      }
    
      else{
        ///
        ///the payload format to IdeasChain and MQTT Dash to get the relevant value
        ///
        c="{\"Measure_Weight\":\""+ String(Measure_Weight) +"\",\"Target_Weight\":\"" + String(TargetWeight) + "\",\"number\":\"" + String(Number) + "\",\"Y/N\":\"Y\",\"color\":\"#00ff11\"}";
        if (client.connected()) {
          client.publish(PublishTopic, c.c_str());
        }
        Serial.println(c);
        
        uAreaA3Print("success!!", "", "");
    
        for(int i = 0 ; i < 8 ; i+=3){
          Ledstrip.setPixelColor(i,0,255,0); //All_RGB_to_Green
        }
        Ledstrip.show();
      }
      TimeNow1 = 12000.0 + millis() ; //avoid collison with another time
      TimeNow2 = 12000.0 + millis() ; //avoid collison with another time
      TimeNow3 = millis();
    }
    if(millis()- TimeNow3 > 3000 && millis() > TimeNow3 ){
      uAreaA3Print("continue measuring", "", "");
      for(int i = 0 ; i < 8 ; i+=3){
        Ledstrip.setPixelColor(i,85,85,85);
      }
      Ledstrip.show();
    Next = true;
    TimeNow2 = 12000.0 + millis() ;  //avoid collison with another time
    TimeNow3 = 12000.0 + millis() ;  //avoid collison with another time
    TimeNow1 = millis(); 
    }
  }    
  ///
  /// set up to next goods (weight and number)
  ///  
  if(Next){
   uAreaA3Print("enter", "weight , number", "");
   for(int i = 0 ; i < 8 ; i+=3){
     Ledstrip.setPixelColor(i,85,85,85); //stop RGB  
   }
   Ledstrip.show();
   Next = false;
  }
  if (wifi_status == WL_CONNECTED) {
    if (!client.connected()) {
       reconnect();
       delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  ///
  /// cuz pulish to the same portal,using first word of playoad to determine what to do  
  ///
  if(char(payload[2])!= 'w'){
    if (char(payload[2])== 'c')WhetherContinue = false ;
    for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    }
    return;
  }
  /*
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  */
  int cnt = 0 ; 
  String weight_str ="";
  String number_str ="";
  ///
  ///get thw weight and number yoy sended
  ///
  for (int i = 0; i < length; i++) {
    // Serial.print((char)payload[i]);
    if((char)payload[i]=='\"') cnt ++ ;
    else if(cnt==7) weight_str = weight_str + (char)payload[i]; // position of two parameter
    else if(cnt==15) number_str = number_str + (char)payload[i];
  }
  Serial.println();
  TargetWeight = atof(weight_str.c_str());
  Number = atof(number_str.c_str());
  SumWeight =  TargetWeight * Number; //count the total weight

  uAreaA3Print("start measuring", "", ""); 

  WhetherContinue = true; //set up continue measure
  delay(3000);
  TimeNow = millis(); //initialize time
  TimeNow1 = millis(); //initialize time
  TimeNow2 = millis()+10000; //avoid collison with another time
  TimeNow3 = millis()+10000; //avoid collison with another time
}


 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("\nAttempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ClientId,Username,Password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(PublishTopic2, PublishPayload);
      // ... and resubscribe
      client.subscribe(SubscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
