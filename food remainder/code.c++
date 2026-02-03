#include <WiFi.h>
#include <HTTPClient.h>
#include <BluetoothSerial.h>
#include <Firebase_ESP_Client.h>
#include <arduinoFFT.h>

#define WIFI_SSID "YOUR_WIFI"
#define WIFI_PASSWORD "YOUR_PASSWORD"


String BOT_TOKEN = "YOUR_BOT_TOKEN";
String CHAT_ID  = "YOUR_CHAT_ID";


#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "https://your-db.firebaseio.com/"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


#define SAMPLES 256
#define SAMPLING_FREQUENCY 2000

arduinoFFT FFT = arduinoFFT();
double vReal[SAMPLES];
double vImag[SAMPLES];

#define MIC_PIN 34


BluetoothSerial SerialBT;

unsigned long lastMealTime = 0;
unsigned long minMealGap = 4UL * 60UL * 60UL * 1000UL; // 4 hours

float adaptiveThreshold = 0.0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Smart_Hunger_Reminder");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);

  calibrateSensor();
}


void loop() {
  collectFFT();
  float hungerScore = analyzeLowFrequency();

  if (hungerScore > adaptiveThreshold &&
      millis() - lastMealTime > minMealGap) {

    sendTelegram("🍽️ Hunger Detected!\nSmart system suggests eating now.");
    SerialBT.println("🍽️ Hunger Detected. Time to Eat.");

    Firebase.RTDB.pushFloat(&fbdo, "/hungerScore", hungerScore);

    lastMealTime = millis();
    delay(60000);
  }

  delay(3000);
}




void collectFFT() {
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = analogRead(MIC_PIN);
    vImag[i] = 0;
    delayMicroseconds(1000000 / SAMPLING_FREQUENCY);
  }

  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
}


float analyzeLowFrequency() {
  float energy = 0;
  for (int i = 5; i < 40; i++) {
    energy += vReal[i];
  }
  return energy / 35;
}


void calibrateSensor() {
  float baseline = 0;
  for (int i = 0; i < 20; i++) {
    collectFFT();
    baseline += analyzeLowFrequency();
    delay(500);
  }
  adaptiveThreshold = (baseline / 20) * 1.8;
}


void sendTelegram(String msg) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + BOT_TOKEN +
                 "/sendMessage?chat_id=" + CHAT_ID +
                 "&text=" + msg;
    http.begin(url);
    http.GET();
    http.end();
  }
}
