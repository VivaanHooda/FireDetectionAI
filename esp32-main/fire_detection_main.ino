#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <ArduinoJson.h>
#define BLYNK_TEMPLATE_ID "TMPL3307X4vp6"
#define BLYNK_TEMPLATE_NAME "Fire detection Project"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "dPvIz6VgVVVCv6aUqXAjhaRrwMPZzzIf";
char ssid[] = "motorola edge 40_5402";
char pass[] = "shreyas2005";

#define I2C_SDA 21
#define I2C_SCL 22
#define MQ5_PIN 5
#define FLAME_PIN 4
#define RX_PIN 16
#define TX_PIN 17
#define BPIN 19


//Sensor thresholds
const int MQ2_THRESHOLD = 2000;
const int MQ5_THRESHOLD = 600;
const int MQ7_THRESHOLD = 1800;
const int TEMP_THRESHOLD = 60;
const int hum_threshold = 30;
const int gas_threshold = 15000;

#include <HardwareSerial.h>
HardwareSerial SerialComm(1);

Adafruit_BME680 bme;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

bool worksmlx = true;
bool worksbme = true;
bool recieved = true;
bool tempCheck, gasCheck, humCheck, objCheck, smoke, lpg, co, flameCheck;

unsigned long lastJsonSendTime = 0;
const unsigned long jsonSendInterval = 2000;

float mlxObj, mlxAmb, temp, hum, safe, fire;
int gas, mq2Val, mq5Val, mq7Val, flame, count;

void initializeConditions() {
  tempCheck = false;
  gasCheck = false;
  humCheck = false;
  objCheck = false;
  smoke = false;
  lpg = false;
  co = false;
  flameCheck = false;
}
void initializeValues() {
  mlxObj = 0;
  mlxAmb = 0;
  temp = 0;
  hum = hum_threshold;
  gas = gas_threshold;
  mq2Val = 0;
  mq5Val = 0;
  mq7Val = 0;
  flame = 1;
  safe = 1;
}

bool checkMlx() {
  if (!mlx.begin()) {
    Serial.println("MLX90614 not found. Check wiring!");
    return false;
  }
  return true;
}

bool checkBme() {
  if (!bme.begin()) {
    Serial.println("BME680 not found. Check wiring!");
    return false;
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  SerialComm.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  initializeValues();
  initializeConditions();
  Wire.begin(I2C_SDA, I2C_SCL);
  pinMode(MQ5_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(BPIN, OUTPUT);
  worksmlx = checkMlx();
  worksbme = checkBme();
  Blynk.begin(auth, ssid, pass);
  if (worksbme) {
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setGasHeater(320, 150);
  }
}

void loop() {
  count = 0;
  unsigned long now = millis();
  recieved = true;
  if (worksmlx) {
    mlxObj = mlx.readObjectTempC();
    mlxAmb = mlx.readAmbientTempC();
  } else {
    mlxObj = 0;
    mlxAmb = 0;
    checkMlx();
  }

  if (worksbme && bme.performReading()) {
    temp = bme.temperature;
    hum = bme.humidity;
    gas = bme.gas_resistance;
  } else {
    temp = 0;
    hum = hum_threshold;
    gas = gas_threshold;
    checkBme();
  }
  
  mq5Val = analogRead(MQ5_PIN);
  flame = digitalRead(FLAME_PIN);

  //Recieving data
  StaticJsonDocument<200> doc;
  String message;
  Serial.print("Message recieved: ");
  if (SerialComm.available()) {
    message = SerialComm.readStringUntil('\n');
    Serial.println(message);
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
      Serial.print("❌ JSON parse failed: ");
      Serial.println(error.c_str());
      recieved = false;
      return;
    }
    mq7Val = doc["mq7"];
    mq2Val = doc["mq2"];
    safe = doc["safe"];
    fire = 1 - safe;
  }


  Serial.println("\n--- Gas Sensor Readings ---");
  Serial.print("MQ2 (Smoke): ");
  Serial.println(mq2Val);
  Serial.print("MQ5 (LPG): ");
  Serial.println(mq5Val);
  Serial.print("MQ7 (CO avg): ");
  Serial.println(mq7Val);
  Serial.print("Flame Sensor: ");
  Serial.println(flame);
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
  Serial.print("Gas Level: ");
  Serial.println(gas);
  Serial.print("MLX Object Temperature: ");
  Serial.print(mlxObj);
  Serial.println(" °C");
  Serial.print("Ambient Temperature: ");
  Serial.print(mlxAmb);
  Serial.println(" °C");
  Serial.println("\n--- Fire Probability ---");
  Serial.printf("🔥 Fire probability: %.4f\n", fire);
  Serial.printf("✅ Non-fire probability: %.4f\n", safe);


  //Sending data to app and website
   Blynk.run();

  Blynk.virtualWrite(V6, temp);
  Blynk.virtualWrite(V7, hum);
  Blynk.virtualWrite(V8, gas);
  Blynk.virtualWrite(V2, mq2Val);
  Blynk.virtualWrite(V3, mq5Val);
  Blynk.virtualWrite(V4, mq7Val);
  Blynk.virtualWrite(V9, mlxObj);
  Blynk.virtualWrite(V10, mlxAmb);
  Blynk.virtualWrite(V5, (flame == 1? 0:1));
  Blynk.virtualWrite(V0, safe);
  //Blynk.virtualWrite(V11, fire_prob > 0.7 ? 1 : 0);

  // ---------- Fire Detection ----------

  if (recieved){
    co = mq7Val > MQ7_THRESHOLD;
  }
  else {
     initializeConditions();
     recieved = true;
  }

  if((temp > TEMP_THRESHOLD) || (mlxAmb > TEMP_THRESHOLD))
  {
    count ++;
    tempCheck = true;
  }
  else
  {
    tempCheck = false;
  }

  if(hum < hum_threshold)
  {
    count ++;
    humCheck = true;
  }
  else
  {
    humCheck = false;
  }

  if(gas < gas_threshold)
  {
    count ++;
    gasCheck = true;
  }
  else
  {
    gasCheck = false;
  }

  if(mq2Val > MQ2_THRESHOLD)
  {
    count ++;
    smoke = true;
  }
  else
  {
    smoke = false;
  }

  if(mq5Val > MQ5_THRESHOLD)
  {
    count ++;
    lpg = true;
  }
  else
  {
    lpg = false;
  }

  flameCheck = (flame == 0);

  if ((count >= 3) || flameCheck || safe<0.2) {
    Serial.println("🔥🔥🔥 Fire Possibly Detected! 🔥🔥🔥");
    digitalWrite(BPIN, HIGH);
    Blynk.logEvent("firedetected", "Fire has been Detected");

  }
  else
  {
    Serial.println("✅ Environment considered safe");
    digitalWrite(BPIN, LOW);
  }

  delay(1000);
}

