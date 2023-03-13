#ifdef ESP32 
#include "WiFi.h" 
#else 
#include "ESP8266WiFi.h" 
#endif 
#include "WiFiClientSecure.h" 
#include "UniversalTelegramBot.h" 
#include "ArduinoJson.h" 
#include "Adafruit_BME280.h" 
#include "Adafruit_Sensor.h" 

const char* ssid = "Krokodill";
const char* password = "Birdy1977";

#define BOTtoken "5785951704:AAEhQfziLkTfzeq_3r_TxfEbOZyT2DMJCPM"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

 int botRequestDelay = 1000;
 unsigned long lastTimeBotRan;

 Adafruit_BME280 bme;

 String getReadings(){
  float temperature, humidity, pressure;
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F * 0.7500637554192;

  String message = "🌡Температура: " + String(temperature) + " ºC \n";
  message += "💧 Влажность: " + String(humidity) + " % \n";
  message += "㎪ Давление: " + String(pressure) + " Mm \n";
  return message;
 }

 String getTempr(){
  float temperature;
  temperature = bme.readTemperature();
  String message = "🌡 Температура: " + String(temperature) + " ºC \n";
  return message;
 }
  String getHum(){
  float humidity;
  humidity = bme.readHumidity();
  String message = "💧 Влажность: " + String(humidity) + " % \n";
  return message;
 }
 String getPres(){
  float pressure;
  pressure = bme.readPressure() / 100.0F * 0.7500637554192;
  String message = "㎪ Давление: " + String(pressure) + " Mm \n";
  return message;
 }


void handleNewMessages(int numNewMessages){
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++){
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start"){
      String welcome = "Добро пожаловать, " + from_name + ".\n";
      welcome += "Используйте эти команды, чтобы получить данные:\n\n";
      welcome += "/readings \n";
      welcome += "/temperature \n";
      welcome += "/humidity \n";
      welcome += "/pressure \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/readings"){
      String readings = getReadings();
      bot.sendMessage(chat_id, readings, "");
    }
        if (text == "/temperature"){
      String readings = getTempr();
      bot.sendMessage(chat_id, readings, "");
    }
        if (text == "/humidity"){
      String readings = getHum();
      bot.sendMessage(chat_id, readings, "");
    }
        if (text == "/pressure"){
      String readings = getPres();
      bot.sendMessage(chat_id, readings, "");
    }
  }
}
void setup(){
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");
    client.setTrustAnchors(&cert);
  #endif

  if (!bme.begin(0x76)){
    Serial.println("Could not find a valid sensor!");
    while(1);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
}
void loop(){
  if (millis() > lastTimeBotRan + botRequestDelay){
    int numNewMessages = bot.getUpdates(bot.last_message_received +1);

    while(numNewMessages){
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received +1);
    }
    lastTimeBotRan = millis();
  }
}
