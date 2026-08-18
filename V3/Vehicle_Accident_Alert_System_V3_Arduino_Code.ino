/*


██╗███╗░░██╗███╗░░██╗░█████╗░██╗░░░██╗░█████╗░░█████╗░██████╗░███████╗
██║████╗░██║████╗░██║██╔══██╗██║░░░██║██╔══██╗██╔══██╗██╔══██╗╚════██║
██║██╔██╗██║██╔██╗██║██║░░██║╚██╗░██╔╝╚█████╔╝██║░░██║██████╔╝░░███╔═╝
██║██║╚████║██║╚████║██║░░██║░╚████╔╝░██╔══██╗██║░░██║██╔══██╗██╔══╝░░
██║██║░╚███║██║░╚███║╚█████╔╝░░╚██╔╝░░╚█████╔╝╚█████╔╝██║░░██║███████╗
╚═╝╚═╝░░╚══╝╚═╝░░╚══╝░╚════╝░░░░╚═╝░░░░╚════╝░░╚════╝░╚═╝░░╚═╝╚══════╝

Instagram: https://www.instagram.com/sahadnisham.k/
LinkedIn : https://www.linkedin.com/in/sahad-nisham-k/

*/

// Vehicle Accident Alert System with IR Obstacle Alert
// Using ADXL345 + GPS + SIM800L + IR Sensor
// Author: Sahad Nisham K

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <math.h>

// ================= PHONE =================
const String EMERGENCY_PHONE = "+918590660687";

// ================= SERIAL =================
#define GSM_RX 3
#define GSM_TX 2
#define GPS_RX 9
#define GPS_TX 10

SoftwareSerial sim800(GSM_RX, GSM_TX);
SoftwareSerial neogps(GPS_TX, GPS_RX);
TinyGPSPlus gps;

// ================= ADXL345 =================
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// ================= PINS =================
#define BUZZER 12
#define BUTTON 11
#define IR_PIN 6   // IR sensor OUT connected to D6

// ================= VARIABLES =================
float prevX = 0, prevY = 0, prevZ = 0;
float sensitivity = 18.0;   // Accident sensitivity
bool impact_detected = false;

unsigned long impact_time = 0;
unsigned long alert_delay = 3000;

String latitude = "";
String longitude = "";

// ================= SETUP =================
void setup() {
  Serial.begin(9600);
  sim800.begin(9600);
  neogps.begin(9600);

  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(IR_PIN, INPUT);

  Serial.println("Initializing System...");

  // -------- ADXL345 INIT --------
  if (!accel.begin()) {
    Serial.println("❌ ADXL345 not detected!");
    while (1);
  }
  accel.setRange(ADXL345_RANGE_16_G);
  Serial.println("✅ ADXL345 Initialized");

  // -------- GSM INIT --------
  sim800.println("AT");
  delay(1000);
  sim800.println("AT+CMGF=1");
  delay(1000);
  Serial.println("✅ GSM Ready");

  Serial.println("🚗 System Ready");
}

// ================= LOOP =================
void loop() {

  detectImpact();

  // IR obstacle alert only if no accident
  if (!impact_detected) {
    irObstacleAlert();
  }

  if (impact_detected) {
    if (millis() - impact_time >= alert_delay) {
      digitalWrite(BUZZER, LOW);

      Serial.println("📞 Calling emergency number...");
      makeCall();

      delay(3000);

      Serial.println("📩 Sending SMS...");
      sendAlert();

      impact_detected = false;
    }
  }

  // Manual reset
  if (digitalRead(BUTTON) == LOW) {
    Serial.println("🔄 System Reset");
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    delay(500);
  }
}

// ================= IMPACT DETECTION =================
void detectImpact() {

  sensors_event_t event;
  accel.getEvent(&event);

  float dx = event.acceleration.x - prevX;
  float dy = event.acceleration.y - prevY;
  float dz = event.acceleration.z - prevZ;

  float magnitude = sqrt(dx * dx + dy * dy + dz * dz);

  prevX = event.acceleration.x;
  prevY = event.acceleration.y;
  prevZ = event.acceleration.z;

  Serial.print("Accel Mag: ");
  Serial.println(magnitude);

  if (magnitude > sensitivity && !impact_detected) {
    Serial.println("🚨 Accident Detected!");
    digitalWrite(BUZZER, HIGH);
    impact_detected = true;
    impact_time = millis();

    Serial.println("📍 Fetching GPS...");
    getGps();
  }

  delay(100);
}

// ================= IR OBSTACLE ALERT =================
void irObstacleAlert() {

  int irState = digitalRead(IR_PIN);

  // Most IR modules: LOW = obstacle detected
  if (irState == LOW) {
    Serial.println("⚠ IR Obstacle Detected");
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
    digitalWrite(BUZZER, HIGH);
    delay(100);
     digitalWrite(BUZZER, LOW);
    delay(500);
  } else {
    digitalWrite(BUZZER, LOW);
  }
}

// ================= GPS =================
void getGps() {

  bool newData = false;

  for (unsigned long start = millis(); millis() - start < 2000;) {
    while (neogps.available()) {
      if (gps.encode(neogps.read())) {
        newData = true;
        break;
      }
    }
  }

  if (newData) {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
    Serial.println("✅ GPS Locked");
    Serial.print("Lat: "); Serial.println(latitude);
    Serial.print("Lon: "); Serial.println(longitude);
  } else {
    Serial.println("⚠ GPS Not Available");
    latitude = "";
    longitude = "";
  }
}

// ================= ALERT =================
void sendAlert() {
  String sms = "Accident Alert!\n";
  sms += "Location:\n";
  sms += "http://maps.google.com/maps?q=loc:";
  sms += latitude + "," + longitude;

  sendSms(sms);
}

void makeCall() {
  Serial.println("📞 Dialing...");
  sim800.print("ATD");
  sim800.print(EMERGENCY_PHONE);
  sim800.println(";");

  delay(1000);

  while (sim800.available()) {
    Serial.write(sim800.read());  // See ERROR / NO CARRIER
  }

  delay(30000);  // Call duration
  sim800.println("ATH");
}


void sendSms(String text) {
  sim800.print("AT+CMGS=\"" + EMERGENCY_PHONE + "\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A);
  delay(1000);
  Serial.println("✅ SMS Sent");
}
